#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoords;

out vec3 vertexpos;
out vec2 uvs;

uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);
uniform vec2 uvOffset = vec2(0.0);
uniform vec2 uvScale = vec2(1.0);

void main() {
    uvs = texcoords * uvScale + uvOffset;
    vertexpos = position;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
