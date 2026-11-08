#pragma once

#include <stdint.h>
#include <deko3d.hpp>
#include <array>
#include "tekken3_outfits.h"

struct HdTextureMap;

#ifdef __cplusplus
extern "C" {
#endif

// Función de entrada que devuelve el backend Deko3D para la vtable de psxrecomp
struct GpuRenderBackend;
const struct GpuRenderBackend* deko3d_backend_get(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// Estructura de vértice optimizada de 40 bytes para la GPU Tegra X1 Maxwell
struct PS1Vertex {
    float pos[2];          // location 0: 2x32 Float (X, Y coordenadas de VRAM)
    float uv[2];           // location 1: 2x32 Float (U, V coordenadas de textura)
    uint8_t color[4];      // location 2: 4x8 Unorm (R, G, B, A modulación / Gouraud)
    uint16_t tpage_clut[2];// location 3: 2x16 Uint (x: texpage, y: clut)
    uint32_t flags;        // location 4: 1x32 Uint (depth, raw, semi, blend, textured, abs, hd)
    float q;               // location 5: 1x32 Float (peso de perspectiva 1/Z del GTE; 0.0 = afín)
    float hd_uv[2];        // location 6: 2x32 Float (coordenadas de atlas HD [0..1])
};
static_assert(sizeof(PS1Vertex) == 40, "PS1Vertex must be exactly 40 bytes");

// Uniform buffer para proyección de Viewport y Texture Window (binding = 0 en psx_vsh.glsl y psx_fsh.glsl)
struct ViewportUniforms {
    float screenSize[2];   // 1024.0, 512.0
    float offset[2];       // m_offsetX, m_offsetY
    int32_t texWindow[4];  // mask_x, mask_y, off_x, off_y
    float extra[4];        // extra[0]: subpixel shift (0.5 / scale), extra[1]: scale
    float wideParams[4];   // x: u_xoff, y: u_yoff, z: u_xhalf, w: u_xscale
    float wideCenter[4];   // x: u_xcenter, y,z,w: padding
};

class Deko3DRenderer {
public:
    static constexpr unsigned NumFramebuffers = 3;
    static constexpr uint32_t TargetWidth = 1920;
    static constexpr uint32_t TargetHeight = 1080;
    static constexpr uint32_t HandheldWidth = 1280;
    static constexpr uint32_t HandheldHeight = 720;
    static constexpr uint32_t VramWidth = 1024;
    static constexpr uint32_t VramHeight = 512;
    static constexpr uint32_t RawVramHeight = 1024; // 0..511: PS1 VRAM, 512..1023: High VRAM (Tekken 3 Expanded)
    static constexpr uint32_t OsdMaxWidth = 1024;
    static constexpr uint32_t OsdMaxHeight = 1024;
    static constexpr unsigned CmdBufferSize = 4 * 1024 * 1024;       // 4 MB slice inicial por frame
    static constexpr unsigned CmdPoolTotalSize = 16 * 1024 * 1024;   // 16 MB pool total para comandos
    static constexpr unsigned MaxVerticesPerFrame = 98304;           // 96K vértices por frame (~3.84 MB en VertexRing 4MB)

    uint32_t m_internalScale = 2;
    uint32_t m_hrVramWidth = VramWidth * 2;
    uint32_t m_hrVramHeight = VramHeight * 2;
    bool m_widescreen = false;

    // Estado del compositor panorámico (Fase 1: Native-Wide Sidecar)
    int m_wideW = 0;              // Ancho nativo wide (ej. 490 para 16:9 en Tekken 3)
    int m_wideOff = 0;            // Offset de centrado en px nativos (ej. 61)
    int m_wideCurIdx = -1;        // Índice de superficie wide activa (-1 = ninguna, 0 = buffer 0, 1 = buffer 1)
    int m_wideCurBase = 0;        // Base X del framebuffer activo (0 o 368)

    // Deltas y propiedades por primitiva (enviados desde gpu.c / tekken3_widescreen_mod)
    int m_widePrimDx = 0;
    int m_widePrimDy = 0;
    int m_widePrimClipBottom = -1;
    int m_widePrimSuppressed = 0;
    int m_widePrimUnclipped = 0;
    int m_widePrimExpanded = 0;
    int m_wideMirrorAuthoritative = 0;
    int m_wideSidecarComplete = 0;

    static Deko3DRenderer& Instance();

    bool Init(uint16_t* vramPtr, uint32_t internalScale = 2, bool widescreen = false);
    void Shutdown();

    void BeginFrame();

    // Estado del viewport y primitivas
    void SetDrawOffset(int x, int y);
    void SetDrawArea(int x1, int y1, int x2, int y2);
    void SetTextureWindow(uint32_t raw);
    void SetSemiTransparency(int enabled, int mode);
    void SetPerspectiveTriangle(int enabled, float q0, float q1, float q2);
    void SetColorModulation(int r, int g, int b, int raw);

    // Métodos del compositor Native-Wide
    void WideConfigure(int wide_w, int offset);
    void WideSetTarget(int base_x);
    void WideDisableTarget();
    void WideSetPrimitiveDelta(int dx, int dy);
    void WideSetPrimitiveClipBottom(int y);
    void WideFadeWhite(int x, int y, int w, int h);
    void WideSetPrimitiveSuppressed(int suppressed);
    void WideSetPrimitiveUnclipped(int unclipped);
    void WideSetPrimitiveExpanded(int expanded);
    void WideClear(int base_x, int y, int h, uint16_t color);
    void WideClearMargins(int base_x, int y, int h, uint16_t color, int sides);
    void WideSetMirrorAuthoritative(int authoritative);
    void WideSetSidecarComplete(int complete);
    int RenderWideDisplay(uint32_t* out, int pitch, int base_x, int disp_y, int disp_h);

    // Primitivas de PS1 aceleradas por hardware GPU Maxwell
    void DrawFlatTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
    void DrawGouraudTriangle(int x0, int y0, uint16_t c0,
                             int x1, int y1, uint16_t c1,
                             int x2, int y2, uint16_t c2);
    void DrawTexturedTriangle(int x0, int y0, int u0, int v0,
                              int x1, int y1, int u1, int v1,
                              int x2, int y2, int u2, int v2,
                              uint16_t clut_x, uint16_t clut_y, uint16_t texpage);
    void DrawShadedTexturedTriangle(int x0, int y0, int u0, int v0, uint32_t col0,
                                    int x1, int y1, int u1, int v1, uint32_t col1,
                                    int x2, int y2, int u2, int v2, uint32_t col2,
                                    uint16_t clut_x, uint16_t clut_y, uint16_t texpage, int raw);

    void DrawRect(int x, int y, int w, int h, uint16_t color);
    void DrawLine(int x0, int y0, int x1, int y1, uint16_t color);
    void DrawShadedLine(int x0, int y0, uint16_t c0, int x1, int y1, uint16_t c1);
    void DrawTexturedRect(int x, int y, int w, int h, int u, int v,
                          uint16_t clut_x, uint16_t clut_y, uint16_t texpage);
    void DrawTexturedRectScaled(int x, int y, int w, int h,
                                int u0, int v0, int u1, int v1,
                                uint16_t clut_x, uint16_t clut_y, uint16_t texpage);

    void FillRect(int x, int y, int w, int h, uint16_t color);
    void CopyRect(int sx, int sy, int dx, int dy, int w, int h);

    // Transferencia de texturas/CLUTs desde CPU/DMA a GPU
    void VramTransferIn(int x, int y, int w, int h, const uint16_t* data);
    int SetHighVram(const uint16_t* high, int tex_high, int clut_high);
    void HighVramWritten(int x, int y, int w, int h);

    // Presentación directa sin conversión en CPU
    int RenderDisplay(uint32_t* out, int pitch, int dx, int dy, int dw, int dh);

    // Callback dinámico de adición de memoria para el command buffer
    void OnCmdBufAddMem(DkCmdBuf cmdbuf, size_t minReqSize);

    // Obtener número de vértices presentados en el último frame
    uint32_t GetLastFrameVtx() const { return m_lastFrameVtx; }

    static constexpr unsigned NumFrameResources = 2;
    static constexpr unsigned CmdPoolSliceSize = 8 * 1024 * 1024;   // 8 MB pool por frame
    static constexpr unsigned VertexRingSliceSize = 4 * 1024 * 1024;// 4 MB vertices por frame (131072 vertices)
    static constexpr uint32_t StagingRingStart = 0x00600000;         // 6 MB offset inicial (0x0..0x600000 reservado para depth24 FMVs)
    static constexpr uint32_t StagingRingEnd = 64 * 1024 * 1024;     // 64 MB límite de staging (58 MB anillo dinámico)
    static constexpr uint32_t StagingSliceSize = (StagingRingEnd - StagingRingStart) / NumFrameResources; // ~29 MB por frame

    // Sincronización de seguridad con fences GPU antes de lecturas de VRAM
    void WaitPendingFences();

    struct RenderTimings {
        uint64_t vram_prep_ns = 0;
        uint64_t submit_ns = 0;
        uint64_t present_ns = 0;
        uint64_t fence_wait_ns = 0;
        uint64_t gpu_fence_wait_ns = 0;
        uint64_t acq_wait_ns = 0;
        uint64_t vram_fence_wait_ns = 0;
        uint64_t vram_upload_ns = 0;
        uint64_t vram_upload_bytes = 0;
        uint64_t samples = 0;
    };
    RenderTimings GetAndResetRenderTimings();
    RenderTimings GetLastFrameTimings() const { return m_lastFrameTimings; }

private:
    Deko3DRenderer() = default;
    ~Deko3DRenderer() = default;

    bool LoadShader(dk::Shader& shader, dk::MemBlock& block, uint32_t offset, const char* path);
    void PushTriangle(const PS1Vertex& v0, const PS1Vertex& v1, const PS1Vertex& v2);
    void FlushBatch();
    void SyncRawVram();
    void DrawOsdImage(const uint32_t* argbPixels, int ow, int oh,
                      int vx, int vy, int dw, int dh,
                      uint32_t outW, uint32_t outH,
                      dk::ImageView& colorTarget);

    uint16_t* m_vram = nullptr;
    const uint16_t* m_highVram = nullptr;
    int m_texHigh = 0;
    int m_clutHigh = 0;
    bool m_highVramDirty = false;
    int m_highVramDirtyY1 = 0;
    int m_highVramDirtyY2 = 512;
    int m_offsetX = 0;
    int m_offsetY = 0;
    int m_clipX1 = 0, m_clipY1 = 0, m_clipX2 = 1024, m_clipY2 = 512;
    int m_twMaskX = 0, m_twMaskY = 0, m_twOffX = 0, m_twOffY = 0;
    int m_semiEnabled = 0;
    int m_semiMode = 0;
    int m_modR = 128, m_modG = 128, m_modB = 128, m_rawTex = 0;

    // Recursos Deko3D
    dk::UniqueDevice m_device;
    dk::UniqueQueue m_queue;
    dk::UniqueSwapchain m_swapchain;

    // Doble buffer de recursos GPU para paralelismo completo CPU/GPU
    int m_frameResIdx = 0;
    dk::UniqueCmdBuf m_cmdBufPool[NumFrameResources];
    dk::CmdBuf m_cmdBuf; // Handle activo para el slot actual
    DkFence m_frameFences[NumFrameResources]{};
    bool m_fenceInFlight[NumFrameResources] = {false, false};

    dk::MemBlock m_memImages;
    dk::MemBlock m_memCode;
    dk::MemBlock m_memData;
    dk::MemBlock m_memStaging;

    dk::Image m_framebuffers[NumFramebuffers];
    dk::Image m_vramTexture;      // 1024x512 RGBA8 Render Target y Display Source (canónico 4:3)
    dk::Image m_wideTextures[2];   // Superficies Native-Wide (para base_x = 0 y base_x = 368)
    dk::Image m_rawVramImage;     // 1024x1024 R16_Uint Fuente de texturas PS1 + High VRAM
    dk::Image m_osdImage;      // 1024x1024 RGBA8_Unorm Superposición OSD (carrusel de trajes/retratos y avisos)
    dk::Image m_skinImages[T3_SKIN_COUNT]; // Texturas de atlases HD de los 7 personajes
    bool m_skinLoaded[T3_SKIN_COUNT] = { false };
    HdTextureMap* m_skinMap = nullptr;

    void LoadAllHdSkinPacks(uint64_t& curImgOffset);
    void InvalidateSkinMaps();

    dk::Shader m_vsh;
    dk::Shader m_fsh;

    int m_curSlot = 0;

    // Buffers de vértices, descriptores, uniformes y staging
    PS1Vertex* m_vtxRingCpuPool[NumFrameResources] = {nullptr, nullptr};
    DkGpuAddr m_vtxRingGpuPool[NumFrameResources] = {0, 0};
    PS1Vertex* m_vtxRingCpu = nullptr;
    DkGpuAddr m_vtxRingGpu = 0;
    uint32_t m_vtxAllocated = 0;
    uint32_t m_vtxBatchStart = 0;
    uint32_t m_lastFrameVtx = 0;

    bool m_pqValid = false;
    float m_pq[3] = { 0.0f, 0.0f, 0.0f };
    uint64_t m_perspTriHits = 0;

    void* m_imgDescCpu = nullptr;
    DkGpuAddr m_imgDescGpu = 0;
    void* m_smpDescCpu = nullptr;
    DkGpuAddr m_smpDescGpu = 0;

    DkGpuAddr m_viewportUboGpuPool[NumFrameResources] = {0, 0};
    DkGpuAddr m_viewportUboGpu = 0;
    DkGpuAddr m_wideViewportUboGpuPool[NumFrameResources] = {0, 0};
    DkGpuAddr m_wideViewportUboGpu = 0;

    uint32_t* m_vramStagingCpu = nullptr;
    DkGpuAddr m_vramStagingGpu = 0;
    uint32_t m_stagingUploadOffset = StagingRingStart;
    uint32_t m_cmdPoolBase[NumFrameResources] = {0, 8 * 1024 * 1024};
    uint32_t m_cmdPoolOffset[NumFrameResources] = {CmdBufferSize, 8 * 1024 * 1024 + CmdBufferSize};

    bool m_vramDirty = true;
    bool m_initialized = false;
    RenderTimings m_timings{};
    RenderTimings m_lastFrameTimings{};
    uint64_t m_curFrameVramUploadNs = 0;
    uint64_t m_curFrameVramUploadBytes = 0;
    uint64_t m_curFrameGpuFenceNs = 0;
    uint64_t m_curFrameAcqWaitNs = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
uint32_t deko3d_get_last_frame_vtx(void);
#ifdef __cplusplus
}
#endif

#endif
