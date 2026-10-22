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

// Estructura de vértice optimizada para la GPU Tegra X1
struct PS1Vertex {
    float pos[2];     // X, Y (coordenadas de pantalla/VRAM)
    float uv[2];      // U, V (textura normalizada)
    uint8_t color[4]; // R, G, B, A (Gouraud / modulación)
};

// Uniform buffer para proyección de Viewport (binding = 0 en psx_vsh.glsl)
struct ViewportUniforms {
    float screenSize[2];
    float offset[2];
};

// Uniform buffer para los modos de primitiva (binding = 1 en psx_fsh.glsl)
struct DrawModeUniforms {
    int32_t textured;
    int32_t rawTexture;
    int32_t semiTransparent;
    int32_t blendMode;
};

class Deko3DRenderer {
public:
    static constexpr unsigned NumFramebuffers = 2;
    static constexpr uint32_t TargetWidth = 1280;
    static constexpr uint32_t TargetHeight = 720;
    static constexpr uint32_t VramWidth = 1024;
    static constexpr uint32_t VramHeight = 512;
    static constexpr unsigned CmdBufferSize = 0x40000;
    static constexpr unsigned MaxVerticesPerFrame = 65536;

    static Deko3DRenderer& Instance();

    bool Init(uint16_t* vramPtr);
    void Shutdown();

    void BeginFrame();

    // Estado del viewport y primitivas
    void SetDrawOffset(int x, int y);
    void SetDrawArea(int x1, int y1, int x2, int y2);
    void SetSemiTransparency(int enabled, int mode);
    void SetColorModulation(int r, int g, int b, int raw);

    // Primitivas de PS1
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

    void FillRect(int x, int y, int w, int h, uint16_t color);
    void CopyRect(int sx, int sy, int dx, int dy, int w, int h);

    // Presentación
    int RenderDisplay(uint32_t* out, int pitch, int dx, int dy, int dw, int dh);

private:
    Deko3DRenderer() = default;
    ~Deko3DRenderer() = default;

    bool LoadShader(dk::Shader& shader, dk::MemBlock& block, uint32_t offset, const char* path);
    void PushTriangle(const PS1Vertex& v0, const PS1Vertex& v1, const PS1Vertex& v2);
    void FlushBatch();

    uint16_t* m_vram = nullptr;
    int m_offsetX = 0;
    int m_offsetY = 0;
    int m_clipX1 = 0, m_clipY1 = 0, m_clipX2 = 1024, m_clipY2 = 512;
    int m_semiEnabled = 0;
    int m_semiMode = 0;
    int m_modR = 128, m_modG = 128, m_modB = 128, m_rawTex = 0;

    // Recursos Deko3D
    dk::UniqueDevice m_device;
    dk::UniqueQueue m_queue;
    dk::UniqueCmdBuf m_cmdBuf;
    dk::UniqueSwapchain m_swapchain;

    dk::MemBlock m_memImages;
    dk::MemBlock m_memCode;
    dk::MemBlock m_memData;
    dk::MemBlock m_memStaging;

    dk::Image m_framebuffers[NumFramebuffers];
    dk::Image m_vramTexture;

    dk::Shader m_vsh;
    dk::Shader m_fsh;

    int m_curSlot = 0;

    // Buffers de vértices, descriptores, uniformes y staging en m_memData
    PS1Vertex* m_vtxRingCpu = nullptr;
    DkGpuAddr m_vtxRingGpu = 0;
    uint32_t m_vtxCount = 0;

    void* m_imgDescCpu = nullptr;
    DkGpuAddr m_imgDescGpu = 0;
    void* m_smpDescCpu = nullptr;
    DkGpuAddr m_smpDescGpu = 0;

    ViewportUniforms* m_viewportUboCpu = nullptr;
    DkGpuAddr m_viewportUboGpu = 0;
    DrawModeUniforms* m_drawModeUboCpu = nullptr;
    DkGpuAddr m_drawModeUboGpu = 0;

    uint32_t* m_vramStagingCpu = nullptr;
    DkGpuAddr m_vramStagingGpu = 0;

    DrawModeUniforms m_currentDrawMode{};
    bool m_vramDirty = true;
    bool m_initialized = false;
};

#endif
