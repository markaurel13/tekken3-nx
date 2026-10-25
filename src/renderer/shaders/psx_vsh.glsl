#version 460

// Inputs de vértices
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;
layout (location = 3) in uvec2 inTPageClut;
layout (location = 4) in uint inFlags;

// Outputs al Fragment Shader
layout (location = 0) noperspective out vec2 outUV;
layout (location = 1) noperspective out vec4 outColor;
layout (location = 2) flat out uvec2 outTPageClut;
layout (location = 3) flat out uint outFlags;

// Uniform block: Parámetros de proyección del Viewport
layout (std140, binding = 0) uniform ViewportBlock {
    vec2 u_ScreenSize; // 1024.0, 512.0
    vec2 u_Offset;     // Draw offset (m_offsetX, m_offsetY)
    ivec4 u_TexWindow; // Texture window (mask_x, mask_y, off_x, off_y)
};

void main()
{
    // Las coordenadas inPos ya incluyen el draw offset aplicado por gpu.c en CPU.
    // Mapeo a NDC para Deko3D (VRAM 1024x512, origen en esquina superior izquierda):
    // NDC X: [0..1024] -> [-1.0..+1.0]
    // NDC Y: [0..512]  -> [-1.0..+1.0] (Y=0 es -1.0 [fila 0], Y=512 es +1.0 [fila 512])
    const float shift = 0.484375; // 0.5 - 1.0/64.0 (alineación de píxeles enteros PS1)
    vec2 ndc;
    ndc.x = (inPos.x + shift) / 512.0 - 1.0;
    ndc.y = 1.0 - (inPos.y + shift) / 256.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    outUV = inUV;
    outColor = inColor;
    outTPageClut = inTPageClut;
    outFlags = inFlags;
}
