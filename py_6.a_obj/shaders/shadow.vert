#version 460 core

layout (location = 1) in vec3 in_position;

uniform mat4 m_proj;
uniform mat4 m_view_light;
uniform mat4 m_model;

void main() {
    const vec4 in_position4 = vec4(in_position, 1.0);

    gl_Position = m_proj * m_view_light * m_model * in_position4;
}