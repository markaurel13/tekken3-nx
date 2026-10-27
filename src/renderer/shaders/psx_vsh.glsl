#version 460

// Inputs de vértices
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;
layout (location = 3) in uvec2 inTPageClut;
layout (location = 4) in uint inFlags;
layout (location = 5) in float inQ;

// Outputs al Fragment Shader
layout (location = 0) noperspective out vec2 outUV;
layout (location = 1) noperspective out vec4 outColor;
layout (location = 2) flat out uvec2 outTPageClut;
layout (location = 3) flat out uint outFlags;
layout (location = 4) smooth out vec2 outPerspUV;
layout (location = 5) flat out uint outIsPersp;

// Uniform block: Parámetros de proyección del Viewport
layout (std140, binding = 0) uniform ViewportBlock {
    vec2 u_ScreenSize; // 1024.0, 512.0
    vec2 u_Offset;     // Draw offset (m_offsetX, m_offsetY)
    ivec4 u_TexWindow; // Texture window (mask_x, mask_y, off_x, off_y)
    vec4 u_Extra;      // x = subpixel shift (0.5 / Scale), y = scale
};

void main()
{
    // Las coordenadas inPos ya incluyen el draw offset aplicado por gpu.c en CPU.
    // Mapeo a NDC para Deko3D (VRAM 1024x512, origen en esquina superior izquierda):
    // NDC X: [0..1024] -> [-1.0..+1.0]
    // NDC Y: [0..512]  -> [-1.0..+1.0] (Y=0 es -1.0 [fila 0], Y=512 es +1.0 [fila 512])
    float shift = u_Extra.x; // Alineación dinámica de subpíxeles según el factor de Supersampling
    vec2 ndc;
    ndc.x = (inPos.x + shift) / 512.0 - 1.0;
    ndc.y = 1.0 - (inPos.y + shift) / 256.0;

    // Corrección de perspectiva 3D para texturas:
    // La división por perspectiva por hardware de la GPU interpola outPerspUV hiperbólicamente
    // entre vértices mediante w = 1/q = Z sin desplazar los vértices en pantalla (ndc*w / w == ndc).
    float w = (inQ > 0.0) ? (1.0 / inQ) : 1.0;
    gl_Position = vec4(ndc * w, 0.0, w);

    outUV = inUV;
    outPerspUV = inUV;
    outIsPersp = (inQ > 0.0) ? 1u : 0u;

    outColor = inColor;
    outTPageClut = inTPageClut;
    outFlags = inFlags;
}
