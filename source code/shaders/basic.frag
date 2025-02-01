//basic.frag
#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform sampler2D specularTexture;
float shadowBias = 0.01; // pentru a preveni "shadow acne"

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
vec3 colorPoint = vec3(0.0f);

//fog
uniform float fogDensity;
uniform vec4 fogColor;

//auxiliar data
vec3 ambient1 = vec3(0.0f);
vec3 diffuse1 = vec3(0.0f);
vec3 specular1 = vec3(0.0f);


// uniform spotlight
uniform vec3 lampColor;
uniform vec3 position;

//spotlight
in vec4 lightPosEye ;

float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;
float attenuation;

uniform vec3 spotDir;         // direcția spotului (în world space sau eye space, vom vedea)
uniform float spotCutoff;     // cos(unghi interior)
uniform float spotOuterCutoff;// cos(unghi exterior)

float computeShadow(vec4 fragPosLS) {
    // 1. Convertim la NDC (divide by w)
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    // 2. Transform [(-1..1)] -> ([0..1])
    projCoords = projCoords * 0.5 + 0.5;

    // Dacă e in afara volumului (z > 1), nu e umbră
    if (projCoords.z > 1.0) {
        return 0.0;
    }

    // 3. Luăm adâncimea cea mai apropiată din shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 4. Adâncimea actuală a fragmentului în spațiul luminii
    float currentDepth = projCoords.z;

    // 5. Comparăm
    //    Dacă currentDepth > closestDepth + bias => umbră
    float shadow = 0.0;
    if (currentDepth - shadowBias > closestDepth) {
        shadow = 1.0;
    }

    return shadow;
}
float computeFog()
{

    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    float fragmentDistance = length(fPosEye.xyz);
 	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 	return clamp(fogFactor, 0.0f, 1.0f);
}

void calcSpotLight()
{
    // 1. Vector normal
    vec3 normalEye = normalize(fNormal);

    // 2. Poziția luminii (farul) în coordonate “eye” (dacă preferi).
    //    Avem deja 'position' = far location, să-l convertim:
    vec3 lightPosEye = (view * vec4(position, 1.0f)).xyz;

    // 3. Vectorul de la fragment la sursă
    vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);

    // 4. Vectorul direcției “farului” (spotDir), convertit în “eye space”
    vec3 spotDirEye = normalize( (view * vec4(spotDir, 0.0f)).xyz );

    // 5. Calculăm unghiul (theta) dintre direcția luminii și direcția spotului
    //   - atenție la semne: dacă spotDir e “în față”, vrem dot(lightDirN, -spotDirEye) etc.
    float theta = dot(lightDirN, -spotDirEye);

    // 6. Construim factorul de “spot”
    float epsilon = spotCutoff - spotOuterCutoff;
    float spotFactor = clamp((theta - spotOuterCutoff) / epsilon, 0.0, 1.0);

    // 7. Atenuarea cu distanța (exact ca în calcPointLight)
    float distance = length(lightPosEye - fPosEye.xyz);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // 8. Calcule de ambient, diffuse, specular (similar calcPointLight)
    vec3 ambientp = attenuation * ambientStrength * lampColor;
    vec3 diffusep = attenuation * max(dot(normalEye, lightDirN), 0.0f) * lampColor;

    vec3 viewDirN = normalize(-fPosEye.xyz);
    // BLINN-PHONG
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
    vec3 specularp = attenuation * specularStrength * specCoeff * lampColor;

    // 9. Aplica factorul de spot = spotFactor
    ambientp  *= spotFactor;
    diffusep  *= spotFactor;
    specularp *= spotFactor;

    // 10. Aplica texturi
    ambientp  *= texture(diffuseTexture, fTexCoords).rgb;
    diffusep  *= texture(diffuseTexture, fTexCoords).rgb;
    specularp *= texture(specularTexture, fTexCoords).rgb;

    // 11. Adăugăm la valorile globale
    ambient1  += ambientp;
    diffuse1  += diffusep;
    specular1 += specularp;
}

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = normalize((view * vec4(lightDir, 0.0f)).xyz);

    //compute view direction (in eye coordinates, the viewer is situated at the origin)
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    // BLINN-PHONG
    vec3 halfDir = normalize(lightDirN + viewDir);
    float specCoeff = pow(max(dot(normalEye, halfDir), 0.0), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

void main()
{
    computeDirLight();
    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;

    // 3. calcSpotLight() - noua logică cu unghi
    calcSpotLight();

    ambient  += ambient1;
    diffuse  += diffuse1;
    specular += specular1;

    float shadowFactor = computeShadow(fragPosLightSpace);

    // 2. Reducem componentele diffuse și specular în zonele de umbră
    //    (Nu afectăm ambient, de obicei)
    diffuse *= (1.0 - shadowFactor);
    specular *= (1.0 - shadowFactor);

    vec3 color = min((ambient + diffuse) + specular, 1.0f);

    // Fog
    float fogFactor = computeFog();
    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);

}
