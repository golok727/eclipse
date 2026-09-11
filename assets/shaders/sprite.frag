#version 410 core

out vec4 outColor;

in vec3 vertexpos;
in vec2 uvs;

uniform sampler2D tex;
uniform vec3 color = vec3(1.0);

void main() {
    outColor = texture(tex, uvs) * vec4(color, 1.0);
}
