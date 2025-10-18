#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 outColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform int modelType; // 0=Constant, 1=Lambert, 2=Phong, 3=Blinn

void main() {
    vec3 objectColor = vec3(0.8, 0.8, 0.8);
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragWorldPos);
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (modelType == 0) {
        // Constant
        outColor = vec4(objectColor, 1.0);
        return;
    }
    if (modelType == 1) {
        // Lambert
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = diff * lightColor;
        outColor = vec4((ambient + diffuse) * objectColor, 1.0);
        return;
    }
    if (modelType == 2) {
        // Phong
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = diff * lightColor;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        specular = spec * lightColor;
        outColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
        return;
    }
    if (modelType == 3) {
        // Blinn-Phong
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = diff * lightColor;
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
        specular = spec * lightColor;
        outColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
        return;
    }
    // Default: constant
    outColor = vec4(objectColor, 1.0);
}
