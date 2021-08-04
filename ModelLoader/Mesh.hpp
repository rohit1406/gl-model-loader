//
//  Mesh.hpp
//  ModelLoader
//
//  Created by Apple on 03/08/21.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "Shader.hpp"

#define MAX_BONE_INFLUENCE 4

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    //glm::vec3 tangent;
    //glm::vec3 bitangent;
    
    // Bone indices which will influence this vertex
    int mBoneIds[MAX_BONE_INFLUENCE];
    
    // Weight from each bone
    float mWeights[MAX_BONE_INFLUENCE];
};

struct Texture{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh{
public:
    // Properties
    // -- Mesh data
    std::vector<Vertex>  vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    
    // Behaviors
    // -- Constructors and Destructors
    Mesh(std::vector<Vertex>  vertices,
         std::vector<unsigned int> indices,
         std::vector<Texture> textures);
    ~Mesh();
    
    // -- Render Functions
    void draw(Shader &shader);
    
private:
    // Properties
    // -- Render data
    unsigned int VAO, VBO, EBO;
    
    // Behaviors
    void setupMesh();
};
#endif /* Mesh_hpp */
