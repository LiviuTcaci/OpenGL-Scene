#version 410 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;

out vec4 outColor;

uniform sampler2D waterTexture;
uniform float time;
uniform vec3  waterColor;
uniform bool  setNight;

void main()
{
    vec3 normal = normalize(fragNormal);

    // 1. Ia culoarea texturii
    vec3 texColor = texture(waterTexture, fragTexCoords).rgb;

    // 2. Mai redu puțin intensitatea
    texColor *= 0.2;

    // 3. Un pic de variație
    float variation = 0.02 * sin(time * 0.7 + fragPos.x + fragPos.z);
    texColor += variation;

    // 4. Creezi colCombined (să-l poți modifica dacă e noapte)
    vec3 colCombined = texColor;

    if (setNight) {
        // Reduci puternic luminozitatea
        colCombined *= 0.1;
    }

    // 5. Transparența apei
    float alpha = 0.8;
    outColor = vec4(colCombined, alpha);
}