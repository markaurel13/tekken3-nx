#version 460

// Inputs de vértices
layout (location = 0) in vec2 inPos;      // Posición en píxeles (espacio VRAM/pantalla)
layout (location = 1) in vec2 inTexCoord; // Coordenadas UV normalizadas [0, 1]
layout (location = 2) in vec4 inColor;    // Color RGBA de vértice (Gouraud / modulación)

// Outputs al Fragment Shader
layout (location = 0) out vec2 outTexCoord;
layout (location = 1) out vec4 outColor;

// Uniform block: Parámetros de proyección del Viewport
layout (std140, binding = 0) uniform ViewportBlock {
    vec2 u_ScreenSize; // Ancho y alto de la pantalla destino (ej. 1280x720 o 1024x512)
    vec2 u_Offset;     // Desplazamiento de dibujo (draw offset de PS1)
};

void main()
{
    // Transformar de espacio de píxeles [0, W] x [0, H] a NDC [-1.0, +1.0]
    vec2 screenPos = inPos + u_Offset;
    vec2 ndc;
    ndc.x = (screenPos.x / (u_ScreenSize.x * 0.5)) - 1.0;
    ndc.y = 1.0 - (screenPos.y / (u_ScreenSize.y * 0.5)); // Invertir eje Y para origen superior izquierdo

    gl_Position = vec4(ndc, 0.0, 1.0);
    outTexCoord = inTexCoord;
    outColor = inColor;
}
