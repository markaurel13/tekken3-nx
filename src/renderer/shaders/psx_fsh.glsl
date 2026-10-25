#version 460

// Inputs del Vertex Shader
layout (location = 0) noperspective in vec2 inUV;
layout (location = 1) noperspective in vec4 inColor;
layout (location = 2) flat in uvec2 inTPageClut;
layout (location = 3) flat in uint inFlags;

// Salida de color al framebuffer
layout (location = 0) out vec4 outColor;

// Sampler de textura de VRAM pura (1024x512 R16_Uint)
layout (binding = 0) uniform usampler2D s_rawVram;

// Uniform block: Parámetros del Viewport y Texture Window
layout (std140, binding = 0) uniform ViewportBlock {
    vec2 u_ScreenSize;
    vec2 u_Offset;
    ivec4 u_TexWindow; // mask_x, mask_y, off_x, off_y
};

int vram_at(int x, int y) {
    return int(texelFetch(s_rawVram, ivec2(x & 1023, y & 511), 0).r);
}

int fetch_texel(int u, int v, int depth, int base_x, int base_y, int clut_x, int clut_y) {
    if ((u_TexWindow.x | u_TexWindow.y) != 0) {
        u = (u & ~(u_TexWindow.x * 8)) | ((u_TexWindow.z & u_TexWindow.x) * 8);
        v = (v & ~(u_TexWindow.y * 8)) | ((u_TexWindow.w & u_TexWindow.y) * 8);
    }
    u &= 255;
    v &= 255;
    if (depth == 0) { // 4-bit CLUT
        int px = vram_at(base_x + (u >> 2), base_y + v);
        int entry = (px >> ((u & 3) * 4)) & 0xF;
        return vram_at(clut_x + entry, clut_y);
    } else if (depth == 1) { // 8-bit CLUT
        int px = vram_at(base_x + (u >> 1), base_y + v);
        int entry = (px >> ((u & 1) * 8)) & 0xFF;
        return vram_at(clut_x + entry, clut_y);
    }
    // 15-bit direct
    return vram_at(base_x + u, base_y + v);
}

vec3 col16_to_rgb(int raw) {
    float r = float(raw & 31) / 31.0;
    float g = float((raw >> 5) & 31) / 31.0;
    float b = float((raw >> 10) & 31) / 31.0;
    return vec3(r, g, b);
}

void main()
{
    bool is_textured = (inFlags & 0x40u) != 0u;
    if (!is_textured) {
        outColor = vec4(inColor.rgb, 1.0);
        return;
    }

    uint texpage = inTPageClut.x;
    uint clut = inTPageClut.y;
    int base_x = int(texpage & 0xFu) * 64;
    int base_y = int((texpage >> 4u) & 1u) * 256;
    int depth = int(inFlags & 0x3u);
    int clut_x = int(clut & 0x3Fu) * 16;
    int clut_y = int((clut >> 6u) & 0x1FFu);

    // Clamping con epsilon para evitar desbordamiento por truncamiento negativo en bordes de polígonos
    int iu = clamp(int(floor(inUV.x + 1e-4)), 0, 255);
    int iv = clamp(int(floor(inUV.y + 1e-4)), 0, 255);
    int raw = fetch_texel(iu, iv, depth, base_x, base_y, clut_x, clut_y);

    // Transparencia estándar de PS1: Si el texel es 0, descartar fragmento
    if (raw == 0) {
        discard;
    }

    vec3 rgb = col16_to_rgb(raw);
    bool rawTex = (inFlags & 0x4u) != 0u;

    if (!rawTex) {
        // Modulación PS1: texCol.rgb * inColor.rgb * 2.0
        rgb = clamp(rgb * inColor.rgb * 2.0, 0.0, 1.0);
    }

    // Alpha siempre 1.0 para que el compositor de Switch NWindow no trate la imagen como transparente
    outColor = vec4(rgb, 1.0);
}
