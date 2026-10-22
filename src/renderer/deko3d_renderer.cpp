#include "deko3d_renderer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <vector>
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

    // Copiar el código del shader directamente en el bloque de memoria ejecutable de GPU
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

    return true;
}

bool Deko3DRenderer::Init(uint16_t* vramPtr) {
    if (m_initialized) {
        m_vram = vramPtr;
        return true;
    }
    m_vram = vramPtr;
    printf("[Deko3D] Inicializando pipeline gráfico...\n");

    // 1. Crear Device y Graphics Queue
    m_device = dk::DeviceMaker{}.create();
    m_queue = dk::QueueMaker{m_device}.setFlags(DkQueueFlags_Graphics).create();

    // 2. Asignar Memoria GPU
    // Bloque para Framebuffers y Textura de VRAM (32 MB)
    m_memImages = dk::MemBlockMaker{m_device, 32 * 1024 * 1024}
        .setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
        .create();

    // Bloque para Shaders (128 KB ejecutable)
    m_memCode = dk::MemBlockMaker{m_device, 128 * 1024}
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code)
        .create();

    // Bloque para Datos de Vértices, Comandos, Descriptores y UBOs (2 MB)
    m_memData = dk::MemBlockMaker{m_device, 2 * 1024 * 1024}
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
        .create();

    // Bloque dedicado con caché de CPU activa para el Staging Buffer RGBA8 1024x512 (2 MB)
    m_memStaging = dk::MemBlockMaker{m_device, 2 * 1024 * 1024}
        .setFlags(DkMemBlockFlags_CpuCached | DkMemBlockFlags_GpuCached)
        .create();

    // 3. Crear Command Buffer
    m_cmdBuf = dk::CmdBufMaker{m_device}.create();

    // 4. Configurar Layout de Framebuffers (1280x720 RGBA8)
    dk::ImageLayout fbLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_Usage2DEngine | DkImageFlags_HwCompression)
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

    // 5. Crear Swapchain con nwindowGetDefault()
    m_swapchain = dk::SwapchainMaker{m_device, nwindowGetDefault(), fbPointers.data(), NumFramebuffers}.create();

    // 6. Cargar Shaders desde romfs
    uint32_t vshOffset = 0;
    uint32_t fshOffset = 0x1000;
    if (!LoadShader(m_vsh, m_memCode, vshOffset, "romfs:/shaders/psx_vsh.dksh")) {
        printf("[Deko3D] Advertencia: fallo al cargar romfs:/shaders/psx_vsh.dksh\n");
    }
    if (!LoadShader(m_fsh, m_memCode, fshOffset, "romfs:/shaders/psx_fsh.dksh")) {
        printf("[Deko3D] Advertencia: fallo al cargar romfs:/shaders/psx_fsh.dksh\n");
    }

    // 7. Configurar Textura VRAM de PS1 (1024x512 RGBA8)
    dk::ImageLayout vramLayout;
    dk::ImageLayoutMaker{m_device}
        .setFlags(DkImageFlags_BlockLinear | DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(VramWidth, VramHeight)
        .initialize(vramLayout);

    curImgOffset = (curImgOffset + vramLayout.getAlignment() - 1) & ~(vramLayout.getAlignment() - 1);
    m_vramTexture.initialize(vramLayout, m_memImages, curImgOffset);

    // 8. Mapear regiones en m_memData (8 MB):
    // 0x000000: Command Buffer (256 KB)
    // 0x040000: Vertex Ring Buffer (1 MB)
    // 0x140000: Image Descriptor Set (256 B)
    // 0x140100: Sampler Descriptor Set (256 B)
    // 0x140200: ViewportBlock UBO (256 B, alineado a DK_UNIFORM_BUF_ALIGNMENT)
    // 0x140300: DrawModeBlock UBO (256 B, alineado a DK_UNIFORM_BUF_ALIGNMENT)
    // 0x200000: Staging Buffer RGBA8 1024x512 (2 MB)
    uint8_t* dataCpu = (uint8_t*)m_memData.getCpuAddr();
    DkGpuAddr dataGpu = m_memData.getGpuAddr();

    m_vtxRingCpu = (PS1Vertex*)(dataCpu + CmdBufferSize);
    m_vtxRingGpu = dataGpu + CmdBufferSize;
    m_vtxCount = 0;

    m_imgDescCpu = dataCpu + 0x140000;
    m_imgDescGpu = dataGpu + 0x140000;
    m_smpDescCpu = dataCpu + 0x140100;
    m_smpDescGpu = dataGpu + 0x140100;

    m_viewportUboCpu = (ViewportUniforms*)(dataCpu + 0x140200);
    m_viewportUboGpu = dataGpu + 0x140200;
    m_drawModeUboCpu = (DrawModeUniforms*)(dataCpu + 0x140300);
    m_drawModeUboGpu = dataGpu + 0x140300;

    m_vramStagingCpu = (uint32_t*)m_memStaging.getCpuAddr();
    m_vramStagingGpu = m_memStaging.getGpuAddr();

    // Inicializar descriptores de imagen y sampler para m_vramTexture
    dk::ImageView vramView{ m_vramTexture };
    dk::ImageDescriptor imgDesc;
    imgDesc.initialize(vramView);
    memcpy(m_imgDescCpu, &imgDesc, sizeof(imgDesc));

    dk::Sampler sampler;
    sampler.setFilter(DkFilter_Linear, DkFilter_Linear);
    sampler.setWrapMode(DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge);
    dk::SamplerDescriptor smpDesc;
    smpDesc.initialize(sampler);
    memcpy(m_smpDescCpu, &smpDesc, sizeof(smpDesc));

    // Inicializar el primer frame para que el command buffer esté siempre activo
    BeginFrame();

    m_initialized = true;
    printf("[Deko3D] Pipeline gráfico listo para renderizar a 60 FPS.\n");
    return true;
}

void Deko3DRenderer::BeginFrame() {
    m_queue.waitIdle();
    m_cmdBuf.clear();
    m_cmdBuf.addMemory(m_memData, 0, CmdBufferSize);

    m_curSlot = m_queue.acquireImage(m_swapchain);

    dk::ImageView colorTarget{ m_framebuffers[m_curSlot] };
    m_cmdBuf.bindRenderTargets(&colorTarget);

    DkViewport viewport{ 0.0f, 0.0f, (float)TargetWidth, (float)TargetHeight, 0.0f, 1.0f };
    m_cmdBuf.setViewports(0, { viewport });

    DkScissor scissor{ 0, 0, TargetWidth, TargetHeight };
    m_cmdBuf.setScissors(0, { scissor });

    m_cmdBuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);

    m_vtxCount = 0;
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
    m_offsetX = x;
    m_offsetY = y;
}

void Deko3DRenderer::SetDrawArea(int x1, int y1, int x2, int y2) {
    m_clipX1 = x1;
    m_clipY1 = y1;
    m_clipX2 = x2;
    m_clipY2 = y2;
}

void Deko3DRenderer::SetSemiTransparency(int enabled, int mode) {
    m_semiEnabled = enabled;
    m_semiMode = mode;
    m_currentDrawMode.semiTransparent = enabled;
    m_currentDrawMode.blendMode = mode;
}

void Deko3DRenderer::SetColorModulation(int r, int g, int b, int raw) {
    m_modR = r;
    m_modG = g;
    m_modB = b;
    m_rawTex = raw;
}

void Deko3DRenderer::PushTriangle(const PS1Vertex& v0, const PS1Vertex& v1, const PS1Vertex& v2) {
    if (m_vtxCount + 3 > MaxVerticesPerFrame) {
        FlushBatch();
    }

    m_vtxRingCpu[m_vtxCount + 0] = v0;
    m_vtxRingCpu[m_vtxCount + 1] = v1;
    m_vtxRingCpu[m_vtxCount + 2] = v2;
    m_vtxCount += 3;
}

void Deko3DRenderer::FlushBatch() {
    if (m_vtxCount == 0) return;

    // Subir descriptores y constantes de UBO por el command buffer (coherencia con GpuCached)
    dk::ImageView vramView{ m_vramTexture };
    dk::ImageDescriptor imgDesc;
    imgDesc.initialize(vramView);
    m_cmdBuf.pushData(m_imgDescGpu, &imgDesc, sizeof(imgDesc));

    dk::Sampler sampler;
    sampler.setFilter(DkFilter_Linear, DkFilter_Linear);
    sampler.setWrapMode(DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge);
    dk::SamplerDescriptor smpDesc;
    smpDesc.initialize(sampler);
    m_cmdBuf.pushData(m_smpDescGpu, &smpDesc, sizeof(smpDesc));

    m_cmdBuf.pushConstants(m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT, 0, sizeof(ViewportUniforms), m_viewportUboCpu);
    m_cmdBuf.pushConstants(m_drawModeUboGpu, DK_UNIFORM_BUF_ALIGNMENT, 0, sizeof(DrawModeUniforms), m_drawModeUboCpu);

    m_cmdBuf.barrier(DkBarrier_Full,
                     DkInvalidateFlags_Image | DkInvalidateFlags_Shader |
                     DkInvalidateFlags_Descriptors | DkInvalidateFlags_L2Cache);

    // Vincular shaders de PS1
    DkShader const shaders[] = { m_vsh, m_fsh };
    m_cmdBuf.bindShaders(DkStageFlag_GraphicsMask, shaders);

    // Vincular Uniform Buffers (ViewportBlock en Vertex binding=0, DrawModeBlock en Fragment binding=0 y 1)
    m_cmdBuf.bindUniformBuffer(DkStage_Vertex, 0, m_viewportUboGpu, DK_UNIFORM_BUF_ALIGNMENT);
    m_cmdBuf.bindUniformBuffer(DkStage_Fragment, 0, m_drawModeUboGpu, DK_UNIFORM_BUF_ALIGNMENT);
    m_cmdBuf.bindUniformBuffer(DkStage_Fragment, 1, m_drawModeUboGpu, DK_UNIFORM_BUF_ALIGNMENT);

    // Vincular descriptores y textura de VRAM en Fragment binding=0
    m_cmdBuf.bindImageDescriptorSet(m_imgDescGpu, 1);
    m_cmdBuf.bindSamplerDescriptorSet(m_smpDescGpu, 1);
    m_cmdBuf.bindTextures(DkStage_Fragment, 0, dkMakeTextureHandle(0, 0));

    // Rasterizador sin cull de caras
    dk::RasterizerState rState;
    rState.setCullMode(DkFace_None);
    m_cmdBuf.bindRasterizerState(rState);

    // Desactivar test de profundidad para renderizado 2D/VRAM
    dk::DepthStencilState dsState;
    dsState.setDepthTestEnable(false);
    dsState.setDepthWriteEnable(false);
    dsState.setStencilTestEnable(false);
    m_cmdBuf.bindDepthStencilState(dsState);

    dk::ColorState cState;
    m_cmdBuf.bindColorState(cState);

    dk::ColorWriteState cwState;
    m_cmdBuf.bindColorWriteState(cwState);

    dk::BlendState bState;
    m_cmdBuf.bindBlendStates(0, { bState });

    // Configurar Scissor a pantalla completa
    DkScissor scissor{ 0, 0, TargetWidth, TargetHeight };
    m_cmdBuf.setScissors(0, { scissor });

    // Configurar atributos y buffer de vértices
    static const std::array<DkVtxAttribState, 3> vtxAttribState = {{
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, pos),   DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, uv),    DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(PS1Vertex, color), DkVtxAttribSize_4x8,  DkVtxAttribType_Unorm, 0 },
    }};
    DkVtxBufferState vtxBufferState{ sizeof(PS1Vertex), 0 };
    m_cmdBuf.bindVtxAttribState(vtxAttribState);
    m_cmdBuf.bindVtxBufferState({ vtxBufferState });
    m_cmdBuf.bindVtxBuffers(0, { { m_vtxRingGpu, (uint32_t)(m_vtxCount * sizeof(PS1Vertex)) } });

    // Emitir draw call
    m_cmdBuf.draw(DkPrimitive_Triangles, m_vtxCount, 1, 0, 0);

    m_vtxCount = 0;
}

void Deko3DRenderer::DrawFlatTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    sw_draw_flat_triangle(x0, y0, x1, y1, x2, y2, color);
}

void Deko3DRenderer::DrawGouraudTriangle(int x0, int y0, uint16_t c0,
                                        int x1, int y1, uint16_t c1,
                                        int x2, int y2, uint16_t c2) {
    sw_draw_gouraud_triangle(x0, y0, c0, x1, y1, c1, x2, y2, c2);
}

void Deko3DRenderer::DrawTexturedTriangle(int x0, int y0, int u0, int v0,
                                         int x1, int y1, int u1, int v1,
                                         int x2, int y2, int u2, int v2,
                                         uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    sw_draw_textured_triangle(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, clut_x, clut_y, texpage);
}

void Deko3DRenderer::DrawShadedTexturedTriangle(int x0, int y0, int u0, int v0, uint32_t col0,
                                               int x1, int y1, int u1, int v1, uint32_t col1,
                                               int x2, int y2, int u2, int v2, uint32_t col2,
                                               uint16_t clut_x, uint16_t clut_y, uint16_t texpage, int raw) {
    sw_draw_shaded_textured_triangle(x0, y0, u0, v0, col0, x1, y1, u1, v1, col1, x2, y2, u2, v2, col2, clut_x, clut_y, texpage, raw);
}

void Deko3DRenderer::DrawRect(int x, int y, int w, int h, uint16_t color) {
    sw_draw_flat_rect(x, y, w, h, color);
}

void Deko3DRenderer::DrawTexturedRect(int x, int y, int w, int h, int u, int v,
                                     uint16_t clut_x, uint16_t clut_y, uint16_t texpage) {
    sw_draw_textured_rect(x, y, w, h, u, v, clut_x, clut_y, texpage);
}

void Deko3DRenderer::FillRect(int x, int y, int w, int h, uint16_t color) {
    sw_fill_rect(x, y, w, h, color);
}

void Deko3DRenderer::CopyRect(int sx, int sy, int dx, int dy, int w, int h) {
    sw_copy_rect(sx, sy, dx, dy, w, h);
    m_vramDirty = true;
}

int Deko3DRenderer::RenderDisplay(uint32_t* out, int pitch, int dx, int dy, int dw, int dh) {
    static uint64_t s_present_cnt = 0;
    u64 render_tick_start = armGetSystemTick();
    if (out != nullptr) {
        return sw_render_display(out, pitch, dx, dy, dw, dh);
    }

    if (!m_initialized) {
        return 0;
    }

    GpuDisplayInfo di{};
    gpu_get_display_info(&di);

    uint32_t w = di.width;
    uint32_t h = di.height;
    if (w == 0) w = 320;
    if (h == 0) h = 240;
    if (w > VramWidth) w = VramWidth;
    if (h > VramHeight) h = VramHeight;

    uint32_t nonZeroPixels = 0;

    if ((s_present_cnt <= 5) || ((s_present_cnt % 120) == 0)) {
        printf("[Deko3DPerf] frame=%llu begin size=%ux%u dirty=%d\n",
               (unsigned long long)s_present_cnt,
               w, h,
               m_vramDirty ? 1 : 0);
    }

    if (!di.disabled && m_vram && m_vramStagingCpu) {
        if (di.depth24) {
            for (uint32_t y = 0; y < h; y++) {
                uint32_t* dstRow = m_vramStagingCpu + (size_t)y * w;
                gpu_depth24_present_row(&di, y, dstRow, w);
                for (uint32_t x = 0; x < w; x++) {
                    uint32_t argb = dstRow[x];
                    uint32_t r = (argb >> 16) & 0xFFu;
                    uint32_t g = (argb >> 8) & 0xFFu;
                    uint32_t b = argb & 0xFFu;
                    if (r | g | b) nonZeroPixels++;
                    dstRow[x] = r | (g << 8) | (b << 16) | 0xFF000000u;
                }
            }
        } else {
            for (uint32_t y = 0; y < h; y++) {
                uint32_t vy = (di.display_y + y) & (VramHeight - 1);
                const uint16_t* srcRow = m_vram + (size_t)vy * VramWidth;
                uint32_t* dstRow = m_vramStagingCpu + (size_t)y * w;
                for (uint32_t x = 0; x < w; x++) {
                    uint16_t c = srcRow[(di.display_x + x) & (VramWidth - 1)];
                    if (c & 0x7FFFu) nonZeroPixels++;
                    uint32_t r5 = c & 0x1Fu;
                    uint32_t g5 = (c >> 5) & 0x1Fu;
                    uint32_t b5 = (c >> 10) & 0x1Fu;
                    uint32_t r8 = (r5 << 3) | (r5 >> 2);
                    uint32_t g8 = (g5 << 3) | (g5 >> 2);
                    uint32_t b8 = (b5 << 3) | (b5 >> 2);
                    dstRow[x] = r8 | (g8 << 8) | (b8 << 16) | 0xFF000000u;
                }
            }
        }

        // Volcar caché L1/L2 de CPU del bloque de staging antes de que el motor DMA/2D de GPU lo lea
        m_memStaging.flushCpuCache(0, (size_t)w * h * sizeof(uint32_t));

        // Invalidar L2 de GPU antes de leer el buffer de staging escrito por CPU
        m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_L2Cache);

        // Subir región activa de VRAM a m_vramTexture en la GPU Tegra X1
        dk::ImageView vramView{ m_vramTexture };
        dk::ImageView colorTarget{ m_framebuffers[m_curSlot] };
        m_cmdBuf.copyBufferToImage({ m_vramStagingGpu, 0, 0 }, vramView, { 0, 0, 0, w, h, 1 });
        m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image | DkInvalidateFlags_L2Cache);

        // Blit y escalado directo por hardware 2D Engine hacia el framebuffer actual (1280x720)
        m_cmdBuf.blitImage(vramView, { 0, 0, 0, w, h, 1 },
                           colorTarget, { 0, 0, 0, TargetWidth, TargetHeight, 1 },
                           DkBlitFlag_FilterLinear);
        m_cmdBuf.barrier(DkBarrier_Full, DkInvalidateFlags_Image);
    }

    if (m_vtxCount > 0) {
        FlushBatch();
    }

    s_present_cnt++;
    if (s_present_cnt <= 5 || (s_present_cnt % 300) == 0) {
        uint64_t gp0_nop = 0, gp0_fill = 0, gp0_draw = 0, gp0_env = 0, gp0_copy = 0;
        gpu_get_gp0_stats(&gp0_nop, &gp0_fill, &gp0_draw, &gp0_env, &gp0_copy);
        printf("[Deko3D] present=%llu qErr=%d disp=(%u,%u %ux%u d24=%d dis=%d) nonZeroPx=%u gp0=(fill=%llu draw=%llu copy=%llu env=%llu)\n",
               (unsigned long long)s_present_cnt,
               (int)m_queue.isInErrorState(),
               di.display_x, di.display_y, w, h, di.depth24, di.disabled,
               nonZeroPixels,
               (unsigned long long)gp0_fill,
               (unsigned long long)gp0_draw,
               (unsigned long long)gp0_copy,
               (unsigned long long)gp0_env);
    }

    // Finalizar la lista de comandos y enviar a la cola gráfica
    DkCmdList finalCmdList = m_cmdBuf.finishList();
    m_queue.submitCommands(finalCmdList);

    // Enviar a pantalla y sincronizar VSync
    m_queue.presentImage(m_swapchain, m_curSlot);

    // Iniciar inmediatamente el siguiente frame para los comandos de dibujo
    BeginFrame();

    if (s_present_cnt <= 5 || (s_present_cnt % 120) == 0) {
        u64 render_tick_end = armGetSystemTick();
        printf("[Deko3DPerf] frame=%llu present_ns=%llu qErr=%d dirty=%d\n",
               (unsigned long long)s_present_cnt,
               (unsigned long long)armTicksToNs(render_tick_end - render_tick_start),
               (int)m_queue.isInErrorState(),
               m_vramDirty ? 1 : 0);
    }

    return (int)(w * h);
}

// =============================================================================
// Implementación de la interfaz GpuRenderBackend para psxrecomp
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

static int dk_render_display(uint32_t* out, int pitch, int dx, int dy, int dw, int dh) {
    return Deko3DRenderer::Instance().RenderDisplay(out, pitch, dx, dy, dw, dh);
}

// Vtable completa de Deko3D (con respaldo completo de VRAM de PS1 en sw_*)
static const GpuRenderBackend DEKO3D_BACKEND = {
    .name                           = "deko3d",
    .init                           = dk_init,
    .set_scale                      = sw_renderer_set_scale,
    .scale                          = sw_renderer_scale,
    .set_texture_filter             = sw_set_texture_filter,
    .texture_filter                 = sw_texture_filter,
    .set_semi_transparency          = dk_set_semi_transparency,
    .set_mask_bits                  = sw_set_mask_bits,
    .set_texture_window             = sw_set_texture_window,
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
    .draw_textured_rect_scaled      = sw_draw_textured_rect_scaled,
    .draw_line                      = sw_draw_line,
    .draw_shaded_line               = sw_draw_shaded_line,
    .render_display                 = dk_render_display,
    .render_display_hires           = sw_render_display_hires,
    .vram_write                     = sw_vram_write,
    .vram_read                      = sw_vram_read,
    .vram_transfer_in               = sw_vram_transfer_in,
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
