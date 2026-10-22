#version 460

// Inputs del Vertex Shader
layout (location = 0) in vec2 inTexCoord;
layout (location = 1) in vec4 inColor;

// Salida de color al framebuffer
layout (location = 0) out vec4 outColor;

// Sampler de textura (VRAM de PS1 mapeada a textura RGBA)
layout (binding = 0) uniform sampler2D s_PSXVram;

// Uniform block: Opciones de renderizado de la primitiva actual
layout (std140, binding = 1) uniform DrawModeBlock {
    int u_Textured;        // 0 = Flat/Gouraud, 1 = Con textura
    int u_RawTexture;      // 1 = Textura sin modulación de color, 0 = Modulado con inColor
    int u_SemiTransparent; // 0 = Opaco, 1 = Semi-transparente
    int u_BlendMode;       // Modos de PS1: 0 (0.5B+0.5F), 1 (B+F), 2 (B-F), 3 (B+0.25F)
};

void main()
{
    vec4 baseColor = inColor;

    if (u_Textured != 0) {
        vec4 texSample = texture(s_PSXVram, inTexCoord);

        // En PS1, si el pixel de textura es transparente (máscara o negro transparente)
        if (texSample.a < 0.01) {
            discard;
        }

        if (u_RawTexture != 0) {
            baseColor = texSample;
        } else {
            // Modulación de color PS1: (Color * Tex) * 2 aproximadamente en rango estándar
            baseColor = texSample * inColor * 2.0;
            baseColor = clamp(baseColor, 0.0, 1.0);
            baseColor.a = texSample.a;
        }
    }

    outColor = baseColor;
}
