#include "deko3d_renderer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <switch.h>

// Interfaz externa con psxrecomp
extern "C" {
#include "gpu.h"
#include "gpu_render.h"
#include "gpu_sw_renderer.h"
}

// Estructura de cabecera de archivo compilado .dksh
struct DkshHeader {
    uint32_t magic;
    uint32_t header_sz;
    uint32_t control_sz;
    uint32_t code_sz;
    uint32_t programs_off;
    uint32_t num_programs;
};

// Convertidor de color de 16-bit BGR555 de PS1 a RGBA8
static inline void PS1ColorToRGBA8(uint16_t c, uint8_t rgba[4]) {
    uint8_t r = (c & 0x1F) << 3;
    uint8_t g = ((c >> 5) & 0x1F) << 3;
    uint8_t b = ((c >> 10) & 0x1F) << 3;
    rgba[0] = r | (r >> 5);
    rgba[1] = g | (g >> 5);
    rgba[2] = b | (b >> 5);
    rgba[3] = 255;
}

// Convertidor de 24-bit RGB de PS1 a RGBA8
static inline void PS1Color24ToRGBA8(uint32_t c, uint8_t rgba[4]) {
    rgba[0] = (uint8_t)(c & 0xFF);
    rgba[1] = (uint8_t)((c >> 8) & 0xFF);
    rgba[2] = (uint8_t)((c >> 16) & 0xFF);
    rgba[3] = 255;
}

static void DekoDebugCallback(void* userData, const char* context, DkResult result, const char* message) {
    printf("[Deko3D Debug] ctx='%s' res=%d msg='%s'\n", context ? context : "", (int)result, message ? message : "");
    fflush(stdout);
}

static void DekoCmdBufAddMem(void* userData, DkCmdBuf cmdbuf, size_t minReqSize) {
    if (userData) {
        static_cast<Deko3DRenderer*>(userData)->OnCmdBufAddMem(cmdbuf, minReqSize);
    }
}

void Deko3DRenderer::OnCmdBufAddMem(DkCmdBuf cmdbuf, size_t minReqSize) {
    uint32_t allocSize = (uint32_t)((minReqSize + 0x100000 - 1) & ~(0x100000 - 1));
    if (allocSize < 0x200000) allocSize = 0x200000;

    if (m_cmdPoolOffset + allocSize <= CmdPoolTotalSize) {
        ::dkCmdBufAddMemory(cmdbuf, m_memData, m_cmdPoolOffset, allocSize);
        printf("[Deko3D] Dynamic CmdBuf extension: size=0x%X (pool used=0x%X/0x%X)\n",
               allocSize, m_cmdPoolOffset + allocSize, CmdPoolTotalSize);
        m_cmdPoolOffset += allocSize;
        fflush(stdout);
    } else {
        printf("[Deko3D] ERROR FATAL: Command pool agotado en OnCmdBufAddMem (req=%zu)!\n", minReqSize);
        fflush(stdout);
    }
}

Deko3DRenderer& Deko3DRenderer::Instance() {
    static Deko3DRenderer s_instance;
    return s_instance;
}

bool Deko3DRenderer::LoadShader(dk::Shader& shader, dk::MemBlock& block, uint32_t offset, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("[Deko3D] Error: no se pudo abrir shader en %s\n", path);
        return false;
    }

    DkshHeader hdr{};
    if (fread(&hdr, sizeof(hdr), 1, f) != 1) {
        fclose(f);
        return false;
    }

    std::vector<uint8_t> control(hdr.control_sz);
    rewind(f);
    if (fread(control.data(), 1, hdr.control_sz, f) != hdr.control_sz) {
        fclose(f);
        return false;
    }

    uint8_t* codeDst = (uint8_t*)block.getCpuAddr() + offset;
    if (fread(codeDst, 1, hdr.code_sz, f) != hdr.code_sz) {
        fclose(f);
        return false;
    }
    fclose(f);

    dk::ShaderMaker{block, offset}
        .setControl(control.data())
        .setProgramId(0)
        .initialize(shader);

    printf("[Deko3D] Shader cargado exitosamente: %s (code_sz=%u, control_sz=%u)\n", path, hdr.code_sz, hdr.control_sz);
    fflush(stdout);
    return true;
}

bool Deko3DRenderer::Init(uint16_t* vramPtr) {
    if (m_initialized) {
        m_vram = vramPtr;
        return true;
    }
    m_vram = vramPtr;
    printf("[Deko3D] Inicializando pipeline gráfico acelerado por GPU Tegra X1...\n");
    fflush(stdout);

    // 1. Crear Device con depurador y Graphics Queue
    m_device = dk::DeviceMaker{}.setCbDebug(DekoDebugCallback).create();
    m_queue = dk::QueueMaker{m_device}
        .setFlags(DkQueueFlags_Graphics)
        .setCommandMemorySize(1024 * 1024)
        .create();

    // 2. Asignar Memoria GPU
    // Bloque para Framebuffers y Texturas de VRAM (32 MB)
    m_memImages = dk::MemBlockMaker{m_device, 32 * 1024 * 1024}
        .setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
        .create();

    // Bloque para Shaders (128 KB ejecutable)
    m_memCode = dk::MemBlockMaker{m_device, 128 * 1024}
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code)
        .create();

    // Bloque para Comandos, Buffers de Vértices, Descriptores y UBOs (32 MB)
    m_memData = dk::MemBlockMaker{m_device, 32 * 1024 * 1024}
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
        .create();

    // Bloque de staging con caché de CPU activa (64 MB)
    m_memStaging = dk::MemBlockMaker{m_device, 64 * 1024 * 1024}
        .setFlags(DkMemBlockFlags_CpuCached | DkMemBlockFlags_GpuCached)
        .create();

    // 3. Crear Command Buffer con callback de memoria dinámica para prevenir overflow
    m_cmdBuf = dk::CmdBufMaker{m_device}
        .setUserData(this)
        .setCbAddMem(DekoCmdBufAddMem)
        .create();

    // 4. Configurar Layout de Framebuffers (1280x720 RGBA8)
    dk::ImageLayout fbLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(TargetWidth, TargetHeight)
        .initialize(fbLayout);

    uint64_t fbSize = fbLayout.getSize();
    uint32_t fbAlign = fbLayout.getAlignment();
    uint64_t curImgOffset = 0;

    std::array<DkImage const*, NumFramebuffers> fbPointers;
    for (unsigned i = 0; i < NumFramebuffers; i++) {
        curImgOffset = (curImgOffset + fbAlign - 1) & ~(fbAlign - 1);
        m_framebuffers[i].initialize(fbLayout, m_memImages, curImgOffset);
        curImgOffset += fbSize;
        fbPointers[i] = &m_framebuffers[i];
    }

    // 5. Configurar Textura Render Target VRAM (1024x512 RGBA8)
    dk::ImageLayout vramLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_BlockLinear | DkImageFlags_UsageRender | DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(VramWidth, VramHeight)
        .initialize(vramLayout);

    curImgOffset = (curImgOffset + vramLayout.getAlignment() - 1) & ~(vramLayout.getAlignment() - 1);
    m_vramTexture.initialize(vramLayout, m_memImages, curImgOffset);
    curImgOffset += vramLayout.getSize();

    // 6. Configurar Textura Raw VRAM (1024x512 R16_Uint) para muestreo de texturas y CLUT
    dk::ImageLayout rawLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_BlockLinear | DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_R16_Uint)
        .setDimensions(VramWidth, VramHeight)
        .initialize(rawLayout);

    curImgOffset = (curImgOffset + rawLayout.getAlignment() - 1) & ~(rawLayout.getAlignment() - 1);
    m_rawVramImage.initialize(rawLayout, m_memImages, curImgOffset);
    curImgOffset += rawLayout.getSize();

    // 7. Crear Swapchain
    m_swapchain = dk::SwapchainMaker{m_device, nwindowGetDefault(), fbPointers.data(), NumFramebuffers}.create();

    // 8. Cargar Shaders compilados
    uint32_t vshOffset = 0;
    uint32_t fshOffset = 0x1000;
    if (!LoadShader(m_vsh, m_memCode, vshOffset, "romfs:/shaders/psx_vsh.dksh")) {
        printf("[Deko3D] Error crítico: fallo al cargar romfs:/shaders/psx_vsh.dksh\n");
    }
    if (!LoadShader(m_fsh, m_memCode, fshOffset, "romfs:/shaders/psx_fsh.dksh")) {
        printf("[Deko3D] Error crítico: fallo al cargar romfs:/shaders/psx_fsh.dksh\n");
    }

    // 9. Mapear buffers en m_memData (32 MB):
    // 0x00000000..0x01000000: Command Buffer Pool (16 MB: 4 MB inicial + 12 MB extensiones)
    // 0x01000000..0x01800000: Vertex Ring Buffers (8 MB: hasta 131,072 vertices)
    // 0x01E00000: Image Descriptor Set
    // 0x01E00100: Sampler Descriptor Set
    // 0x01E00200: ViewportBlock UBO
    uint8_t* dataCpu = (uint8_t*)m_memData.getCpuAddr();
    DkGpuAddr dataGpu = m_memData.getGpuAddr();

    m_vtxRingCpu = (PS1Vertex*)(dataCpu + 0x01000000);
    m_vtxRingGpu = dataGpu + 0x01000000;
    m_vtxAllocated = 0;
    m_vtxBatchStart = 0;

    m_imgDescCpu = dataCpu + 0x01E00000;
    m_imgDescGpu = dataGpu + 0x01E00000;
    m_smpDescCpu = dataCpu + 0x01E00100;
    m_smpDescGpu = dataGpu + 0x01E00100;

    m_viewportUboCpu = (ViewportUniforms*)(dataCpu + 0x01E00200);
    m_viewportUboGpu = dataGpu + 0x01E00200;

    m_vramStagingCpu = (uint32_t*)m_memStaging.getCpuAddr();
    m_vramStagingGpu = m_memStaging.getGpuAddr();

    // Inicializar descriptores en GPU usando pushData y comando en cola
    dk::ImageView rawView{ m_rawVramImage };
    dk::ImageDescriptor imgDesc;
    imgDesc.initialize(rawView);
    memcpy(m_imgDescCpu, &imgDesc, sizeof(imgDesc));

    dk::Sampler sampler;
    sampler.setFilter(DkFilter_Nearest, DkFilter_Nearest);
    sampler.setWrapMode(DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge);
    dk::SamplerDescriptor smpDesc;
    smpDesc.initialize(sampler);
    memcpy(m_smpDescCpu, &smpDesc, sizeof(smpDesc));

    m_cmdBuf.clear();
    m_cmdPoolOffset = CmdBufferSize;
    m_cmdBuf.addMemory(m_memData, 0, CmdBufferSize);
    m_cmdBuf.pushData(m_imgDescGpu, &imgDesc, sizeof(imgDesc));
    m_cmdBuf.pushData(m_smpDescGpu, &smpDesc, sizeof(smpDesc));
    m_cmdBuf.bindImageDescriptorSet(m_imgDescGpu, 1);
    m_cmdBuf.bindSamplerDescriptorSet(m_smpDescGpu, 1);
    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Descriptors);

    // Limpiar m_vramTexture a negro inicial
    dk::ImageView vramTarget{ m_vramTexture };
    m_cmdBuf.bindRenderTargets(&vramTarget);
    m_cmdBuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);
    m_cmdBuf.bindRenderTargets({});
    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image);

    m_queue.submitCommands(m_cmdBuf.finishList());
    m_queue.waitIdle();

    m_cmdBuf.clear();
    m_cmdPoolOffset = CmdBufferSize;
    m_cmdBuf.addMemory(m_memData, 0, CmdBufferSize);
    m_curSlot = m_queue.acquireImage(m_swapchain);

    m_vtxAllocated = 0;
    m_vtxBatchStart = 0;
    m_stagingUploadOffset = StagingRingStart;
    m_vramDirty = true;
    m_initialized = true;

    printf("[Deko3D] Hardware GPU Renderer activo (60 FPS target).\n");
    return true;
}

void Deko3DRenderer::BeginFrame() {
    m_queue.waitIdle();
    m_cmdBuf.clear();
    m_cmdPoolOffset = CmdBufferSize;
    m_cmdBuf.addMemory(m_memData, 0, CmdBufferSize);

    m_curSlot = m_queue.acquireImage(m_swapchain);

    m_vtxAllocated = 0;
    m_vtxBatchStart = 0;
    m_stagingUploadOffset = StagingRingStart;
}

void Deko3DRenderer::Shutdown() {
    if (m_queue) m_queue.waitIdle();
    m_swapchain.destroy();
    m_cmdBuf.destroy();
    m_memStaging.destroy();
    m_memData.destroy();
    m_memCode.destroy();
    m_memImages.destroy();
    m_queue.destroy();
    m_device.destroy();
}

void Deko3DRenderer::SetDrawOffset(int x, int y) {
    if (m_offsetX != x || m_offsetY != y) {
        FlushBatch();
        m_offsetX = x;
        m_offsetY = y;
    }
}

void Deko3DRenderer::SetDrawArea(int x1, int y1, int x2, int y2) {
    if (m_clipX1 != x1 || m_clipY1 != y1 || m_clipX2 != x2 || m_clipY2 != y2) {
        FlushBatch();
        m_clipX1 = x1;
        m_clipY1 = y1;
        m_clipX2 = x2;
        m_clipY2 = y2;
    }
}

void Deko3DRenderer::SetTextureWindow(uint32_t raw) {
    int mask_x = (int)(raw & 0x1F);
    int mask_y = (int)((raw >> 5) & 0x1F);
    int off_x  = (int)((raw >> 10) & 0x1F);
    int off_y  = (int)((raw >> 15) & 0x1F);
    if (m_twMaskX != mask_x || m_twMaskY != mask_y || m_twOffX != off_x || m_twOffY != off_y) {
        FlushBatch();
        m_twMaskX = mask_x;
        m_twMaskY = mask_y;
        m_twOffX = off_x;
        m_twOffY = off_y;
    }
}

void Deko3DRenderer::SetSemiTransparency(int enabled, int mode) {
    if (m_semiEnabled != enabled || m_semiMode != mode) {
        FlushBatch();
        m_semiEnabled = enabled;
        m_semiMode = mode;
    }
}

void Deko3DRenderer::SetColorModulation(int r, int g, int b, int raw) {
    m_modR = r;
    m_modG = g;
    m_modB = b;
    m_rawTex = raw;
}

void Deko3DRenderer::PushTriangle(const PS1Vertex& v0, const PS1Vertex& v1, const PS1Vertex& v2) {
    if (m_vtxAllocated + 3 > MaxVerticesPerFrame) {
        FlushBatch();
        m_queue.submitCommands(m_cmdBuf.finishList());
        m_queue.waitIdle();
        m_cmdBuf.clear();
        m_cmdPoolOffset = CmdBufferSize;
        m_cmdBuf.addMemory(m_memData, 0, CmdBufferSize);
        m_vtxAllocated = 0;
        m_vtxBatchStart = 0;
    }

    m_vtxRingCpu[m_vtxAllocated + 0] = v0;
    m_vtxRingCpu[m_vtxAllocated + 1] = v1;
    m_vtxRingCpu[m_vtxAllocated + 2] = v2;
    m_vtxAllocated += 3;
    __asm__ __volatile__("dsb sy" ::: "memory");
}

void Deko3DRenderer::SyncRawVram() {
    if (!m_vramDirty || !m_vram) return;

    // PS1 VRAM es exactamente 1024x512 palabras de 16 bits = 1,048,576 bytes (1 MB)
    const size_t rawVramSize = VramWidth * VramHeight * sizeof(uint16_t);
    const uint32_t rawOffset = RawVramStagingOffset; // 0x00600000 en m_memStaging
    uint8_t* dst = (uint8_t*)m_memStaging.getCpuAddr() + rawOffset;

    memcpy(dst, m_vram, rawVramSize);
    m_memStaging.flushCpuCache(rawOffset, rawVramSize);

    dk::ImageView rawView{ m_rawVramImage };
    // Con w = 1024, el pitch = 1024 * 2 = 2048 bytes (alineado a 512 bytes, múltiplo perfecto para Maxwell 2D)
    m_cmdBuf.copyBufferToImage(
        { m_memStaging.getGpuAddr() + rawOffset, 0, 0 },
        rawView,
        { 0, 0, 0, VramWidth, VramHeight, 1 }
    );
    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);

    static uint64_t s_sync_count = 0;
    s_sync_count++;
    if (s_sync_count <= 5 || (s_sync_count % 300) == 0) {
        uint32_t nz = 0;
        for (size_t i = 0; i < VramWidth * VramHeight; i++) {
            if (m_vram[i]) nz++;
        }
        printf("[Deko3D] SyncRawVram #%llu: non_zero_texels=%u / 524288\n",
               (unsigned long long)s_sync_count, nz);
        fflush(stdout);
    }

    m_vramDirty = false;
}

void Deko3DRenderer::FlushBatch() {
    uint32_t count = m_vtxAllocated - m_vtxBatchStart;
    if (count == 0) return;

    SyncRawVram();

    // 1. Configurar Uniforms de Viewport y Texture Window
    m_viewportUboCpu->screenSize[0] = (float)VramWidth;
    m_viewportUboCpu->screenSize[1] = (float)VramHeight;
    m_viewportUboCpu->offset[0] = (float)m_offsetX;
    m_viewportUboCpu->offset[1] = (float)m_offsetY;
    m_viewportUboCpu->texWindow[0] = m_twMaskX;
    m_viewportUboCpu->texWindow[1] = m_twMaskY;
    m_viewportUboCpu->texWindow[2] = m_twOffX;
    m_viewportUboCpu->texWindow[3] = m_twOffY;

    m_cmdBuf.pushConstants(m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT, 0, sizeof(ViewportUniforms), m_viewportUboCpu);

    // 2. Vincular Render Target (m_vramTexture RGBA8 1024x512)
    dk::ImageView vramTarget{ m_vramTexture };
    m_cmdBuf.bindRenderTargets(&vramTarget);

    DkViewport viewport{ 0.0f, 0.0f, (float)VramWidth, (float)VramHeight, 0.0f, 1.0f };
    m_cmdBuf.setViewports(0, { viewport });

    // 3. Configurar Scissor (primitivos absolutos como FillRect ignoran el clip de dibujo en PS1)
    bool isAbsolute = ((m_vtxRingCpu[m_vtxBatchStart].flags & 0x80000000u) != 0);
    if (isAbsolute) {
        DkScissor scissor{ 0, 0, VramWidth, VramHeight };
        m_cmdBuf.setScissors(0, { scissor });
    } else {
        int scX = m_clipX1;
        int scY = m_clipY1;
        int scX2 = m_clipX2;
        int scY2 = m_clipY2;

        if (scX < 0) scX = 0;
        if (scY < 0) scY = 0;
        if (scX2 >= (int)VramWidth) scX2 = (int)VramWidth - 1;
        if (scY2 >= (int)VramHeight) scY2 = (int)VramHeight - 1;

        int scW = scX2 - scX + 1;
        int scH = scY2 - scY + 1;

        if (scW <= 0 || scH <= 0 || scX >= (int)VramWidth || scY >= (int)VramHeight) {
            scX = 0; scY = 0; scW = 0; scH = 0;
        }
        DkScissor scissor{ (uint32_t)scX, (uint32_t)scY, (uint32_t)scW, (uint32_t)scH };
        m_cmdBuf.setScissors(0, { scissor });
    }

    // 4. Configurar Blending de PS1
    dk::BlendState bState;
    if (m_semiEnabled) {
        if (m_semiMode == 0) {
            bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
            bState.setFactors(DkBlendFactor_ConstAlpha, DkBlendFactor_ConstAlpha, DkBlendFactor_One, DkBlendFactor_Zero);
            m_cmdBuf.setBlendConst(0.5f, 0.5f, 0.5f, 0.5f);
        } else if (m_semiMode == 1) {
            bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
            bState.setFactors(DkBlendFactor_One, DkBlendFactor_One, DkBlendFactor_One, DkBlendFactor_Zero);
        } else if (m_semiMode == 2) {
            bState.setOps(DkBlendOp_RevSub, DkBlendOp_Add);
            bState.setFactors(DkBlendFactor_One, DkBlendFactor_One, DkBlendFactor_One, DkBlendFactor_Zero);
        } else if (m_semiMode == 3) {
            bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
            bState.setFactors(DkBlendFactor_ConstAlpha, DkBlendFactor_One, DkBlendFactor_One, DkBlendFactor_Zero);
            m_cmdBuf.setBlendConst(0.25f, 0.25f, 0.25f, 0.25f);
        }
    } else {
        bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
        bState.setFactors(DkBlendFactor_One, DkBlendFactor_Zero, DkBlendFactor_One, DkBlendFactor_Zero);
    }
    m_cmdBuf.bindBlendStates(0, { bState });

    // 5. Configurar Rasterizador, Profundidad y Máscara de Color
    dk::RasterizerState rState;
    rState.setCullMode(DkFace_None);
    m_cmdBuf.bindRasterizerState(rState);

    dk::DepthStencilState dsState;
    dsState.setDepthTestEnable(false);
    dsState.setDepthWriteEnable(false);
    dsState.setStencilTestEnable(false);
    m_cmdBuf.bindDepthStencilState(dsState);

    dk::ColorState cState;
    if (m_semiEnabled) {
        cState.setBlendEnable(0, true);
    }
    m_cmdBuf.bindColorState(cState);

    dk::ColorWriteState cwState;
    m_cmdBuf.bindColorWriteState(cwState);

    // 6. Vincular Shaders, Descriptores y Uniform Buffers
    m_cmdBuf.bindShaders(DkStageFlag_GraphicsMask, { &m_vsh, &m_fsh });

    m_cmdBuf.bindUniformBuffer(DkStage_Vertex, 0, m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT);
    m_cmdBuf.bindUniformBuffer(DkStage_Fragment, 0, m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT);

    m_cmdBuf.bindImageDescriptorSet(m_imgDescGpu, 1);
    m_cmdBuf.bindSamplerDescriptorSet(m_smpDescGpu, 1);
    m_cmdBuf.bindTextures(DkStage_Fragment, 0, dkMakeTextureHandle(0, 0));

    // 7. Configurar Formato de Vértices y Buffer
    static const std::array<DkVtxAttribState, 5> vtxAttribState = {{
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, pos),        DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, uv),         DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, color),      DkVtxAttribSize_4x8,  DkVtxAttribType_Unorm, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, tpage_clut), DkVtxAttribSize_2x16, DkVtxAttribType_Uint,  0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, flags),      DkVtxAttribSize_1x32, DkVtxAttribType_Uint,  0 },
    }};
    DkVtxBufferState vtxBufferState{ sizeof(PS1Vertex), 0 };
    m_cmdBuf.bindVtxAttribState(vtxAttribState);
    m_cmdBuf.bindVtxBufferState({ vtxBufferState });

    m_cmdBuf.bindVtxBuffers(0, { { m_vtxRingGpu, (uint32_t)(MaxVerticesPerFrame * sizeof(PS1Vertex)) } });

    // 8. Barrera de memoria y ejecución del Draw Call
    __asm__ __volatile__("dsb sy" ::: "memory");
    m_cmdBuf.barrier(DkBarrier_Primitives, DkInvalidateFlags_Shader | DkInvalidateFlags_Descriptors | DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
    m_cmdBuf.draw(DkPrimitive_Triangles, count, 1, m_vtxBatchStart, 0);

    static uint64_t s_flush_count = 0;
    s_flush_count++;
    if (s_flush_count <= 10 || (s_flush_count % 300) == 0) {
        uint16_t tp = m_vtxRingCpu[m_vtxBatchStart].tpage_clut[0];
        uint16_t clut = m_vtxRingCpu[m_vtxBatchStart].tpage_clut[1];
        uint32_t clut_x = (clut & 0x3Fu) * 16u;
        uint32_t clut_y = (clut >> 6u) & 0x1FFu;
        uint16_t c0 = m_vram ? m_vram[clut_y * VramWidth + clut_x] : 0;
        uint16_t c1 = m_vram ? m_vram[clut_y * VramWidth + clut_x + 1] : 0;
        printf("[Deko3D] FlushBatch #%llu: cnt=%u clip=(%d,%d..%d,%d) abs=%d v0=(%.1f,%.1f) uv0=(%.1f,%.1f) tp=0x%x clut=0x%x(c0=0x%04x,c1=0x%04x) flags=0x%x\n",
               (unsigned long long)s_flush_count, count,
               m_clipX1, m_clipY1, m_clipX2, m_clipY2, isAbsolute ? 1 : 0,
               m_vtxRingCpu[m_vtxBatchStart].pos[0], m_vtxRingCpu[m_vtxBatchStart].pos[1],
               m_vtxRingCpu[m_vtxBatchStart].uv[0], m_vtxRingCpu[m_vtxBatchStart].uv[1],
               tp, clut, c0, c1,
               m_vtxRingCpu[m_vtxBatchStart].flags);
        fflush(stdout);
    }

    m_vtxBatchStart = m_vtxAllocated;
}

void Deko3DRenderer::DrawFlatTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    uint8_t rgba[4];
    PS1ColorToRGBA8(color, rgba);

    PS1Vertex v0 = { {(float)x0, (float)y0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, 0, 0 };
    PS1Vertex v1 = { {(float)x1, (float)y1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, 0, 0 };
    PS1Vertex v2 = { {(float)x2, (float)y2}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, 0, 0 };

    PushTriangle(v0, v1, v2);
}

void Deko3DRenderer::DrawGouraudTriangle(int x0, int y0, uint16_t c0,
                                        int x1, int y1, uint16_t c1,
                                        int x2, int y2, uint16_t c2) {
    uint8_t rgba0[4], rgba1[4], rgba2[4];
    PS1ColorToRGBA8(c0, rgba0);
    PS1ColorToRGBA8(c1, rgba1);
    PS1ColorToRGBA8(c2, rgba2);

    PS1Vertex v0 = { {(float)x0, (float)y0}, {0.0f, 0.0f}, {rgba0[0], rgba0[1], rgba0[2], 255}, {0, 0}, 0, 0 };
    PS1Vertex v1 = { {(float)x1, (float)y1}, {0.0f, 0.0f}, {rgba1[0], rgba1[1], rgba1[2], 255}, {0, 0}, 0, 0 };
    PS1Vertex v2 = { {(float)x2, (float)y2}, {0.0f, 0.0f}, {rgba2[0], rgba2[1], rgba2[2], 255}, {0, 0}, 0, 0 };

    PushTriangle(v0, v1, v2);
}

void Deko3DRenderer::DrawTexturedTriangle(int x0, int y0, int u0, int v0,
                                         int x1, int y1, int u1, int v1,
                                         int x2, int y2, int u2, int v2,
                                         uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    uint16_t clut = (uint16_t)((clut_x / 16) | (clut_y << 6));
    int depth = (texpage >> 7) & 3;
    uint32_t flags = (uint32_t)depth | (m_rawTex ? 4u : 0u) | (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u) | 0x40u;
    uint8_t rgba[4] = { (uint8_t)m_modR, (uint8_t)m_modG, (uint8_t)m_modB, 255 };

    PS1Vertex vert0 = { {(float)x0, (float)y0}, {(float)u0, (float)v0}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, 0 };
    PS1Vertex vert1 = { {(float)x1, (float)y1}, {(float)u1, (float)v1}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, 0 };
    PS1Vertex vert2 = { {(float)x2, (float)y2}, {(float)u2, (float)v2}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, 0 };

    PushTriangle(vert0, vert1, vert2);
}

void Deko3DRenderer::DrawShadedTexturedTriangle(int x0, int y0, int u0, int v0, uint32_t col0,
                                               int x1, int y1, int u1, int v1, uint32_t col1,
                                               int x2, int y2, int u2, int v2, uint32_t col2,
                                               uint16_t clut_x, uint16_t clut_y, uint16_t texpage, int raw) {
    uint16_t clut = (uint16_t)((clut_x / 16) | (clut_y << 6));
    int depth = (texpage >> 7) & 3;
    uint32_t flags = (uint32_t)depth | (raw ? 4u : 0u) | (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u) | 0x40u;

    uint8_t rgba0[4], rgba1[4], rgba2[4];
    PS1Color24ToRGBA8(col0, rgba0);
    PS1Color24ToRGBA8(col1, rgba1);
    PS1Color24ToRGBA8(col2, rgba2);

    PS1Vertex vert0 = { {(float)x0, (float)y0}, {(float)u0, (float)v0}, {rgba0[0], rgba0[1], rgba0[2], 255}, {texpage, clut}, flags, 0 };
    PS1Vertex vert1 = { {(float)x1, (float)y1}, {(float)u1, (float)v1}, {rgba1[0], rgba1[1], rgba1[2], 255}, {texpage, clut}, flags, 0 };
    PS1Vertex vert2 = { {(float)x2, (float)y2}, {(float)u2, (float)v2}, {rgba2[0], rgba2[1], rgba2[2], 255}, {texpage, clut}, flags, 0 };

    PushTriangle(vert0, vert1, vert2);
}

void Deko3DRenderer::DrawRect(int x, int y, int w, int h, uint16_t color) {
    uint8_t rgba[4];
    PS1ColorToRGBA8(color, rgba);

    float fx0 = (float)x, fy0 = (float)y;
    float fx1 = (float)(x + w), fy1 = (float)(y + h);

    PS1Vertex v0 = { {fx0, fy0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, 0, 0 };
    PS1Vertex v1 = { {fx1, fy0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, 0, 0 };
    PS1Vertex v2 = { {fx0, fy1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, 0, 0 };
    PS1Vertex v3 = { {fx1, fy1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, 0, 0 };

    PushTriangle(v0, v1, v2);
    PushTriangle(v1, v3, v2);
}

void Deko3DRenderer::DrawTexturedRect(int x, int y, int w, int h, int u, int v,
                                     uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    DrawTexturedRectScaled(x, y, w, h, u, v, u + w, v + h, clut_x, clut_y, texpage);
}

void Deko3DRenderer::DrawTexturedRectScaled(int x, int y, int w, int h,
                                           int u0, int v0, int u1, int v1,
                                           uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    uint16_t clut = (uint16_t)((clut_x / 16) | (clut_y << 6));
    int depth = (texpage >> 7) & 3;
    uint32_t flags = (uint32_t)depth | (m_rawTex ? 4u : 0u) | (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u) | 0x40u;
    uint8_t rgba[4] = { (uint8_t)m_modR, (uint8_t)m_modG, (uint8_t)m_modB, 255 };

    float fx0 = (float)x, fy0 = (float)y;
    float fx1 = (float)(x + w), fy1 = (float)(y + h);
    float fu0 = (float)u0, fv0 = (float)v0;
    float fu1 = (float)u1, fv1 = (float)v1;

    PS1Vertex vert0 = { {fx0, fy0}, {fu0, fv0}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, 0 };
    PS1Vertex vert1 = { {fx1, fy0}, {fu1, fv0}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, 0 };
    PS1Vertex vert2 = { {fx0, fy1}, {fu0, fv1}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, 0 };
    PS1Vertex vert3 = { {fx1, fy1}, {fu1, fv1}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, 0 };

    PushTriangle(vert0, vert1, vert2);
    PushTriangle(vert1, vert3, vert2);
}

void Deko3DRenderer::FillRect(int x, int y, int w, int h, uint16_t color) {
    FlushBatch();
    sw_fill_rect(x, y, w, h, color);
    m_vramDirty = true;

    uint8_t rgba[4];
    PS1ColorToRGBA8(color, rgba);

    float fx0 = (float)x, fy0 = (float)y;
    float fx1 = (float)(x + w), fy1 = (float)(y + h);
    uint32_t flags = 0x80000000u; // Coordenadas absolutas, sin textura

    PS1Vertex v0 = { {fx0, fy0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v1 = { {fx1, fy0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v2 = { {fx0, fy1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v3 = { {fx1, fy1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };

    PushTriangle(v0, v1, v2);
    PushTriangle(v1, v3, v2);

    FlushBatch();
}

void Deko3DRenderer::CopyRect(int sx, int sy, int dx, int dy, int w, int h) {
    FlushBatch();
    sw_copy_rect(sx, sy, dx, dy, w, h);
    m_vramDirty = true;
    if (!m_initialized || w <= 0 || h <= 0) return;

    sx &= (VramWidth - 1);
    sy &= (VramHeight - 1);
    dx &= (VramWidth - 1);
    dy &= (VramHeight - 1);
    if (sx + w > (int)VramWidth) w = (int)VramWidth - sx;
    if (sy + h > (int)VramHeight) h = (int)VramHeight - sy;
    if (dx + w > (int)VramWidth) w = (int)VramWidth - dx;
    if (dy + h > (int)VramHeight) h = (int)VramHeight - dy;
    if (w <= 0 || h <= 0) return;

    dk::ImageView vramTexView{ m_vramTexture };

    m_cmdBuf.copyImage(vramTexView, { (uint32_t)sx, (uint32_t)sy, 0, (uint32_t)w, (uint32_t)h, 1 },
                       vramTexView, { (uint32_t)dx, (uint32_t)dy, 0, (uint32_t)w, (uint32_t)h, 1 });
    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image);
}

void Deko3DRenderer::VramTransferIn(int x, int y, int w, int h, const uint16_t* data) {
    if (!m_initialized || !data || w <= 0 || h <= 0) return;

    FlushBatch();

    sw_vram_transfer_in(x, y, w, h, data);
    m_vramDirty = true;

    static uint64_t s_upload_count = 0;
    s_upload_count++;
    if (s_upload_count <= 10 || (s_upload_count % 100) == 0) {
        printf("[Deko3D] VramTransferIn #%llu: rect=(%d,%d %dx%d)\n",
               (unsigned long long)s_upload_count, x, y, w, h);
        fflush(stdout);
    }

    x &= (VramWidth - 1);
    y &= (VramHeight - 1);
    if (x + w > (int)VramWidth) w = (int)VramWidth - x;
    if (y + h > (int)VramHeight) h = (int)VramHeight - y;
    if (w <= 0 || h <= 0) return;

    // Pitch alineado estrictamente a 32 bytes para el Maxwell 2D Copy Engine de Tegra X1
    uint32_t pitch32 = (w * sizeof(uint32_t) + 31u) & ~31u;
    size_t uploadBytes32 = (size_t)pitch32 * h;
    uint32_t align32 = (uint32_t)((uploadBytes32 + 511u) & ~511u);

    // Buffer circular de staging dinámico (StagingRingStart .. StagingRingEnd = 56 MB)
    if (m_stagingUploadOffset + align32 > StagingRingEnd) {
        FlushBatch();
        m_queue.submitCommands(m_cmdBuf.finishList());
        m_queue.waitIdle();
        m_cmdBuf.clear();
        m_cmdPoolOffset = CmdBufferSize;
        m_cmdBuf.addMemory(m_memData, 0, CmdBufferSize);
        m_stagingUploadOffset = StagingRingStart;
        m_vtxAllocated = 0;
        m_vtxBatchStart = 0;
    }

    uint32_t uploadOffset32 = m_stagingUploadOffset;
    m_stagingUploadOffset += align32;

    uint8_t* dst32Base = (uint8_t*)m_memStaging.getCpuAddr() + uploadOffset32;

    for (int row = 0; row < h; row++) {
        const uint16_t* srcRow = data + (size_t)row * w;
        uint32_t* dRow32 = (uint32_t*)(dst32Base + (size_t)row * pitch32);
        for (int col = 0; col < w; col++) {
            uint16_t c = srcRow[col];
            uint32_t r5 = c & 0x1Fu;
            uint32_t g5 = (c >> 5) & 0x1Fu;
            uint32_t b5 = (c >> 10) & 0x1Fu;
            uint32_t r8 = (r5 << 3) | (r5 >> 2);
            uint32_t g8 = (g5 << 3) | (g5 >> 2);
            uint32_t b8 = (b5 << 3) | (b5 >> 2);
            dRow32[col] = r8 | (g8 << 8) | (b8 << 16) | 0xFF000000u;
        }
    }

    m_memStaging.flushCpuCache(uploadOffset32, uploadBytes32);

    dk::ImageView vramTexView{ m_vramTexture };
    m_cmdBuf.copyBufferToImage({ m_memStaging.getGpuAddr() + uploadOffset32, pitch32, 0 },
                               vramTexView,
                               { (uint32_t)x, (uint32_t)y, 0, (uint32_t)w, (uint32_t)h, 1 });

    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
}

int Deko3DRenderer::RenderDisplay(uint32_t* out, int pitch, int dx, int dy, int dw, int dh) {
    if (out != nullptr) {
        return sw_render_display(out, pitch, dx, dy, dw, dh);
    }

    if (!m_initialized) {
        return 0;
    }

    if (m_vtxAllocated > m_vtxBatchStart) {
        FlushBatch();
    }

    GpuDisplayInfo di{};
    gpu_get_display_info(&di);

    uint32_t dispX = di.display_x;
    uint32_t dispY = di.display_y;
    uint32_t w = di.width ? di.width : 320;
    uint32_t h = di.height ? di.height : 240;

    if (dispX >= VramWidth) dispX = 0;
    if (dispY >= VramHeight) dispY = 0;
    if (dispX + w > VramWidth) w = VramWidth - dispX;
    if (dispY + h > VramHeight) h = VramHeight - dispY;
    if (w == 0) w = 1;
    if (h == 0) h = 1;

    dk::ImageView colorTarget{ m_framebuffers[m_curSlot] };

    if (di.disabled) {
        m_cmdBuf.bindRenderTargets(&colorTarget);
        m_cmdBuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        m_cmdBuf.bindRenderTargets({});
        if (di.depth24 && m_vram && m_vramStagingCpu) {
            // Ruta para FMVs de 24-bit
            uint32_t* slotStagingCpu = m_vramStagingCpu + (size_t)m_curSlot * (VramWidth * VramHeight);
            DkGpuAddr slotStagingGpu = m_vramStagingGpu + (size_t)m_curSlot * (VramWidth * VramHeight * sizeof(uint32_t));
            for (uint32_t y = 0; y < h; y++) {
                uint32_t* dstRow = slotStagingCpu + (size_t)y * w;
                gpu_depth24_present_row(&di, y, dstRow, w);
                for (uint32_t x = 0; x < w; x++) {
                    uint32_t argb = dstRow[x];
                    uint32_t r = (argb >> 16) & 0xFFu;
                    uint32_t g = (argb >> 8) & 0xFFu;
                    uint32_t b = argb & 0xFFu;
                    dstRow[x] = r | (g << 8) | (b << 16) | 0xFF000000u;
                }
            }
            m_memStaging.flushCpuCache((size_t)m_curSlot * (VramWidth * VramHeight * sizeof(uint32_t)), (size_t)w * h * sizeof(uint32_t));
            m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_L2Cache);
            dk::ImageView vramTexView{ m_vramTexture };
            m_cmdBuf.copyBufferToImage({ slotStagingGpu, 0, 0 }, vramTexView, { 0, 0, 0, w, h, 1 });
            m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
            m_cmdBuf.blitImage(vramTexView, { 0, 0, 0, w, h, 1 },
                               colorTarget, { 0, 0, 0, TargetWidth, TargetHeight, 1 },
                               DkBlitFlag_FilterLinear);
        } else {
            // Presentación directa por hardware GPU Maxwell (0 ms de conversión CPU)
            dk::ImageView vramTexView{ m_vramTexture };
            m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
            m_cmdBuf.blitImage(vramTexView, { dispX, dispY, 0, w, h, 1 },
                               colorTarget, { 0, 0, 0, TargetWidth, TargetHeight, 1 },
                               DkBlitFlag_FilterLinear);
            m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
        }
    }

    DkCmdList finalCmdList = m_cmdBuf.finishList();
    m_queue.submitCommands(finalCmdList);

    m_queue.presentImage(m_swapchain, m_curSlot);

    static uint64_t s_present_count = 0;
    s_present_count++;
    if (s_present_count <= 10 || (s_present_count % 120) == 0) {
        printf("[Deko3D] Present #%llu disp=(%u,%u,%ux%u) d24=%d dis=%d vtx=%u\n",
               (unsigned long long)s_present_count,
               di.display_x, di.display_y, w, h,
               di.depth24, di.disabled, m_vtxAllocated);
        fflush(stdout);
    }

    BeginFrame();

    return (int)(w * h);
}

// =============================================================================
// Interfaz de backend GpuRenderBackend para psxrecomp
// =============================================================================

static void dk_init(uint16_t* vram) {
    sw_renderer_init(vram);
    Deko3DRenderer::Instance().Init(vram);
}

static void dk_set_draw_offset(int x, int y) {
    sw_set_draw_offset(x, y);
    Deko3DRenderer::Instance().SetDrawOffset(x, y);
}

static void dk_set_draw_area(int x1, int y1, int x2, int y2) {
    sw_set_draw_area(x1, y1, x2, y2);
    Deko3DRenderer::Instance().SetDrawArea(x1, y1, x2, y2);
}

static void dk_set_texture_window(uint32_t raw) {
    sw_set_texture_window(raw);
    Deko3DRenderer::Instance().SetTextureWindow(raw);
}

static void dk_set_semi_transparency(int enabled, int mode) {
    sw_set_semi_transparency(enabled, mode);
    Deko3DRenderer::Instance().SetSemiTransparency(enabled, mode);
}

static void dk_set_color_modulation(int r, int g, int b, int raw) {
    sw_set_color_modulation(r, g, b, raw);
    Deko3DRenderer::Instance().SetColorModulation(r, g, b, raw);
}

static void dk_draw_flat_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    Deko3DRenderer::Instance().DrawFlatTriangle(x0, y0, x1, y1, x2, y2, color);
}

static void dk_draw_gouraud_triangle(int x0, int y0, uint16_t c0,
                                     int x1, int y1, uint16_t c1,
                                     int x2, int y2, uint16_t c2) {
    Deko3DRenderer::Instance().DrawGouraudTriangle(x0, y0, c0, x1, y1, c1, x2, y2, c2);
}

static void dk_draw_textured_triangle(int x0, int y0, int u0, int v0,
                                      int x1, int y1, int u1, int v1,
                                      int x2, int y2, int u2, int v2,
                                      uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    Deko3DRenderer::Instance().DrawTexturedTriangle(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, clut_x, clut_y, texpage);
}

static void dk_draw_shaded_textured_triangle(int x0, int y0, int u0, int v0, uint32_t c0,
                                            int x1, int y1, int u1, int v1, uint32_t c1,
                                            int x2, int y2, int u2, int v2, uint32_t c2,
                                            uint16_t clut_x, uint16_t clut_y, uint16_t texpage, int raw) {
    Deko3DRenderer::Instance().DrawShadedTexturedTriangle(x0, y0, u0, v0, c0, x1, y1, u1, v1, c1, x2, y2, u2, v2, c2, clut_x, clut_y, texpage, raw);
}

static void dk_fill_rect(int x, int y, int w, int h, uint16_t color) {
    Deko3DRenderer::Instance().FillRect(x, y, w, h, color);
}

static void dk_copy_rect(int sx, int sy, int dx, int dy, int w, int h) {
    Deko3DRenderer::Instance().CopyRect(sx, sy, dx, dy, w, h);
}

static void dk_draw_flat_rect(int x, int y, int w, int h, uint16_t color) {
    Deko3DRenderer::Instance().DrawRect(x, y, w, h, color);
}

static void dk_draw_textured_rect(int x, int y, int w, int h, int u, int v,
                                 uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    Deko3DRenderer::Instance().DrawTexturedRect(x, y, w, h, u, v, clut_x, clut_y, texpage);
}

static void dk_draw_textured_rect_scaled(int x, int y, int w, int h,
                                        int u0, int v0, int u1, int v1,
                                        uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    Deko3DRenderer::Instance().DrawTexturedRectScaled(x, y, w, h, u0, v0, u1, v1, clut_x, clut_y, texpage);
}

static void dk_vram_transfer_in(int x, int y, int w, int h, const uint16_t* data) {
    sw_vram_transfer_in(x, y, w, h, data);
    Deko3DRenderer::Instance().VramTransferIn(x, y, w, h, data);
}

static int dk_render_display(uint32_t* out, int pitch, int dx, int dy, int dw, int dh) {
    return Deko3DRenderer::Instance().RenderDisplay(out, pitch, dx, dy, dw, dh);
}

// Vtable del backend Deko3D
static const GpuRenderBackend DEKO3D_BACKEND = {
    .name                           = "deko3d",
    .init                           = dk_init,
    .set_scale                      = sw_renderer_set_scale,
    .scale                          = sw_renderer_scale,
    .set_texture_filter             = sw_set_texture_filter,
    .texture_filter                 = sw_texture_filter,
    .set_semi_transparency          = dk_set_semi_transparency,
    .set_mask_bits                  = sw_set_mask_bits,
    .set_texture_window             = dk_set_texture_window,
    .set_color_modulation           = dk_set_color_modulation,
    .set_precise_triangle           = sw_set_precise_triangle,
    .set_perspective_triangle       = sw_set_perspective_triangle,
    .fill_rect                      = dk_fill_rect,
    .copy_rect                      = dk_copy_rect,
    .draw_flat_triangle             = dk_draw_flat_triangle,
    .draw_gouraud_triangle          = dk_draw_gouraud_triangle,
    .draw_textured_triangle         = dk_draw_textured_triangle,
    .draw_shaded_textured_triangle  = dk_draw_shaded_textured_triangle,
    .draw_flat_rect                 = dk_draw_flat_rect,
    .draw_textured_rect             = dk_draw_textured_rect,
    .draw_textured_rect_scaled      = dk_draw_textured_rect_scaled,
    .draw_line                      = sw_draw_line,
    .draw_shaded_line               = sw_draw_shaded_line,
    .render_display                 = dk_render_display,
    .render_display_hires           = sw_render_display_hires,
    .vram_write                     = sw_vram_write,
    .vram_read                      = sw_vram_read,
    .vram_transfer_in               = dk_vram_transfer_in,
    .vram_transfer_out              = sw_vram_transfer_out,
    .set_draw_area                  = dk_set_draw_area,
    .get_draw_area                  = sw_get_draw_area,
    .set_draw_offset                = dk_set_draw_offset,
    .wide_configure                 = sw_wide_configure,
    .wide_set_target                = sw_wide_set_target,
    .wide_disable_target            = sw_wide_disable_target,
    .wide_set_primitive_delta       = sw_wide_set_primitive_delta,
    .wide_set_primitive_clip_bottom = sw_wide_set_primitive_clip_bottom,
    .wide_fade_white                = sw_wide_fade_white,
    .wide_set_primitive_suppressed  = sw_wide_set_primitive_suppressed,
    .wide_set_primitive_unclipped   = sw_wide_set_primitive_unclipped,
    .wide_set_primitive_expanded    = sw_wide_set_primitive_expanded,
    .wide_clear                     = sw_wide_clear,
    .wide_clear_margins             = sw_wide_clear_margins,
    .wide_set_mirror_authoritative  = sw_wide_set_mirror_authoritative,
    .wide_set_sidecar_complete      = sw_wide_set_sidecar_complete,
    .render_wide_display            = sw_render_wide_display,
    .wide_dump_full                 = sw_wide_dump_full,
};

const GpuRenderBackend* deko3d_backend_get(void) {
    return &DEKO3D_BACKEND;
}
