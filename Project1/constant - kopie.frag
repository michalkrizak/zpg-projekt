#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec2 fragTexCoord;
out vec4 outColor;

uniform vec3 objectColor;
uniform int useTexture;
uniform sampler2D textureSampler;

void main() {
    vec3 color;
    if (useTexture == 1) {
        color = texture(textureSampler, fragTexCoord).rgb;
    } else {
        color = objectColor;
    }
    outColor = vec4(color, 1.0);
}
