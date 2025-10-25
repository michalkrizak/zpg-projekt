#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 outColor;

uniform vec3 objectColor;

void main() {
    // Emissive object - glows with its own color
    outColor = vec4(objectColor, 1.0);
}
