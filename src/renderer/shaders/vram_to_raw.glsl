#version 460
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (binding = 0) uniform sampler2D s_srcVram;
layout (r16ui, binding = 0) uniform writeonly uimage2D u_dstRawVram;

layout (std140, binding = 0) uniform CopyParams {
    ivec4 u_Rect; // x = srcX, y = srcY, z = dstX, w = dstY (1x PS1 coords)
    ivec4 u_Size; // x = width, y = height, z = scale, w = unused
};

void main() {
    int lx = int(gl_GlobalInvocationID.x);
    int ly = int(gl_GlobalInvocationID.y);
    if (lx >= u_Size.x || ly >= u_Size.y) return;

    int scale = max(u_Size.z, 1);
    ivec2 srcCoord = ivec2((u_Rect.x + lx) * scale + (scale >> 1),
                           (u_Rect.y + ly) * scale + (scale >> 1));
    vec4 c = texelFetch(s_srcVram, srcCoord, 0);
    uint r5 = uint(clamp(int(round(c.r * 31.0)), 0, 31));
    uint g5 = uint(clamp(int(round(c.g * 31.0)), 0, 31));
    uint b5 = uint(clamp(int(round(c.b * 31.0)), 0, 31));
    uint raw = r5 | (g5 << 5u) | (b5 << 10u);
    imageStore(u_dstRawVram, ivec2(u_Rect.z + lx, u_Rect.w + ly), uvec4(raw, 0u, 0u, 0u));
}
