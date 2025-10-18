#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 outColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main() {
    vec3 ambient = 0.1 * lightColor;
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;
    vec3 result = (ambient + diffuse + specular) * objectColor;
    outColor = vec4(result, 1.0);
}
