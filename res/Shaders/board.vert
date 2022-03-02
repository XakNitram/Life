#version 460 core
layout(location = 0) in vec4 position;
//layout(std430, binding=0) buffer cells { uint data[100]; };
out flat uint alive;

uniform mat4 projection = mat4(1.0);
uniform usamplerBuffer data;


void main() {
    gl_Position = projection * position;
    alive = texelFetch(data, gl_VertexID / 4).r;
}
