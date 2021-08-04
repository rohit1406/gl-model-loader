#version 330 core       // Represents OpenGL version 4.3

// In Attributes
layout(location = 0) in vec3 aPos;  // Vertex Position
layout(location = 1) in vec3 aNorm; // Normal Position
layout(location = 2) in vec2 aTexCoords;    // Texture Coordinate
layout(location = 3) in ivec4 boneIds;      // Bone Ids
layout(location = 4) in vec4 weights;       // Weights of the muscles

// Uniforms
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

// Out Parameters
out vec2 TexCoords;

void main(){
    vec4 totalPosition = vec4(0.0f);
    for(int i=0; i<MAX_BONE_INFLUENCE; i++){
        if(boneIds[i] == -1){
            continue;
        }
        
        if(boneIds[i] >= MAX_BONES){
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNorm;
    }
    
    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * totalPosition;
    TexCoords = aTexCoords;
}
