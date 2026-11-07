#version 460

// Inputs de vértices
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;
layout (location = 3) in uvec2 inTPageClut;
layout (location = 4) in uint inFlags;
layout (location = 5) in float inQ;
layout (location = 6) in vec2 inHdUV;

// Outputs al Fragment Shader
layout (location = 0) noperspective out vec2 outUV;
layout (location = 1) noperspective out vec4 outColor;
layout (location = 2) flat out uvec2 outTPageClut;
layout (location = 3) flat out uint outFlags;
layout (location = 4) smooth out vec2 outPerspUV;
layout (location = 5) flat out uint outIsPersp;
layout (location = 6) noperspective out vec2 outHdUV;
layout (location = 7) smooth out vec2 outPerspHdUV;

// Uniform block: Parámetros de proyección del Viewport
layout (std140, binding = 0) uniform ViewportBlock {
    vec2 u_ScreenSize;   // 1024.0, 512.0
    vec2 u_Offset;       // Draw offset (m_offsetX, m_offsetY)
    ivec4 u_TexWindow;   // Texture window (mask_x, mask_y, off_x, off_y)
    vec4 u_Extra;        // x = subpixel shift (0.5 / Scale), y = scale
    vec4 u_WideParams;   // x = u_xoff, y = u_yoff, z = u_xhalf, w = u_xscale
    vec4 u_WideCenter;   // x = u_xcenter, y,z,w = unused
};

void main()
{
    // Las coordenadas inPos ya incluyen el draw offset aplicado por gpu.c en CPU.
    // Soporte Native-Wide Sidecar y estiramiento de cielo 2D:
    // u_xoff: traslación horizontal al espacio del Wide FBO (0.0 en canónico 4:3)
    // u_yoff: traslación vertical al espacio del Wide FBO (0.0 en canónico 4:3)
    // u_xhalf: semiancho de proyección horizontal (512.0 en canónico 4:3, wide_w * 0.5 en Wide)
    // u_xscale / u_xcenter: estiramiento de cielo/backdrop 2D alrededor de u_xcenter (1.0 / 0.0 en 3D y canónico)
    float shift = u_Extra.x; // Alineación dinámica de subpíxeles según el factor de Supersampling
    float xhalf = (u_WideParams.z > 0.0) ? u_WideParams.z : 512.0;
    float xscale = u_WideParams.w;
    float xcenter = u_WideCenter.x;

    float xb = inPos.x;
    if (xscale < 0.0) {
        float s = -xscale;
        float native_half = (u_WideCenter.y > 0.0) ? u_WideCenter.y : (xhalf / s);
        float l = xcenter - native_half;
        float r = xcenter + native_half;
        if (xb < l) xb = l + (xb - l) * s;
        else if (xb > r) xb = r + (xb - r) * s;
    } else {
        xb = (xb - xcenter) * xscale + xcenter;
    }

    vec2 ndc;
    ndc.x = (xb + shift + u_WideParams.x) / xhalf - 1.0;
    ndc.y = 1.0 - (inPos.y + shift + u_WideParams.y) / 256.0;

    // Corrección de perspectiva 3D para texturas:
    // La división por perspectiva por hardware de la GPU interpola outPerspUV hiperbólicamente
    // entre vértices mediante w = 1/q = Z sin desplazar los vértices en pantalla (ndc*w / w == ndc).
    float w = (inQ > 0.0) ? (1.0 / inQ) : 1.0;
    gl_Position = vec4(ndc * w, 0.0, w);

    outUV = inUV;
    outPerspUV = inUV;
    outIsPersp = (inQ > 0.0) ? 1u : 0u;

    outHdUV = inHdUV;
    outPerspHdUV = inHdUV;

    outColor = inColor;
    outTPageClut = inTPageClut;
    outFlags = inFlags;
}
