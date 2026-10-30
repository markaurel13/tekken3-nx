#include "deko3d_renderer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <cmath>
#include <vector>
#include <algorithm>
#include <switch.h>

// Interfaz externa con psxrecomp
extern "C" {
#include "gpu.h"
#include "gpu_render.h"
#include "gpu_sw_renderer.h"
#include "host_osd.h"
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

    int idx = (cmdbuf == (DkCmdBuf)m_cmdBufPool[0]) ? 0 : 1;
    if (m_cmdPoolOffset[idx] + allocSize <= m_cmdPoolBase[idx] + CmdPoolSliceSize) {
        ::dkCmdBufAddMemory(cmdbuf, m_memData, m_cmdPoolOffset[idx], allocSize);
        printf("[Deko3D] Dynamic CmdBuf[%d] extension: size=0x%X (pool used=0x%X/0x%X)\n",
               idx, allocSize, m_cmdPoolOffset[idx] + allocSize - m_cmdPoolBase[idx], CmdPoolSliceSize);
        m_cmdPoolOffset[idx] += allocSize;
        fflush(stdout);
    } else {
        printf("[Deko3D] ERROR FATAL: Command pool[%d] agotado en OnCmdBufAddMem (req=%zu)!\n", idx, minReqSize);
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

bool Deko3DRenderer::Init(uint16_t* vramPtr, uint32_t internalScale, bool widescreen) {
    if (m_initialized) {
        m_vram = vramPtr;
        return true;
    }
    m_vram = vramPtr;

    if (internalScale < 1) internalScale = 1;
    if (internalScale > 3) internalScale = 3;
    m_internalScale = internalScale;
    m_hrVramWidth = VramWidth * m_internalScale;
    m_hrVramHeight = VramHeight * m_internalScale;
    m_widescreen = widescreen;

    printf("[Deko3D] Inicializando pipeline acelerado por GPU (SSAA=%ux [%ux%u], Relacion=%s)...\n",
           m_internalScale, m_hrVramWidth, m_hrVramHeight,
           m_widescreen ? "16:9 Widescreen" : "4:3 Original");
    fflush(stdout);

    // 1. Crear Device con depurador y Graphics Queue
    m_device = dk::DeviceMaker{}.setCbDebug(DekoDebugCallback).create();
    m_queue = dk::QueueMaker{m_device}
        .setFlags(DkQueueFlags_Graphics)
        .setCommandMemorySize(1024 * 1024)
        .create();

    // 2. Asignar Memoria GPU
    // Bloque para Framebuffers y Texturas de VRAM (64 MB para acomodar Framebuffers + VRAM Render Target 2048x1024)
    m_memImages = dk::MemBlockMaker{m_device, 64 * 1024 * 1024}
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

    // 3. Crear Command Buffers con callback de memoria dinámica para prevenir overflow (Doble Búfer)
    for (unsigned i = 0; i < NumFrameResources; i++) {
        m_cmdBufPool[i] = dk::CmdBufMaker{m_device}
            .setUserData(this)
            .setCbAddMem(DekoCmdBufAddMem)
            .create();
        m_cmdPoolBase[i] = i * CmdPoolSliceSize;
        m_cmdPoolOffset[i] = m_cmdPoolBase[i] + CmdBufferSize;
        m_fenceInFlight[i] = false;
    }

    // 4. Configurar Layout de Framebuffers (1920x1080 RGBA8 con soporte dinámico 1080p Docked / 720p Handheld)
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

    // 5. Configurar Textura Render Target VRAM (m_hrVramWidth x m_hrVramHeight RGBA8 con m_internalScale)
    dk::ImageLayout vramLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_BlockLinear | DkImageFlags_UsageRender | DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(m_hrVramWidth, m_hrVramHeight)
        .initialize(vramLayout);

    curImgOffset = (curImgOffset + vramLayout.getAlignment() - 1) & ~(vramLayout.getAlignment() - 1);
    m_vramTexture.initialize(vramLayout, m_memImages, curImgOffset);
    curImgOffset += vramLayout.getSize();

    // 6. Configurar Textura Raw VRAM (1024x1024 R16_Uint: y=0..511 estándar, y=512..1023 High VRAM)
    dk::ImageLayout rawLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_BlockLinear | DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_R16_Uint)
        .setDimensions(VramWidth, RawVramHeight)
        .initialize(rawLayout);

    curImgOffset = (curImgOffset + rawLayout.getAlignment() - 1) & ~(rawLayout.getAlignment() - 1);
    m_rawVramImage.initialize(rawLayout, m_memImages, curImgOffset);
    curImgOffset += rawLayout.getSize();

    // 6b. Configurar Textura OSD RGBA8 (1024x1024 RGBA8_Unorm) para carrusel de trajes/retratos y avisos
    dk::ImageLayout osdLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_BlockLinear | DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(OsdMaxWidth, OsdMaxHeight)
        .initialize(osdLayout);

    curImgOffset = (curImgOffset + osdLayout.getAlignment() - 1) & ~(osdLayout.getAlignment() - 1);
    m_osdImage.initialize(osdLayout, m_memImages, curImgOffset);
    curImgOffset += osdLayout.getSize();

    // 7. Configurar NWindow a 1920x1080 máximo y crear Swapchain (con recorte dinámico 720p Handheld / 1080p Docked)
    NWindow* win = nwindowGetDefault();
    nwindowSetDimensions(win, TargetWidth, TargetHeight);
    m_swapchain = dk::SwapchainMaker{m_device, win, fbPointers.data(), NumFramebuffers}.create();

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
    // 0x00000000..0x00800000: Frame 0 Command Pool (8 MB)
    // 0x00800000..0x01000000: Frame 1 Command Pool (8 MB)
    // 0x01000000..0x01400000: Frame 0 Vertex Pool (4 MB = 131,072 vertices)
    // 0x01400000..0x01800000: Frame 1 Vertex Pool (4 MB = 131,072 vertices)
    // 0x01E00000: Image Descriptor Set
    // 0x01E00100: Sampler Descriptor Set
    // 0x01E00200: ViewportBlock UBO
    uint8_t* dataCpu = (uint8_t*)m_memData.getCpuAddr();
    DkGpuAddr dataGpu = m_memData.getGpuAddr();

    for (unsigned i = 0; i < NumFrameResources; i++) {
        m_vtxRingCpuPool[i] = (PS1Vertex*)(dataCpu + 0x01000000 + i * VertexRingSliceSize);
        m_vtxRingGpuPool[i] = dataGpu + 0x01000000 + i * VertexRingSliceSize;
    }
    m_frameResIdx = 0;
    m_cmdBuf = m_cmdBufPool[0];
    m_vtxRingCpu = m_vtxRingCpuPool[0];
    m_vtxRingGpu = m_vtxRingGpuPool[0];
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
    dk::ImageView osdView{ m_osdImage };
    dk::ImageDescriptor imgDescs[2];
    imgDescs[0].initialize(rawView);
    imgDescs[1].initialize(osdView);
    memcpy(m_imgDescCpu, imgDescs, sizeof(imgDescs));

    dk::Sampler sampler;
    sampler.setFilter(DkFilter_Nearest, DkFilter_Nearest);
    sampler.setWrapMode(DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge);
    dk::SamplerDescriptor smpDesc;
    smpDesc.initialize(sampler);
    memcpy(m_smpDescCpu, &smpDesc, sizeof(smpDesc));

    m_cmdBuf.clear();
    m_cmdPoolOffset[0] = m_cmdPoolBase[0] + CmdBufferSize;
    m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[0], CmdBufferSize);
    m_cmdBuf.pushData(m_imgDescGpu, imgDescs, sizeof(imgDescs));
    m_cmdBuf.pushData(m_smpDescGpu, &smpDesc, sizeof(smpDesc));
    m_cmdBuf.bindImageDescriptorSet(m_imgDescGpu, 2);
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
    m_cmdPoolOffset[0] = m_cmdPoolBase[0] + CmdBufferSize;
    m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[0], CmdBufferSize);
    m_curSlot = m_queue.acquireImage(m_swapchain);

    m_vtxAllocated = 0;
    m_vtxBatchStart = 0;
    m_stagingUploadOffset = StagingRingStart;
    m_vramDirty = true;
    m_initialized = true;

    printf("[Deko3D] Hardware GPU Renderer activo (Doble Búfer con Fences y 60 FPS target).\n");
    return true;
}

void Deko3DRenderer::WaitPendingFences() {
    u64 t0 = armGetSystemTick();
    bool waited = false;
    for (unsigned i = 0; i < NumFrameResources; i++) {
        if (m_fenceInFlight[i]) {
            DkResult res = dkFenceWait(&m_frameFences[i], 100000000ULL);
            if (res != DkResult_Success) {
                printf("[Deko3D] WARNING: WaitPendingFences timeout en fence %u (res=%d)!\n", i, (int)res);
            }
            m_fenceInFlight[i] = false;
            waited = true;
        }
    }
    if (waited) {
        u64 t1 = armGetSystemTick();
        m_timings.vram_fence_wait_ns += armTicksToNs(t1 - t0);
    }
}

void Deko3DRenderer::BeginFrame() {
    // Si la fence del buffer de este frame aún no ha terminado, esperar con timeout seguro de 100ms
    u64 t0 = armGetSystemTick();
    if (m_fenceInFlight[m_frameResIdx]) {
        DkResult res = dkFenceWait(&m_frameFences[m_frameResIdx], 100000000ULL);
        if (res != DkResult_Success) {
            printf("[Deko3D] WARNING: BeginFrame timeout en fence %u (res=%d)!\n", m_frameResIdx, (int)res);
        }
        m_fenceInFlight[m_frameResIdx] = false;
    }
    u64 t1 = armGetSystemTick();
    m_timings.gpu_fence_wait_ns += armTicksToNs(t1 - t0);

    m_cmdBuf = m_cmdBufPool[m_frameResIdx];
    m_vtxRingCpu = m_vtxRingCpuPool[m_frameResIdx];
    m_vtxRingGpu = m_vtxRingGpuPool[m_frameResIdx];

    m_cmdBuf.clear();
    m_cmdPoolOffset[m_frameResIdx] = m_cmdPoolBase[m_frameResIdx] + CmdBufferSize;
    m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[m_frameResIdx], CmdBufferSize);

    u64 t2 = armGetSystemTick();
    m_curSlot = m_queue.acquireImage(m_swapchain);
    u64 t3 = armGetSystemTick();
    m_timings.acq_wait_ns += armTicksToNs(t3 - t2);

    m_vtxAllocated = 0;
    m_vtxBatchStart = 0;
    m_stagingUploadOffset = StagingRingStart;
}

void Deko3DRenderer::Shutdown() {
    if (!m_initialized) return;
    m_initialized = false;
    WaitPendingFences();
    if (m_queue) m_queue.waitIdle();
    m_cmdBuf = dk::CmdBuf{};
    m_swapchain.destroy();
    for (unsigned i = 0; i < NumFrameResources; i++) {
        m_cmdBufPool[i].destroy();
    }
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

void Deko3DRenderer::SetPerspectiveTriangle(int enabled, float q0, float q1, float q2) {
    if (enabled && q0 > 0.0f && q1 > 0.0f && q2 > 0.0f) {
        m_pqValid = true;
        m_pq[0] = q0;
        m_pq[1] = q1;
        m_pq[2] = q2;
        m_perspTriHits++;
        static bool s_first_persp_logged = false;
        if (!s_first_persp_logged) {
            s_first_persp_logged = true;
            printf("[Deko3D] Primer triangulo 3D con perspectiva de texturas activa (q0=%.4f q1=%.4f q2=%.4f)\n",
                   q0, q1, q2);
            fflush(stdout);
        }
    } else {
        m_pqValid = false;
        m_pq[0] = 0.0f;
        m_pq[1] = 0.0f;
        m_pq[2] = 0.0f;
    }
    sw_set_perspective_triangle(enabled, q0, q1, q2);
}

void Deko3DRenderer::SetColorModulation(int r, int g, int b, int raw) {
    m_modR = r;
    m_modG = g;
    m_modB = b;
    m_rawTex = raw;
}

void Deko3DRenderer::PushTriangle(const PS1Vertex& v0, const PS1Vertex& v1, const PS1Vertex& v2) {
    if (!std::isfinite(v0.pos[0]) || !std::isfinite(v0.pos[1]) ||
        !std::isfinite(v1.pos[0]) || !std::isfinite(v1.pos[1]) ||
        !std::isfinite(v2.pos[0]) || !std::isfinite(v2.pos[1])) {
        return;
    }
    if (v0.pos[0] < -2048.0f || v0.pos[0] > 2048.0f || v0.pos[1] < -2048.0f || v0.pos[1] > 2048.0f ||
        v1.pos[0] < -2048.0f || v1.pos[0] > 2048.0f || v1.pos[1] < -2048.0f || v1.pos[1] > 2048.0f ||
        v2.pos[0] < -2048.0f || v2.pos[0] > 2048.0f || v2.pos[1] < -2048.0f || v2.pos[1] > 2048.0f) {
        return;
    }

    if (m_vtxAllocated + 3 > MaxVerticesPerFrame) {
        FlushBatch();
        m_queue.submitCommands(m_cmdBuf.finishList());
        WaitPendingFences();
        m_queue.waitIdle();
        m_cmdBuf.clear();
        m_cmdPoolOffset[m_frameResIdx] = m_cmdPoolBase[m_frameResIdx] + CmdBufferSize;
        m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[m_frameResIdx], CmdBufferSize);
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
    if (m_vramDirty && m_vram) {
        // PS1 VRAM es exactamente 1024x512 palabras de 16 bits = 1,048,576 bytes (1 MB)
        const size_t rawVramSize = VramWidth * VramHeight * sizeof(uint16_t);
        const uint32_t alignRaw = (uint32_t)((rawVramSize + 511u) & ~511u);

        // Asignar dinámicamente desde el ring buffer de staging para que cada sincronización
        // tenga su propio buffer inmutable en memoria GPU y no colisione con comandos en vuelo
        if (m_stagingUploadOffset + alignRaw > StagingRingEnd) {
            m_queue.submitCommands(m_cmdBuf.finishList());
            WaitPendingFences();
            m_queue.waitIdle();
            m_cmdBuf.clear();
            m_cmdPoolOffset[m_frameResIdx] = m_cmdPoolBase[m_frameResIdx] + CmdBufferSize;
            m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[m_frameResIdx], CmdBufferSize);
            m_stagingUploadOffset = StagingRingStart;
            m_vtxAllocated = 0;
            m_vtxBatchStart = 0;
        }

        uint32_t rawOffset = m_stagingUploadOffset;
        m_stagingUploadOffset += alignRaw;

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
            printf("[Deko3D] SyncRawVram #%llu: non_zero_texels=%u / 524288 (stagingOffset=0x%x)\n",
                   (unsigned long long)s_sync_count, nz, rawOffset);
            fflush(stdout);
        }

        m_vramDirty = false;
    }

    const uint16_t* highPtr = m_highVram ? m_highVram : gpu_high_vram_pixels();
    if (m_highVramDirty && highPtr) {
        const size_t rawVramSize = VramWidth * VramHeight * sizeof(uint16_t);
        const uint32_t alignRaw = (uint32_t)((rawVramSize + 511u) & ~511u);

        if (m_stagingUploadOffset + alignRaw > StagingRingEnd) {
            m_queue.submitCommands(m_cmdBuf.finishList());
            WaitPendingFences();
            m_queue.waitIdle();
            m_cmdBuf.clear();
            m_cmdPoolOffset[m_frameResIdx] = m_cmdPoolBase[m_frameResIdx] + CmdBufferSize;
            m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[m_frameResIdx], CmdBufferSize);
            m_stagingUploadOffset = StagingRingStart;
            m_vtxAllocated = 0;
            m_vtxBatchStart = 0;
        }

        uint32_t rawOffset = m_stagingUploadOffset;
        m_stagingUploadOffset += alignRaw;

        uint8_t* dst = (uint8_t*)m_memStaging.getCpuAddr() + rawOffset;
        memcpy(dst, highPtr, rawVramSize);
        m_memStaging.flushCpuCache(rawOffset, rawVramSize);

        dk::ImageView rawView{ m_rawVramImage };
        m_cmdBuf.copyBufferToImage(
            { m_memStaging.getGpuAddr() + rawOffset, 0, 0 },
            rawView,
            { 0, VramHeight, 0, VramWidth, VramHeight, 1 }
        );
        m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
        m_highVramDirty = false;
        static uint32_t s_hivram_sync_cnt = 0;
        if (++s_hivram_sync_cnt <= 10 || (s_hivram_sync_cnt % 60) == 0) {
            printf("[HighVram_Sync] #%u: Clean 1024x512 High VRAM upload to GPU (offset=0x%x)\n",
                   s_hivram_sync_cnt, rawOffset);
            fflush(stdout);
        }
    }
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
    m_viewportUboCpu->extra[0] = 0.5f / (float)m_internalScale;
    m_viewportUboCpu->extra[1] = (float)m_internalScale;
    m_viewportUboCpu->extra[2] = 0.0f;
    m_viewportUboCpu->extra[3] = 0.0f;

    m_cmdBuf.pushConstants(m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT, 0, sizeof(ViewportUniforms), m_viewportUboCpu);

    // 2. Vincular Render Target (m_vramTexture RGBA8 m_hrVramWidth x m_hrVramHeight)
    dk::ImageView vramTarget{ m_vramTexture };
    m_cmdBuf.bindRenderTargets(&vramTarget);

    DkViewport viewport{ 0.0f, 0.0f, (float)m_hrVramWidth, (float)m_hrVramHeight, 0.0f, 1.0f };
    m_cmdBuf.setViewports(0, { viewport });

    // 3. Configurar Scissor (primitivos absolutos como FillRect ignoran el clip de dibujo en PS1)
    bool isAbsolute = ((m_vtxRingCpu[m_vtxBatchStart].flags & 0x80000000u) != 0);
    if (isAbsolute) {
        DkScissor scissor{ 0, 0, m_hrVramWidth, m_hrVramHeight };
        m_cmdBuf.setScissors(0, { scissor });
    } else {
        int scX = m_clipX1 * (int)m_internalScale;
        int scY = m_clipY1 * (int)m_internalScale;
        int scX2 = (m_clipX2 + 1) * (int)m_internalScale - 1;
        int scY2 = (m_clipY2 + 1) * (int)m_internalScale - 1;

        if (scX < 0) scX = 0;
        if (scY < 0) scY = 0;
        if (scX2 >= (int)m_hrVramWidth) scX2 = (int)m_hrVramWidth - 1;
        if (scY2 >= (int)m_hrVramHeight) scY2 = (int)m_hrVramHeight - 1;

        int scW = scX2 - scX + 1;
        int scH = scY2 - scY + 1;

        if (scW <= 0 || scH <= 0 || scX >= (int)m_hrVramWidth || scY >= (int)m_hrVramHeight) {
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
            bState.setFactors(DkBlendFactor_SrcAlpha, DkBlendFactor_InvSrcAlpha, DkBlendFactor_Zero, DkBlendFactor_One);
        } else if (m_semiMode == 1) {
            bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
            bState.setFactors(DkBlendFactor_One, DkBlendFactor_SrcAlpha, DkBlendFactor_Zero, DkBlendFactor_One);
        } else if (m_semiMode == 2) {
            bState.setOps(DkBlendOp_RevSub, DkBlendOp_Add);
            bState.setFactors(DkBlendFactor_One, DkBlendFactor_One, DkBlendFactor_Zero, DkBlendFactor_One);
        } else if (m_semiMode == 3) {
            bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
            bState.setFactors(DkBlendFactor_One, DkBlendFactor_SrcAlpha, DkBlendFactor_Zero, DkBlendFactor_One);
        }
    } else {
        bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
        bState.setFactors(DkBlendFactor_One, DkBlendFactor_Zero, DkBlendFactor_Zero, DkBlendFactor_One);
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
    cwState.setMask(0, DkColorMask_RGB);
    m_cmdBuf.bindColorWriteState(cwState);

    // 6. Vincular Shaders, Descriptores y Uniform Buffers
    m_cmdBuf.bindShaders(DkStageFlag_GraphicsMask, { &m_vsh, &m_fsh });

    m_cmdBuf.bindUniformBuffer(DkStage_Vertex, 0, m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT);
    m_cmdBuf.bindUniformBuffer(DkStage_Fragment, 0, m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT);

    m_cmdBuf.bindImageDescriptorSet(m_imgDescGpu, 2);
    m_cmdBuf.bindSamplerDescriptorSet(m_smpDescGpu, 1);
    m_cmdBuf.bindTextures(DkStage_Fragment, 0, { dkMakeTextureHandle(0, 0), dkMakeTextureHandle(1, 0) });

    // 7. Configurar Formato de Vértices y Buffer
    static const std::array<DkVtxAttribState, 6> vtxAttribState = {{
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, pos),        DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, uv),         DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, color),      DkVtxAttribSize_4x8,  DkVtxAttribType_Unorm, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, tpage_clut), DkVtxAttribSize_2x16, DkVtxAttribType_Uint,  0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, flags),      DkVtxAttribSize_1x32, DkVtxAttribType_Uint,  0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, q),          DkVtxAttribSize_1x32, DkVtxAttribType_Float, 0 },
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
        static bool s_first_batch_37f4_logged = false;
        if (c0 == 0x37F4 && !s_first_batch_37f4_logged) {
            s_first_batch_37f4_logged = true;
            printf("[Deko3D_ALERT] FlushBatch #%llu: FIRST BATCH WITH c0=0x37F4! cnt=%u tp=0x%x clut=0x%x(c0=0x%04x,c1=0x%04x)\n",
                   (unsigned long long)s_flush_count, count, tp, clut, c0, c1);
            fflush(stdout);
        }
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
    uint32_t flags = (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u);

    PS1Vertex v0 = { {(float)x0, (float)y0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v1 = { {(float)x1, (float)y1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v2 = { {(float)x2, (float)y2}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };

    PushTriangle(v0, v1, v2);
}

void Deko3DRenderer::DrawGouraudTriangle(int x0, int y0, uint16_t c0,
                                        int x1, int y1, uint16_t c1,
                                        int x2, int y2, uint16_t c2) {
    uint8_t rgba0[4], rgba1[4], rgba2[4];
    PS1ColorToRGBA8(c0, rgba0);
    PS1ColorToRGBA8(c1, rgba1);
    PS1ColorToRGBA8(c2, rgba2);
    uint32_t flags = (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u);

    PS1Vertex v0 = { {(float)x0, (float)y0}, {0.0f, 0.0f}, {rgba0[0], rgba0[1], rgba0[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v1 = { {(float)x1, (float)y1}, {0.0f, 0.0f}, {rgba1[0], rgba1[1], rgba1[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v2 = { {(float)x2, (float)y2}, {0.0f, 0.0f}, {rgba2[0], rgba2[1], rgba2[2], 255}, {0, 0}, flags, 0 };

    PushTriangle(v0, v1, v2);
}

void Deko3DRenderer::DrawTexturedTriangle(int x0, int y0, int u0, int v0,
                                         int x1, int y1, int u1, int v1,
                                         int x2, int y2, int u2, int v2,
                                         uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    uint16_t clut = (uint16_t)((clut_x / 16) | (clut_y << 6));
    int depth = (texpage >> 7) & 3;
    uint32_t flags = (uint32_t)depth | (m_rawTex ? 4u : 0u) | (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u) | 0x40u
                   | (m_texHigh ? 0x80u : 0u) | (m_clutHigh ? 0x100u : 0u);
    uint8_t rgba[4] = { (uint8_t)m_modR, (uint8_t)m_modG, (uint8_t)m_modB, 255 };

    float q0 = m_pqValid ? m_pq[0] : 0.0f;
    float q1 = m_pqValid ? m_pq[1] : 0.0f;
    float q2 = m_pqValid ? m_pq[2] : 0.0f;
    m_pqValid = false;

    PS1Vertex vert0 = { {(float)x0, (float)y0}, {(float)u0, (float)v0}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, q0 };
    PS1Vertex vert1 = { {(float)x1, (float)y1}, {(float)u1, (float)v1}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, q1 };
    PS1Vertex vert2 = { {(float)x2, (float)y2}, {(float)u2, (float)v2}, {rgba[0], rgba[1], rgba[2], 255}, {texpage, clut}, flags, q2 };

    PushTriangle(vert0, vert1, vert2);
}

void Deko3DRenderer::DrawShadedTexturedTriangle(int x0, int y0, int u0, int v0, uint32_t col0,
                                               int x1, int y1, int u1, int v1, uint32_t col1,
                                               int x2, int y2, int u2, int v2, uint32_t col2,
                                               uint16_t clut_x, uint16_t clut_y, uint16_t texpage, int raw) {
    uint16_t clut = (uint16_t)((clut_x / 16) | (clut_y << 6));
    int depth = (texpage >> 7) & 3;
    uint32_t flags = (uint32_t)depth | (raw ? 4u : 0u) | (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u) | 0x40u
                   | (m_texHigh ? 0x80u : 0u) | (m_clutHigh ? 0x100u : 0u);

    uint8_t rgba0[4], rgba1[4], rgba2[4];
    PS1Color24ToRGBA8(col0, rgba0);
    PS1Color24ToRGBA8(col1, rgba1);
    PS1Color24ToRGBA8(col2, rgba2);

    float q0 = m_pqValid ? m_pq[0] : 0.0f;
    float q1 = m_pqValid ? m_pq[1] : 0.0f;
    float q2 = m_pqValid ? m_pq[2] : 0.0f;
    m_pqValid = false;

    PS1Vertex vert0 = { {(float)x0, (float)y0}, {(float)u0, (float)v0}, {rgba0[0], rgba0[1], rgba0[2], 255}, {texpage, clut}, flags, q0 };
    PS1Vertex vert1 = { {(float)x1, (float)y1}, {(float)u1, (float)v1}, {rgba1[0], rgba1[1], rgba1[2], 255}, {texpage, clut}, flags, q1 };
    PS1Vertex vert2 = { {(float)x2, (float)y2}, {(float)u2, (float)v2}, {rgba2[0], rgba2[1], rgba2[2], 255}, {texpage, clut}, flags, q2 };

    PushTriangle(vert0, vert1, vert2);
}

void Deko3DRenderer::DrawRect(int x, int y, int w, int h, uint16_t color) {
    if (h >= 400) {
        static uint32_t s_dk_rect_cnt = 0;
        if (++s_dk_rect_cnt <= 10 || (s_dk_rect_cnt % 60) == 0) {
            printf("[Deko3D_DrawRect_Diag] #%u: x=%d y=%d w=%d h=%d color=0x%04x clip=(%d,%d..%d,%d)\n",
                   s_dk_rect_cnt, x, y, w, h, (unsigned)color,
                   m_clipX1, m_clipY1, m_clipX2, m_clipY2);
            fflush(stdout);
        }
    }
    uint8_t rgba[4];
    PS1ColorToRGBA8(color, rgba);
    uint32_t flags = (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u);

    float fx0 = (float)x, fy0 = (float)y;
    float fx1 = (float)(x + w), fy1 = (float)(y + h);

    PS1Vertex v0 = { {fx0, fy0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v1 = { {fx1, fy0}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v2 = { {fx0, fy1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };
    PS1Vertex v3 = { {fx1, fy1}, {0.0f, 0.0f}, {rgba[0], rgba[1], rgba[2], 255}, {0, 0}, flags, 0 };

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
    uint32_t flags = (uint32_t)depth | (m_rawTex ? 4u : 0u) | (m_semiEnabled ? 8u : 0u) | ((uint32_t)m_semiMode << 4u) | 0x40u
                   | (m_texHigh ? 0x80u : 0u) | (m_clutHigh ? 0x100u : 0u);
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
    if (x >= 384 || y >= 480) return;
    if (x + w > 384) w = 384 - x;
    if (y + h > 480) h = 480 - y;
    x &= (VramWidth - 1);
    if (x + w > (int)VramWidth) w = (int)VramWidth - x;
    if (w <= 0 || h <= 0) return;

    FlushBatch();
    sw_fill_rect(x, y, w, h, color);
    // FillRect draws only to the framebuffer (m_vramTexture), never texture memory.
    // Do NOT set m_vramDirty to avoid overwriting texture sampling memory m_rawVramImage.

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
    if (w <= 0 || h <= 0) return;
    if (sx == dx && sy == dy) {
        // Copia de un rectángulo sobre sí mismo: NO-OP absoluto. No ensuciar VRAM ni hacer flush.
        return;
    }

    FlushBatch();

    extern int g_post_fmv_sync_active;
    if (g_post_fmv_sync_active) {
        printf("[PostFMV_VRAM] CopyRect sx=%d sy=%d dx=%d dy=%d w=%d h=%d\n", sx, sy, dx, dy, w, h);
        fflush(stdout);
    }

    sw_copy_rect(sx, sy, dx, dy, w, h);
    if (!m_initialized) return;

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
    m_cmdBuf.copyImage(vramTexView, { (uint32_t)(sx * m_internalScale), (uint32_t)(sy * m_internalScale), 0, (uint32_t)(w * m_internalScale), (uint32_t)(h * m_internalScale), 1 },
                       vramTexView, { (uint32_t)(dx * m_internalScale), (uint32_t)(dy * m_internalScale), 0, (uint32_t)(w * m_internalScale), (uint32_t)(h * m_internalScale), 1 });

    dk::ImageView rawView{ m_rawVramImage };
    m_cmdBuf.copyImage(rawView, { (uint32_t)sx, (uint32_t)sy, 0, (uint32_t)w, (uint32_t)h, 1 },
                       rawView, { (uint32_t)dx, (uint32_t)dy, 0, (uint32_t)w, (uint32_t)h, 1 });

    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
}

void Deko3DRenderer::VramTransferIn(int x, int y, int w, int h, const uint16_t* data) {
    if (!m_initialized || !data || w <= 0 || h <= 0) return;

    extern int g_post_fmv_sync_active;
    if (g_post_fmv_sync_active) {
        printf("[PostFMV_VRAM] VramTransferIn x=%d y=%d w=%d h=%d\n", x, y, w, h);
        fflush(stdout);
    }

    u64 t_up_start = armGetSystemTick();

    FlushBatch();

    sw_vram_transfer_in(x, y, w, h, data);

    int orig_w = w;
    int orig_h = h;
    int base_x = x & (VramWidth - 1);
    int base_y = y & (VramHeight - 1);

    if (orig_w > (int)VramWidth) orig_w = (int)VramWidth;
    if (orig_h > (int)VramHeight) orig_h = (int)VramHeight;

    // Desglosar la transferencia en sub-rectángulos no envolventes (máx 4 rebanadas si cruza bordes de VRAM)
    int w_parts[2] = { orig_w, 0 };
    int src_x[2]   = { 0, 0 };
    int dst_x[2]   = { base_x, 0 };
    int num_x = 1;
    if (base_x + orig_w > (int)VramWidth) {
        w_parts[0] = (int)VramWidth - base_x;
        w_parts[1] = orig_w - w_parts[0];
        if (w_parts[1] > (int)VramWidth) w_parts[1] = (int)VramWidth;
        src_x[1]   = w_parts[0];
        dst_x[1]   = 0;
        num_x = 2;
    }

    int h_parts[2] = { orig_h, 0 };
    int src_y[2]   = { 0, 0 };
    int dst_y[2]   = { base_y, 0 };
    int num_y = 1;
    if (base_y + orig_h > (int)VramHeight) {
        h_parts[0] = (int)VramHeight - base_y;
        h_parts[1] = orig_h - h_parts[0];
        if (h_parts[1] > (int)VramHeight) h_parts[1] = (int)VramHeight;
        src_y[1]   = h_parts[0];
        dst_y[1]   = 0;
        num_y = 2;
    }

    dk::ImageView vramTexView{ m_vramTexture };
    dk::ImageView rawView{ m_rawVramImage };

    for (int vy = 0; vy < num_y; vy++) {
        int sub_h = h_parts[vy];
        int sy = src_y[vy];
        int dy = dst_y[vy];
        if (dy + sub_h > (int)VramHeight) sub_h = (int)VramHeight - dy;
        if (sub_h <= 0) continue;

        for (int vx = 0; vx < num_x; vx++) {
            int sub_w = w_parts[vx];
            int sx = src_x[vx];
            int dx = dst_x[vx];
            if (dx + sub_w > (int)VramWidth) sub_w = (int)VramWidth - dx;
            if (sub_w <= 0) continue;

            // Pitch alineado estrictamente a 32 bytes para el Maxwell 2D Copy Engine de Tegra X1
            uint32_t sub_w_hr = (uint32_t)sub_w * m_internalScale;
            uint32_t sub_h_hr = (uint32_t)sub_h * m_internalScale;
            uint32_t pitch32 = (sub_w_hr * sizeof(uint32_t) + 31u) & ~31u;
            size_t uploadBytes32 = (size_t)pitch32 * sub_h_hr;
            uint32_t align32 = (uint32_t)((uploadBytes32 + 511u) & ~511u);

            int raw_w = sub_w;
            uint32_t pitch16 = (sub_w * sizeof(uint16_t) + 31u) & ~31u;
            if (raw_w < 32 && (dx & 31) == 0 && dx + 32 <= (int)VramWidth && m_vram) {
                raw_w = 32;
                pitch16 = 32 * sizeof(uint16_t); // 64 bytes (1 GOB exacto)
            }
            size_t uploadBytes16 = (size_t)pitch16 * sub_h;
            uint32_t align16 = (uint32_t)((uploadBytes16 + 511u) & ~511u);

            uint32_t totalAlign = align32 + align16;

            // Buffer circular de staging dinámico (58 MB ring)
            if (m_stagingUploadOffset + totalAlign > StagingRingEnd) {
                FlushBatch();
                m_queue.submitCommands(m_cmdBuf.finishList());
                WaitPendingFences();
                m_queue.waitIdle();
                m_cmdBuf.clear();
                m_cmdPoolOffset[m_frameResIdx] = m_cmdPoolBase[m_frameResIdx] + CmdBufferSize;
                m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[m_frameResIdx], CmdBufferSize);
                m_stagingUploadOffset = StagingRingStart;
                m_vtxAllocated = 0;
                m_vtxBatchStart = 0;
            }

            uint32_t uploadOffset32 = m_stagingUploadOffset;
            m_stagingUploadOffset += align32;

            uint32_t uploadOffset16 = m_stagingUploadOffset;
            m_stagingUploadOffset += align16;

            // 1. Convertir a RGBA8 para m_vramTexture (duplicación NxN para coincidir con la resolución HR)
            uint8_t* dst32Base = (uint8_t*)m_memStaging.getCpuAddr() + uploadOffset32;
            for (int row = 0; row < sub_h; row++) {
                const uint16_t* srcRow = data + (size_t)(sy + row) * orig_w + sx;
                uint32_t* dRows[4];
                for (uint32_t s = 0; s < m_internalScale; s++) {
                    dRows[s] = (uint32_t*)(dst32Base + (size_t)(row * m_internalScale + s) * pitch32);
                }
                for (int col = 0; col < sub_w; col++) {
                    uint16_t c = srcRow[col];
                    uint32_t r5 = c & 0x1Fu;
                    uint32_t g5 = (c >> 5) & 0x1Fu;
                    uint32_t b5 = (c >> 10) & 0x1Fu;
                    uint32_t r8 = (r5 << 3) | (r5 >> 2);
                    uint32_t g8 = (g5 << 3) | (g5 >> 2);
                    uint32_t b8 = (b5 << 3) | (b5 >> 2);
                    uint32_t px = r8 | (g8 << 8) | (b8 << 16) | 0xFF000000u;
                    for (uint32_t s = 0; s < m_internalScale; s++) {
                        for (uint32_t sc = 0; sc < m_internalScale; sc++) {
                            dRows[s][col * m_internalScale + sc] = px;
                        }
                    }
                }
            }
            m_memStaging.flushCpuCache(uploadOffset32, uploadBytes32);

            // 2. Copiar 16-bit directos para m_rawVramImage (texturas y CLUTs de psx_fsh en resolución nativa 1x)
            uint8_t* dst16Base = (uint8_t*)m_memStaging.getCpuAddr() + uploadOffset16;
            for (int row = 0; row < sub_h; row++) {
                uint16_t* dRow16 = (uint16_t*)(dst16Base + (size_t)row * pitch16);
                if (raw_w > sub_w) {
                    const uint16_t* srcRow = m_vram + (size_t)(dy + row) * VramWidth + dx;
                    memcpy(dRow16, srcRow, (size_t)raw_w * sizeof(uint16_t));
                } else {
                    const uint16_t* srcRow = data + (size_t)(sy + row) * orig_w + sx;
                    memcpy(dRow16, srcRow, (size_t)sub_w * sizeof(uint16_t));
                }
            }
            m_memStaging.flushCpuCache(uploadOffset16, uploadBytes16);

            // Copias directas sincrónicas en el command buffer
            m_cmdBuf.copyBufferToImage({ m_memStaging.getGpuAddr() + uploadOffset32, pitch32, 0 },
                                       vramTexView,
                                       { (uint32_t)(dx * m_internalScale), (uint32_t)(dy * m_internalScale), 0, sub_w_hr, sub_h_hr, 1 });

            m_cmdBuf.copyBufferToImage({ m_memStaging.getGpuAddr() + uploadOffset16, pitch16, 0 },
                                       rawView,
                                       { (uint32_t)dx, (uint32_t)dy, 0, (uint32_t)raw_w, (uint32_t)sub_h, 1 });
        }
    }

    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
    u64 t_up_end = armGetSystemTick();
    m_timings.vram_upload_ns += armTicksToNs(t_up_end - t_up_start);
    m_timings.vram_upload_bytes += (size_t)orig_w * orig_h * sizeof(uint16_t);
}

void Deko3DRenderer::DrawOsdImage(const uint32_t* argbPixels, int ow, int oh,
                                  int vx, int vy, int dw, int dh,
                                  uint32_t outW, uint32_t outH,
                                  dk::ImageView& colorTarget) {
    if (!argbPixels || ow <= 0 || oh <= 0 || dw <= 0 || dh <= 0) return;
    if (ow > (int)OsdMaxWidth) ow = (int)OsdMaxWidth;
    if (oh > (int)OsdMaxHeight) oh = (int)OsdMaxHeight;
    if (vx < 0) { dw += vx; vx = 0; }
    if (vy < 0) { dh += vy; vy = 0; }
    if (vx >= (int)outW || vy >= (int)outH) return;
    if (vx + dw > (int)outW) dw = (int)outW - vx;
    if (vy + dh > (int)outH) dh = (int)outH - vy;
    if (dw <= 0 || dh <= 0) return;

    uint32_t pitch32 = ((uint32_t)ow * sizeof(uint32_t) + 31u) & ~31u;
    size_t uploadBytes32 = (size_t)pitch32 * (size_t)oh;
    uint32_t align32 = (uint32_t)((uploadBytes32 + 511u) & ~511u);

    if (m_stagingUploadOffset + align32 > StagingRingEnd || m_vtxAllocated + 6 > MaxVerticesPerFrame) {
        m_queue.submitCommands(m_cmdBuf.finishList());
        WaitPendingFences();
        m_queue.waitIdle();
        m_cmdBuf.clear();
        m_cmdPoolOffset[m_frameResIdx] = m_cmdPoolBase[m_frameResIdx] + CmdBufferSize;
        m_cmdBuf.addMemory(m_memData, m_cmdPoolBase[m_frameResIdx], CmdBufferSize);
        m_stagingUploadOffset = StagingRingStart;
        m_vtxAllocated = 0;
        m_vtxBatchStart = 0;
    }

    uint32_t uploadOffset32 = m_stagingUploadOffset;
    m_stagingUploadOffset += align32;

    uint8_t* dst32Base = (uint8_t*)m_memStaging.getCpuAddr() + uploadOffset32;
    for (int row = 0; row < oh; row++) {
        const uint32_t* srcRow = argbPixels + (size_t)row * (size_t)ow;
        uint32_t* dRow = (uint32_t*)(dst32Base + (size_t)row * pitch32);
        for (int col = 0; col < ow; col++) {
            uint32_t argb = srcRow[col];
            uint32_t a = (argb >> 24) & 0xFFu;
            uint32_t r = (argb >> 16) & 0xFFu;
            uint32_t g = (argb >> 8) & 0xFFu;
            uint32_t b = argb & 0xFFu;
            dRow[col] = r | (g << 8) | (b << 16) | (a << 24);
        }
    }
    m_memStaging.flushCpuCache(uploadOffset32, uploadBytes32);

    dk::ImageView osdView{ m_osdImage };
    m_cmdBuf.copyBufferToImage(
        { m_memStaging.getGpuAddr() + uploadOffset32, pitch32, 0 },
        osdView,
        { 0, 0, 0, (uint32_t)ow, (uint32_t)oh, 1 }
    );
    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);

    m_viewportUboCpu->screenSize[0] = (float)VramWidth;
    m_viewportUboCpu->screenSize[1] = (float)VramHeight;
    m_viewportUboCpu->offset[0] = 0.0f;
    m_viewportUboCpu->offset[1] = 0.0f;
    m_viewportUboCpu->texWindow[0] = 0;
    m_viewportUboCpu->texWindow[1] = 0;
    m_viewportUboCpu->texWindow[2] = 0;
    m_viewportUboCpu->texWindow[3] = 0;
    m_viewportUboCpu->extra[0] = 0.0f;
    m_viewportUboCpu->extra[1] = 1.0f;
    m_viewportUboCpu->extra[2] = 0.0f;
    m_viewportUboCpu->extra[3] = 0.0f;
    m_cmdBuf.pushConstants(m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT, 0, sizeof(ViewportUniforms), m_viewportUboCpu);

    m_cmdBuf.bindRenderTargets(&colorTarget);
    DkViewport viewport{ (float)vx, (float)vy, (float)dw, (float)dh, 0.0f, 1.0f };
    m_cmdBuf.setViewports(0, { viewport });
    DkScissor scissor{ (uint32_t)vx, (uint32_t)vy, (uint32_t)dw, (uint32_t)dh };
    m_cmdBuf.setScissors(0, { scissor });

    dk::BlendState bState;
    bState.setOps(DkBlendOp_Add, DkBlendOp_Add);
    bState.setFactors(DkBlendFactor_SrcAlpha, DkBlendFactor_InvSrcAlpha, DkBlendFactor_Zero, DkBlendFactor_One);
    m_cmdBuf.bindBlendStates(0, { bState });

    dk::RasterizerState rState;
    rState.setCullMode(DkFace_None);
    m_cmdBuf.bindRasterizerState(rState);

    dk::DepthStencilState dsState;
    dsState.setDepthTestEnable(false);
    dsState.setDepthWriteEnable(false);
    dsState.setStencilTestEnable(false);
    m_cmdBuf.bindDepthStencilState(dsState);

    dk::ColorState cState;
    cState.setBlendEnable(0, true);
    m_cmdBuf.bindColorState(cState);

    dk::ColorWriteState cwState;
    cwState.setMask(0, DkColorMask_RGBA);
    m_cmdBuf.bindColorWriteState(cwState);

    m_cmdBuf.bindShaders(DkStageFlag_GraphicsMask, { &m_vsh, &m_fsh });
    m_cmdBuf.bindUniformBuffer(DkStage_Vertex, 0, m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT);
    m_cmdBuf.bindUniformBuffer(DkStage_Fragment, 0, m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT);
    m_cmdBuf.bindImageDescriptorSet(m_imgDescGpu, 2);
    m_cmdBuf.bindSamplerDescriptorSet(m_smpDescGpu, 1);
    m_cmdBuf.bindTextures(DkStage_Fragment, 0, { dkMakeTextureHandle(0, 0), dkMakeTextureHandle(1, 0) });

    static const std::array<DkVtxAttribState, 6> vtxAttribState = {{
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, pos),        DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, uv),         DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, color),      DkVtxAttribSize_4x8,  DkVtxAttribType_Unorm, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, tpage_clut), DkVtxAttribSize_2x16, DkVtxAttribType_Uint,  0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, flags),      DkVtxAttribSize_1x32, DkVtxAttribType_Uint,  0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, q),          DkVtxAttribSize_1x32, DkVtxAttribType_Float, 0 },
    }};
    DkVtxBufferState vtxBufferState{ sizeof(PS1Vertex), 0 };
    m_cmdBuf.bindVtxAttribState(vtxAttribState);
    m_cmdBuf.bindVtxBufferState({ vtxBufferState });
    m_cmdBuf.bindVtxBuffer(0, m_vtxRingGpu, VertexRingSliceSize);

    uint32_t startVtx = m_vtxAllocated;
    uint32_t flags = 0x200u; // Modo OSD RGBA8 en psx_fsh.glsl
    PS1Vertex v0 = { {0.0f, 0.0f},                   {0.0f, 0.0f},           {255, 255, 255, 255}, {0, 0}, flags, 0.0f };
    PS1Vertex v1 = { {(float)VramWidth, 0.0f},       {(float)ow, 0.0f},      {255, 255, 255, 255}, {0, 0}, flags, 0.0f };
    PS1Vertex v2 = { {0.0f, (float)VramHeight},      {0.0f, (float)oh},      {255, 255, 255, 255}, {0, 0}, flags, 0.0f };
    PS1Vertex v3 = { {(float)VramWidth, (float)VramHeight}, {(float)ow, (float)oh}, {255, 255, 255, 255}, {0, 0}, flags, 0.0f };

    m_vtxRingCpu[m_vtxAllocated + 0] = v0;
    m_vtxRingCpu[m_vtxAllocated + 1] = v1;
    m_vtxRingCpu[m_vtxAllocated + 2] = v2;
    m_vtxRingCpu[m_vtxAllocated + 3] = v1;
    m_vtxRingCpu[m_vtxAllocated + 4] = v3;
    m_vtxRingCpu[m_vtxAllocated + 5] = v2;
    m_vtxAllocated += 6;
    m_vtxBatchStart = m_vtxAllocated;
    __asm__ __volatile__("dsb sy" ::: "memory");

    m_cmdBuf.draw(DkPrimitive_Triangles, 6, 1, startVtx, 0);
    m_cmdBuf.bindRenderTargets({});
    m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image);
}

int Deko3DRenderer::RenderDisplay(uint32_t* out, int pitch, int dx, int dy, int dw, int dh) {
    if (out != nullptr) {
        return sw_render_display(out, pitch, dx, dy, dw, dh);
    }

    if (!m_initialized) {
        return 0;
    }

    u64 t_prep_start = armGetSystemTick();

    if (m_vtxAllocated > m_vtxBatchStart) {
        FlushBatch();
    }

    static uint64_t s_present_count = 0;
    s_present_count++;

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

    static int s_prev_depth24 = -1;
    static int s_prev_disabled = -1;
    if (di.depth24 != s_prev_depth24 || di.disabled != s_prev_disabled) {
        printf("[DisplayTransition] Present #%llu: depth24=%d->%d disabled=%d->%d disp=(%u,%u %ux%u)\n",
               (unsigned long long)s_present_count, s_prev_depth24, di.depth24, s_prev_disabled, di.disabled,
               dispX, dispY, w, h);
        s_prev_depth24 = di.depth24;
        s_prev_disabled = di.disabled;
        fflush(stdout);
    }

    dk::ImageView colorTarget{ m_framebuffers[m_curSlot] };

    // Resolución de salida dinámica automática: 1920x1080 nativo en Dock (TV) / 1280x720 nativo en Portátil (Handheld)
    const bool isDocked = (appletGetOperationMode() == AppletOperationMode_Console);
    const uint32_t curOutW = isDocked ? TargetWidth : HandheldWidth;
    const uint32_t curOutH = isDocked ? TargetHeight : HandheldHeight;

    // Aspecto de presentación configurable: 4:3 con Pillarbox o 16:9 pantalla completa
    uint32_t dstW, dstX, dstY, dstH;
    if (m_widescreen) {
        dstW = curOutW;                  // 1920 (Docked) / 1280 (Handheld)
        dstX = 0;
        dstY = 0;
        dstH = curOutH;                  // 1080 (Docked) / 720 (Handheld)
    } else {
        dstW = curOutH * 4 / 3;          // 1440 (Docked) / 960 (Handheld)
        dstX = (curOutW - dstW) / 2;     // 240  (Docked) / 160 (Handheld)
        dstY = 0;
        dstH = curOutH;                  // 1080 (Docked) / 720 (Handheld)
    }

    static int s_last_docked_mode = -1;
    if ((int)isDocked != s_last_docked_mode) {
        s_last_docked_mode = (int)isDocked;
        nwindowSetCrop(nwindowGetDefault(), 0, 0, curOutW, curOutH);
        printf("[Deko3D] %s Present activo (%s): src=(%u,%u %ux%u) hr_src=(%u,%u %ux%u) -> dst=(%u,%u %ux%u sobre %ux%u) [SSAA=%ux]\n",
               m_widescreen ? "16:9 Widescreen" : "4:3 HD",
               isDocked ? "DOCKED 1080p" : "HANDHELD 720p",
               dispX, dispY, w, h, dispX * m_internalScale, dispY * m_internalScale, w * m_internalScale, h * m_internalScale,
               dstX, dstY, dstW, dstH, curOutW, curOutH, m_internalScale);
        fflush(stdout);
    }

    // CRÍTICO: Restaurar el Scissor al tamaño completo del framebuffer de salida (1920x1080)
    // antes de invocar clearColor sobre colorTarget. En el hardware Maxwell / Deko3D, clearColor
    // respeta el DkScissor activo; si se deja el Scissor del último FlushBatch de PS1
    // (ej. clip=(0,98..367,166) -> 736x138), clearColor solo limpia ese pequeño recuadro,
    // dejando restos congelados de la imagen anterior en las franjas negras laterales al alternar
    // entre Docked (1920x1080) y Handheld (1280x720).
    DkScissor fullOutScissor{ 0, 0, TargetWidth, TargetHeight };
    m_cmdBuf.setScissors(0, { fullOutScissor });

    if (di.disabled) {
        m_cmdBuf.bindRenderTargets(&colorTarget);
        m_cmdBuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);
        m_cmdBuf.bindRenderTargets({});
    } else {
        m_cmdBuf.bindRenderTargets({});
        if (!m_widescreen) {
            // Limpiar el 100% del framebuffer de salida a negro puro (pillarboxing 4:3)
            m_cmdBuf.bindRenderTargets(&colorTarget);
            m_cmdBuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);
            m_cmdBuf.bindRenderTargets({});
        }

        if (di.depth24 && m_vram && m_vramStagingCpu) {
            // Ruta para FMVs de 24-bit
            static uint32_t s_fmv_log_cnt = 0;
            if (s_fmv_log_cnt++ < 10 || (s_fmv_log_cnt % 60) == 0) {
                printf("[FMV_Diag] 24-bit FMV Present #%llu: disp=(%u,%u %ux%u) dst=(%u,%u %ux%u)\n",
                       (unsigned long long)s_present_count, dispX, dispY, w, h, dstX, dstY, dstW, dstH);
                fflush(stdout);
            }

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
            uint32_t fmvSrcW = (w > 1) ? (w - 1) : 1;
            m_cmdBuf.blitImage(vramTexView, { 0, 0, 0, fmvSrcW, h, 1 },
                               colorTarget, { dstX, dstY, 0, dstW, dstH, 1 },
                               DkBlitFlag_FilterLinear);
        } else {
            // Presentación directa por hardware GPU Maxwell (0 ms de conversión CPU) con SSAA dinámico
            dk::ImageView vramTexView{ m_vramTexture };
            m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);

            bool inCombat = (w == 368 && h == 480 && m_clipY1 == 20 && m_clipY2 == 467);
            static bool s_wasInCombat = false;
            if (inCombat && !s_wasInCombat) {
                // Al entrar a combate desde la pantalla VS o menú de carga:
                // Limpiar toda la textura de renderizado m_vramTexture a negro puro
                // para garantizar que ningún remanente de retratos o texto estático de la pantalla VS quede en VRAM.
                dk::ImageView vramTarget{ m_vramTexture };
                m_cmdBuf.bindRenderTargets(&vramTarget);
                DkScissor fullScissor{ 0, 0, m_hrVramWidth, m_hrVramHeight };
                m_cmdBuf.setScissors(0, { fullScissor });
                m_cmdBuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);
                m_cmdBuf.bindRenderTargets({});
                m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
            }
            s_wasInCombat = inCombat;

            // Inset de borde derecho: Al reescalar horizontalmente (e.g. 736 -> 960 o 1280) con filtrado bilineal
            // (DkBlitFlag_FilterLinear), el último pixel de destino muestrea a x > (w*scale - 0.5), sangrando
            // con el texel adyacente fuera del framebuffer (memoria de texturas/UI en VRAM a x >= 368),
            // lo cual provocaba una línea semi-transparente clara en el borde derecho.
            // Restar 1 al ancho fuente mantiene el rango de muestreo estrictamente dentro del framebuffer renderizado.
            // En combate 480i, las 448 líneas activas renderizadas por la GPU son [20..467]; las líneas 0..19 y 468..479
            // corresponden al overscan de televisores CRT de la época y nunca se dibujan en combate. Muestrear [20..467]
            // evita mostrar líneas no refrescadas de la pantalla previa.
            uint32_t blitSrcX = dispX * m_internalScale;
            uint32_t blitSrcY = (inCombat ? 20u : (uint32_t)dispY) * m_internalScale;
            uint32_t blitSrcW = (w * m_internalScale > 1) ? (w * m_internalScale - 1) : 1;
            uint32_t blitSrcH = (inCombat ? 448u : (uint32_t)h) * m_internalScale;
            m_cmdBuf.blitImage(vramTexView, { blitSrcX, blitSrcY, 0, blitSrcW, blitSrcH, 1 },
                               colorTarget, { dstX, dstY, 0, dstW, dstH, 1 },
                               DkBlitFlag_FilterLinear);
            m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);
        }

        // Superposición OSD 2D (Carrusel de trajes personalizados con L1, selector y avisos)
        const uint32_t* osdPx = nullptr;
        int osdW = 0, osdH = 0;
        int uiScale = (int)curOutH / 480;
        if (uiScale < 1) uiScale = 1;
        if (uiScale > 8) uiScale = 8;
        int osdMargin = 8 * uiScale;

        for (int player = 0; player < 2; ++player) {
            if (host_osd_outfit_image(player, &osdPx, &osdW, &osdH) && osdPx) {
                int rx = 0, ry = 0, rdw = 0, rdh = 0;
                host_osd_outfit_rect(player, (int)dstW, (int)dstH, osdW, osdH, &rx, &ry, &rdw, &rdh);
                int hiresW = (rdw > (int)OsdMaxWidth) ? (int)OsdMaxWidth : rdw;
                int hiresH = (rdh > (int)OsdMaxHeight) ? (int)OsdMaxHeight : rdh;
                if (host_osd_outfit_image_hires(player, &osdPx, hiresW, hiresH) && osdPx) {
                    DrawOsdImage(osdPx, hiresW, hiresH, (int)dstX + rx, (int)dstY + ry, rdw, rdh, curOutW, curOutH, colorTarget);
                } else {
                    DrawOsdImage(osdPx, osdW, osdH, (int)dstX + rx, (int)dstY + ry, rdw, rdh, curOutW, curOutH, colorTarget);
                }
            }
        }
        if (host_osd_image(&osdPx, &osdW, &osdH) && osdPx) {
            DrawOsdImage(osdPx, osdW, osdH, (int)dstX + osdMargin, (int)dstY + osdMargin, osdW * uiScale, osdH * uiScale, curOutW, curOutH, colorTarget);
        }
        if (host_osd_volume_image(&osdPx, &osdW, &osdH) && osdPx) {
            int vdw = osdW * uiScale;
            int vdh = osdH * uiScale;
            int vx = ((int)curOutW > vdw + osdMargin) ? ((int)curOutW - vdw - osdMargin) : osdMargin;
            int vy = ((int)curOutH > vdh) ? (((int)curOutH - vdh) / 2) : osdMargin;
            DrawOsdImage(osdPx, osdW, osdH, vx, vy, vdw, vdh, curOutW, curOutH, colorTarget);
        }
        host_osd_present_done();
    }

    DkCmdList finalCmdList = m_cmdBuf.finishList();
    u64 t_submit_start = armGetSystemTick();
    m_queue.submitCommands(finalCmdList);
    m_queue.signalFence(m_frameFences[m_frameResIdx], true);
    m_fenceInFlight[m_frameResIdx] = true;

    u64 t_present_start = armGetSystemTick();
    m_queue.presentImage(m_swapchain, m_curSlot);
    u64 t_present_end = armGetSystemTick();

    if (s_present_count <= 10 || (s_present_count % 60) == 0) {
        printf("[Deko3D] Present #%llu disp=(%u,%u,%ux%u) clip=(%d,%d..%d,%d) off=(%d,%d) d24=%d dis=%d vtx=%u slot=%d\n",
               (unsigned long long)s_present_count,
               di.display_x, di.display_y, w, h,
               m_clipX1, m_clipY1, m_clipX2, m_clipY2,
               m_offsetX, m_offsetY,
               di.depth24, di.disabled, m_vtxAllocated, m_frameResIdx);
        fflush(stdout);
    }

    // Monitor CLUT area (y=480..511) for 0x37F4 corruption
    static uint32_t s_last_watchdog_37f4 = 0;
    if (m_vram && (s_present_count % 10 == 0)) {
        uint32_t count_37f4 = 0;
        uint32_t first_x = 0, first_y = 0;
        for (uint32_t cy = 480; cy < 512; cy++) {
            for (uint32_t cx = 0; cx < 1024; cx++) {
                if (m_vram[cy * VramWidth + cx] == 0x37F4) {
                    if (count_37f4 == 0) {
                        first_x = cx;
                        first_y = cy;
                    }
                    count_37f4++;
                }
            }
        }
        if (count_37f4 > 0 && s_last_watchdog_37f4 == 0) {
            printf("\n======================================================\n"
                   "[VRAM_CORRUPTION_DETECTED] Present #%llu: FIRST DETECTION of 0x37F4 in CLUT region (y=480..511)!\n"
                   "  Count of 0x37F4 texels: %u / 32768\n"
                   "  First found at VRAM coords: (%u, %u)\n"
                   "======================================================\n\n",
                   (unsigned long long)s_present_count, count_37f4, first_x, first_y);
            fflush(stdout);
        } else if (count_37f4 != s_last_watchdog_37f4 && (s_present_count % 60 == 0 || s_last_watchdog_37f4 == 0)) {
            printf("[VRAM_MONITOR] Present #%llu: CLUT region 0x37F4 texels: %u (first at %u,%u)\n",
               (unsigned long long)s_present_count, count_37f4, first_x, first_y);
            fflush(stdout);
        }
        s_last_watchdog_37f4 = count_37f4;
    }

    m_lastFrameVtx = m_vtxAllocated;
    u64 t_fence_start = armGetSystemTick();
    // Alternar al otro búfer de recursos para el siguiente frame (CPU/GPU en paralelo)
    m_frameResIdx = 1 - m_frameResIdx;
    BeginFrame();
    u64 t_fence_end = armGetSystemTick();

    m_timings.vram_prep_ns += armTicksToNs(t_submit_start - t_prep_start);
    m_timings.submit_ns += armTicksToNs(t_present_start - t_submit_start);
    m_timings.present_ns += armTicksToNs(t_present_end - t_present_start);
    m_timings.fence_wait_ns += armTicksToNs(t_fence_end - t_fence_start);
    m_timings.samples++;

    return (int)(w * h);
}

Deko3DRenderer::RenderTimings Deko3DRenderer::GetAndResetRenderTimings() {
    RenderTimings t = m_timings;
    m_timings = {};
    return t;
}

int Deko3DRenderer::SetHighVram(const uint16_t* high, int tex_high, int clut_high) {
    if (high && m_highVram != high) {
        m_highVram = high;
        m_highVramDirty = true;
    }
    m_texHigh = (tex_high != 0);
    m_clutHigh = (clut_high != 0);
    return 1;
}

void Deko3DRenderer::HighVramWritten(int x, int y, int w, int h) {
    (void)x; (void)y; (void)w; (void)h;
    m_highVramDirty = true;

    static uint32_t s_hivram_written_cnt = 0;
    if (++s_hivram_written_cnt <= 10 || (s_hivram_written_cnt % 60) == 0) {
        printf("[HighVramWritten_Diag] #%u: (%d,%d) %dx%d -> highVramDirty=1\n",
               s_hivram_written_cnt, x, y, w, h);
        fflush(stdout);
    }
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

static void dk_set_perspective_triangle(int enabled, float q0, float q1, float q2) {
    Deko3DRenderer::Instance().SetPerspectiveTriangle(enabled, q0, q1, q2);
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
    Deko3DRenderer::Instance().VramTransferIn(x, y, w, h, data);
}

static uint16_t dk_vram_read(int x, int y) {
    Deko3DRenderer::Instance().WaitPendingFences();
    return sw_vram_read(x, y);
}

static void dk_vram_transfer_out(int x, int y, int w, int h, uint16_t* data) {
    Deko3DRenderer::Instance().WaitPendingFences();
    sw_vram_transfer_out(x, y, w, h, data);
}

static int dk_render_display(uint32_t* out, int pitch, int dx, int dy, int dw, int dh) {
    return Deko3DRenderer::Instance().RenderDisplay(out, pitch, dx, dy, dw, dh);
}

static int dk_set_high_vram(const uint16_t* high, int tex_high, int clut_high) {
    return Deko3DRenderer::Instance().SetHighVram(high, tex_high, clut_high);
}

static void dk_high_vram_written(int x, int y, int w, int h) {
    Deko3DRenderer::Instance().HighVramWritten(x, y, w, h);
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
    .set_perspective_triangle       = dk_set_perspective_triangle,
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
    .vram_read                      = dk_vram_read,
    .vram_transfer_in               = dk_vram_transfer_in,
    .vram_transfer_out              = dk_vram_transfer_out,
    .set_draw_area                  = dk_set_draw_area,
    .get_draw_area                  = sw_get_draw_area,
    .set_draw_offset                = dk_set_draw_offset,
    .wide_configure                 = nullptr,
    .wide_set_target                = nullptr,
    .wide_disable_target            = nullptr,
    .wide_set_primitive_delta       = nullptr,
    .wide_set_primitive_clip_bottom = nullptr,
    .wide_fade_white                = nullptr,
    .wide_set_primitive_suppressed  = nullptr,
    .wide_set_primitive_unclipped   = nullptr,
    .wide_set_primitive_expanded    = nullptr,
    .wide_clear                     = nullptr,
    .wide_clear_margins             = nullptr,
    .wide_set_mirror_authoritative  = nullptr,
    .wide_set_sidecar_complete      = nullptr,
    .render_wide_display            = nullptr,
    .wide_dump_full                 = nullptr,
    .set_high_vram                  = dk_set_high_vram,
    .high_vram_written              = dk_high_vram_written,
};

const GpuRenderBackend* deko3d_backend_get(void) {
    return &DEKO3D_BACKEND;
}
