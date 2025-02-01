#version 410 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;   // dacă ai normal
layout(location = 2) in vec2 vTexCoords;// dacă ai coordonate de textură

out vec3 fragPos;        // poziția finală a vertexului în world space (dacă avem nevoie)
out vec3 fragNormal;     // normalul final
out vec2 fragTexCoords;  // coordonatele de textură (opțional)

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// uniform pentru timp, folosit la animarea valurilor
uniform float time;

// Parametru pentru amplitudinea valurilor
uniform float waveAmplitude;
// Frecvența valurilor
uniform float waveFrequency;

void main()
{
    // 1. Pornim de la poziția vertexului original
    vec3 position = vPosition;

    // 2. Aplicați un mic offset sinusoidal pe axa Y, pentru efect simplu de val
    //    Poți combina x și z în formula, dacă vrei tip “val 2D”.
    position.y += sin((vPosition.x + vPosition.z) * waveFrequency + time) * waveAmplitude;

    // 3. Calcule finale pentru MVP
    //    Poziția in world space (dacă vrei să o folosești pentru lumini)
    vec4 worldPos = model * vec4(position, 1.0);

    // 4. Output către pipeline
    fragPos = worldPos.xyz;

    // 5. Normal. Dacă vrei ceva mai avansat, ai putea recalcula normalul
    //    local pe baza derivative, dar pentru un efect de bază îl păstrăm
    fragNormal = mat3(transpose(inverse(model))) * vNormal;

    // 6. Coordonate textură, pasate direct (dacă există)
    fragTexCoords = vTexCoords;

    gl_Position = projection * view * worldPos;
}