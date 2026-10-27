#pragma once

#include <stdint.h>
#include <deko3d.hpp>
#include <array>

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

// Estructura de vértice optimizada de 32 bytes para la GPU Tegra X1 Maxwell
struct PS1Vertex {
    float pos[2];          // location 0: 2x32 Float (X, Y coordenadas de VRAM)
    float uv[2];           // location 1: 2x32 Float (U, V coordenadas de textura)
    uint8_t color[4];      // location 2: 4x8 Unorm (R, G, B, A modulación / Gouraud)
    uint16_t tpage_clut[2];// location 3: 2x16 Uint (x: texpage, y: clut)
    uint32_t flags;        // location 4: 1x32 Uint (depth, raw, semi, blend, textured, abs)
    uint32_t pad;          // alineación a 32 bytes
};

// Uniform buffer para proyección de Viewport y Texture Window (binding = 0 en psx_vsh.glsl y psx_fsh.glsl)
struct ViewportUniforms {
    float screenSize[2];   // 1024.0, 512.0
    float offset[2];       // m_offsetX, m_offsetY
    int32_t texWindow[4];  // mask_x, mask_y, off_x, off_y
};

class Deko3DRenderer {
public:
    static constexpr unsigned NumFramebuffers = 3;
    static constexpr uint32_t TargetWidth = 1280;
    static constexpr uint32_t TargetHeight = 720;
    static constexpr uint32_t VramWidth = 1024;
    static constexpr uint32_t VramHeight = 512;
    static constexpr unsigned CmdBufferSize = 4 * 1024 * 1024;       // 4 MB slice inicial por frame
    static constexpr unsigned CmdPoolTotalSize = 16 * 1024 * 1024;   // 16 MB pool total para comandos
    static constexpr unsigned MaxVerticesPerFrame = 131072;          // 128K vértices por frame (~3.5 MB)
    static constexpr uint32_t StagingRingStart = 0x00600000;         // 6 MB offset inicial (0x0..0x600000 reservado para depth24 FMVs)
    static constexpr uint32_t StagingRingEnd = 64 * 1024 * 1024;     // 64 MB límite de staging (58 MB anillo dinámico)

    static Deko3DRenderer& Instance();

    bool Init(uint16_t* vramPtr);
    void Shutdown();

    void BeginFrame();

    // Estado del viewport y primitivas
    void SetDrawOffset(int x, int y);
    void SetDrawArea(int x1, int y1, int x2, int y2);
    void SetTextureWindow(uint32_t raw);
    void SetSemiTransparency(int enabled, int mode);
    void SetColorModulation(int r, int g, int b, int raw);

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
    void DrawTexturedRect(int x, int y, int w, int h, int u, int v,
                          uint16_t clut_x, uint16_t clut_y, uint16_t texpage);
    void DrawTexturedRectScaled(int x, int y, int w, int h,
                                int u0, int v0, int u1, int v1,
                                uint16_t clut_x, uint16_t clut_y, uint16_t texpage);

    void FillRect(int x, int y, int w, int h, uint16_t color);
    void CopyRect(int sx, int sy, int dx, int dy, int w, int h);

    // Transferencia de texturas/CLUTs desde CPU/DMA a GPU
    void VramTransferIn(int x, int y, int w, int h, const uint16_t* data);

    // Presentación directa sin conversión en CPU
    int RenderDisplay(uint32_t* out, int pitch, int dx, int dy, int dw, int dh);

    // Callback dinámico de adición de memoria para el command buffer
    void OnCmdBufAddMem(DkCmdBuf cmdbuf, size_t minReqSize);

    // Obtener número de vértices presentados en el último frame
    uint32_t GetLastFrameVtx() const { return m_lastFrameVtx; }

    static constexpr unsigned NumFrameResources = 2;
    static constexpr unsigned CmdPoolSliceSize = 8 * 1024 * 1024;   // 8 MB pool por frame
    static constexpr unsigned VertexRingSliceSize = 4 * 1024 * 1024;// 4 MB vertices por frame (131072 vertices)

    // Sincronización de seguridad con fences GPU antes de lecturas de VRAM
    void WaitPendingFences();

    struct RenderTimings {
        uint64_t vram_prep_ns = 0;
        uint64_t submit_ns = 0;
        uint64_t present_ns = 0;
        uint64_t fence_wait_ns = 0;
        uint64_t samples = 0;
    };
    RenderTimings GetAndResetRenderTimings();

private:
    Deko3DRenderer() = default;
    ~Deko3DRenderer() = default;

    bool LoadShader(dk::Shader& shader, dk::MemBlock& block, uint32_t offset, const char* path);
    void PushTriangle(const PS1Vertex& v0, const PS1Vertex& v1, const PS1Vertex& v2);
    void FlushBatch();
    void SyncRawVram();

    uint16_t* m_vram = nullptr;
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
    dk::Image m_vramTexture;   // 1024x512 RGBA8 Render Target y Display Source
    dk::Image m_rawVramImage;   // 1024x512 R16_Uint Fuente de texturas PS1

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

    void* m_imgDescCpu = nullptr;
    DkGpuAddr m_imgDescGpu = 0;
    void* m_smpDescCpu = nullptr;
    DkGpuAddr m_smpDescGpu = 0;

    ViewportUniforms* m_viewportUboCpu = nullptr;
    DkGpuAddr m_viewportUboGpu = 0;

    uint32_t* m_vramStagingCpu = nullptr;
    DkGpuAddr m_vramStagingGpu = 0;
    uint32_t m_stagingUploadOffset = StagingRingStart;
    uint32_t m_cmdPoolBase[NumFrameResources] = {0, 8 * 1024 * 1024};
    uint32_t m_cmdPoolOffset[NumFrameResources] = {CmdBufferSize, 8 * 1024 * 1024 + CmdBufferSize};

    bool m_vramDirty = true;
    bool m_initialized = false;
    RenderTimings m_timings{};
};

#endif
