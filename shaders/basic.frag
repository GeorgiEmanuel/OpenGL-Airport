#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec4 fPosEye;
in vec4 fragPosLightSpace;
in vec2 fTexCoords;
in vec3 lightPosEye1;
in vec3 lightPosEye2;
in vec3 lightPosEye3;

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
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.15f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

float reflectCoeff = 32.0f; 

uniform vec3 lightSource1;
uniform vec3 lightSourceColor1;
uniform vec3 lightSource2;
uniform vec3 lightSourceColor2;
uniform vec3 lightSource3;
uniform vec3 lightSourceColor3;

float c = 1.0;
float l = 0.022;
float q = 0.0019;

uniform int nightMode;

uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform vec3 spotLightColor;

void computeDirLight()
{
    vec3 normalEye = normalize(normalMatrix * fNormal);

    
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    
    vec3 viewDir = normalize(- fPosEye.xyz);

    
    ambient = ambientStrength * lightColor;
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), reflectCoeff);
    specular = specularStrength * specCoeff * lightColor;
}

void computeSourceLight(vec3 lightWorldCoord, vec3 lightColor)
{	
	vec3 lightPosEye = vec3(view * vec4(lightWorldCoord, 1.0));
	
	vec3 normalEye = normalize(normalMatrix * fNormal);
	vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
	vec3 viewDir = normalize(-fPosEye.xyz);
	
	vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);
	
	float diff = max(dot(normalEye, lightDirN), 0.0);
	
	vec3 halfVec = normalize(lightDirN + viewDir);
	float specCoeff = pow(max(dot(normalEye, halfVec), 0.0), reflectCoeff);
	
	float dist = length(lightPosEye - fPosEye.xyz);
	float att = 1.0 / (c + l * dist + q * dist * dist);
	
	ambient += att * ambientStrength * lightColor;
	diffuse += att * diff * lightColor;
	specular += att * specularStrength * specCoeff * lightColor;

}

float computeShadow()
{
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    if (normalizedCoords.z > 1.0f) 
    {
        return 0.0f;
    }
    
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = 0.005f;
    return currentDepth - bias > closestDepth ? 1.0f : 0.0f;

}

void computeSpotLight() {
    vec3 lightPosEye = vec3(view * vec4(spotLightPos, 1.0));
    vec3 lightDirEye = normalize(vec3(view * vec4(spotLightDir, 0.0)));

    vec3 normalEye = normalize(normalMatrix * fNormal);
    vec3 viewDir = normalize(- fPosEye.xyz);
    vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);
    
    float theta = dot(lightDirN, normalize(-lightDirEye));
    float cutOff = cos(radians(12.5));
    float outerCutOff = cos(radians(17.5));

    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    if (theta > outerCutOff) {
        float diff = max(dot(normalEye, lightDirN), 0.0);
        
        
        vec3 halfwayDir = normalize(lightDirN + viewDir);
        float shine = 256.0f; 
        float specCoeff = pow(max(dot(normalEye, halfwayDir), 0.0), shine);
        
        float dist = length(lightPosEye - fPosEye.xyz);
        float att = 1.0 / (c + l * dist + q * dist * dist);

        diffuse += att * diff * spotLightColor * intensity;
        specular += att * specularStrength * specCoeff * spotLightColor * intensity;
    }
}
float computeFog()
{
    float fogDensity = 0.002f;
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
    return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    
    ambient = vec3(0.0f);
    diffuse = vec3(0.0f);
    specular = vec3(0.0f);
    
   
    computeDirLight();

    if (nightMode == 0) {
        computeSourceLight(lightSource1, lightSourceColor1);
        computeSourceLight(lightSource2, lightSourceColor2);
        computeSourceLight(lightSource3, lightSourceColor3);
    }
    
    computeSpotLight();
    
    vec3 baseColor = texture(diffuseTexture, fTexCoords).rgb;
    vec3 specularColor = texture(specularTexture, fTexCoords).rgb;
    
   
    float shadow = computeShadow();
    
  
    vec3 lighting = ambient * baseColor;
    lighting += (1.0f - shadow) * diffuse * baseColor;
    lighting += (1.0f - shadow) * specular * specularColor;

    
    vec3 color = min(
    ambient * texture(diffuseTexture, fTexCoords).rgb +               // ambientala
    (1.0f - shadow) * diffuse * texture(diffuseTexture, fTexCoords).rgb + // difuza
    (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, // speculara
    1.0f);




    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    

    vec4 fogColorDay = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    vec4 fogColorNight = vec4(0.01f, 0.01f, 0.02f, 1.0f);

    vec4 finalColor;
    vec4 finalFogColor;
    if(nightMode == 1) {
        finalColor = vec4(color, 1.0f);
        finalFogColor = fogColorNight;
       
    } else {
        finalColor = vec4(color, 1.0f);
        finalFogColor = fogColorDay;
    }

    fColor = mix(finalFogColor, finalColor, fogFactor);
}
