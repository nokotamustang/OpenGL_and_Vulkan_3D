#version 460 core

out vec4 fragColor;

in vec4 clipCoords;

uniform samplerCube u_cube_map;
uniform mat4 m_invProjView;

void main() {
    const vec4 worldCoords = m_invProjView * clipCoords;
    const vec3 texCubeCoord = normalize(worldCoords.xyz / worldCoords.w);
    fragColor = texture(u_cube_map, texCubeCoord);
}