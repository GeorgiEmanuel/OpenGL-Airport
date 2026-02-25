#version 410 core

// Input vertex attributes
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

// Outputs to fragment shader
out vec3 fNormal;
out vec4 fPosEye;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;
out vec3 lightPosEye1;
out vec3 lightPosEye2;
out vec3 lightPosEye3;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;

uniform vec3 lightSource1;
uniform vec3 lightSource2;
uniform vec3 lightSource3;

void main()
{
 
    fPosEye = view * model * vec4(vPosition, 1.0f);

   
    fNormal = normalize(normalMatrix * vNormal);

   
    fTexCoords = vTexCoords;

   
    gl_Position = projection * fPosEye;

    
    lightPosEye1 = vec3(view * vec4(lightSource1, 1.0f));
    lightPosEye2 = vec3(view * vec4(lightSource2, 1.0f));
    lightPosEye3 = vec3(view * vec4(lightSource3, 1.0f));

    
    fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);
}
