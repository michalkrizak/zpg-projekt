#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoord;
out vec3 fragNormal;
out vec3 fragWorldPos;
out vec2 fragTexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;


uniform float w = 500.0; // Standardni hodnota. Zkuste v aplikaci nastavit na: 500.0


mat4 customTransform = mat4(
    1.0, 0.0, 0.0, 0.0,  // prvni sloupec
    0.0, 1.0, 0.0, 0.0,  // druhy sloupec
    0.0, 0.0, 1.0, 0.0,  // treti sloupec
    0.0, 0.0, 0.0, 20.0   // ctvrty sloupec - zkuste: 20.0 misto 1.0
);

void main(){

    vec4 vp_projektivniProstor = vec4(position * w, w);

    vec4 wp = model * vp_projektivniProstor;

    fragWorldPos = wp.xyz / wp.w;
    
    fragNormal = normalMatrix * normal;
    fragTexCoord = texCoord;

    gl_Position = projection * view * customTransform * wp;
}

