#version 330 core
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec2 fragTexCoord;
out vec4 outColor;

#define MAX_LIGHTS 16

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

void main() {
    vec3 objectColor = vec3(0.2, 0.8, 0.2);
    
    vec3 ra = objectColor;
    vec3 rd = objectColor; 
    vec3 rs = vec3(1.0);     
    float h = 1.0;      

    vec3 Ia = vec3(0.01);   
    vec3 I_ambient = Ia * ra; 

    // Ambient
    vec3 I = I_ambient;

    vec3 n = normalize(fragNormal);
    vec3 c = normalize(viewPos - fragWorldPos);

    for(int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        
        vec3 l = normalize(lights[i].position - fragWorldPos);
        
        float d = length(lights[i].position - fragWorldPos);
        float kc = 1.0;  // k_c (constant)
        float kl = 0.14; // k_l (linear)
        float kq = 0.07; // k_q (quadratic)
        
        float attenuation = 1.0 / (kc + kl * d + kq * d * d);


        vec3 Id_source = lights[i].color * attenuation; 
        vec3 Is_source = lights[i].color * attenuation;

        // Diffuze
        // Id * rd * max(0, n . l)
        float dot_n_l = max(dot(n, l), 0.0);
        
        vec3 I_diffuse = Id_source * rd * dot_n_l;
        
        // Specular
        vec3 r = reflect(-l, n); 
        
        // Is * rs * max(0, r . c)^s
        float dot_r_c = max(dot(r, c), 0.0);
        vec3 I_specular = Is_source * rs * pow(dot_r_c, h);

        I += I_diffuse + I_specular;
    }
    
    outColor = vec4(I, 1.0);
}