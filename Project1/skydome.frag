#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec2 fragTexCoord;
out vec4 outColor;

uniform sampler2D textureSampler;

void main() {
    // Normalizujeme směr
    vec3 dir = normalize(fragWorldPos);
    
    // Pro dolní polovinu koule zobrazíme tmavou barvu
    if (dir.y < 0.0) {
        vec3 darkColor = vec3(0.01, 0.01, 0.02);
        outColor = vec4(darkColor, 1.0);
        return;
    }
    
    // Angular/Dome mapování pro horní polokouli
    // Střed textury (0.5, 0.5) = zenit (přímo nahoře)
    // Okraje textury (vzdálenost od středu = 0.5) = horizont
    
    // Úhel od zenitu (0 = zenit, PI/2 = horizont)
    float theta = acos(dir.y);
    
    // Úhel kolem osy Y (azimut)
    float phi = atan(dir.z, dir.x);
    
    // Radiální vzdálenost od středu textury (0 = střed/zenit, 1 = okraj/horizont)
    // Normalizujeme theta tak, aby PI/2 (horizont) -> 1.0
    float r = theta / (3.14159265359 / 2.0);
    
    // Převedeme polární souřadnice na kartézské UV
    float u = 0.5 + r * cos(phi) * 0.5;
    float v = 0.5 + r * sin(phi) * 0.5;
    
    // Vzorkujeme texturu
    vec3 color = texture(textureSampler, vec2(u, v)).rgb;
    
    // Ztmavíme pro noční atmosféru
    float nightFactor = 0.4; // 0.0 = černá, 1.0 = plný jas
    color *= nightFactor;
    
    // Přidáme lehký modrý nádech noční oblohy
    color += vec3(0.02, 0.02, 0.05);
    
    outColor = vec4(color, 1.0);
}
