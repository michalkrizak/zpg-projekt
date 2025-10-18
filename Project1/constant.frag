#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 outColor;

void main() {
    outColor = vec4(0.8, 0.2, 0.2, 1.0); // konstantní barva
}
