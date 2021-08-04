//
//  Mesh.cpp
//  ModelLoader
//
//  Created by Apple on 03/08/21.
//

#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vertex>  vertices,
     std::vector<unsigned int> indices,
           std::vector<Texture> textures){
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    
    // Prepare mesh with the captured data to use for rendering
    setupMesh();
}

Mesh::~Mesh(){
    
}

void Mesh::setupMesh(){
    // Setup VAO, VBO, EBO
    // -- VAO
    glGenVertexArrays(1, &VAO);
    
    // -- VBO
    glGenBuffers(1, &VBO);
    
    // -- EBO
    glGenBuffers(1, &EBO);
    
    // Activate VAO, VBO, EBO to pass the data to VBO, EBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),  // Size of data to be passed
                 &vertices[0],                      // Actual data
                 GL_STATIC_DRAW
                 );
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int), // Size of data to be passed
                 &indices[0],                           // Actual data
                 GL_STATIC_DRAW
                 );
    
    // mapping of the passed data to the attributes of the shader
    // -- Vertex Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
                          0,                // Position of the vertex attribute
                          3,                // Number of values to take for each vertex
                          GL_FLOAT,         // Datatype of values
                          GL_FALSE,         // Normalization not necessary
                          sizeof(Vertex),   // Location of next vertex attribute data
                          (void *) 0       // Start position of the vertex attribute data
                          );
    
    // -- Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
                          1,                // Position of the vertex attribute
                          3,                // Number of values to take for each vertex
                          GL_FLOAT,         // Datatype of values
                          GL_FALSE,         // Normalization not necessary
                          sizeof(Vertex),   // Location of next vertex attribute data
                          (void *) offsetof(Vertex, normal)       // Start position of the vertex attribute data
                          );
    
    // -- Vertex Texture Coordinate
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
                          2,                // Position of the vertex attribute
                          2,                // Number of values to take for each vertex
                          GL_FLOAT,         // Datatype of values
                          GL_FALSE,         // Normalization not necessary
                          sizeof(Vertex),   // Location of next vertex attribute data
                          (void *) offsetof(Vertex, texCoords)       // Start position of the vertex attribute data
                          );
    
    // -- Bone Ids
    // Notice the usage of glVertexAttribIPointer for bone ids
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(
                          3,                // Position of the vertex attribute
                          4,                // Number of values to take for each vertex
                          GL_INT,           // Datatype of values
                          sizeof(Vertex),   // Location of next vertex attribute data
                          (void *) offsetof(Vertex, mBoneIds)       // Start position of the vertex attribute data
                          );
    
    // -- Weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
                          4,                // Position of the vertex attribute
                          4,                // Number of values to take for each vertex
                          GL_INT,           // Datatype of values
                          GL_FALSE,         // Normalization not necessary
                          sizeof(Vertex),   // Location of next vertex attribute data
                          (void *) offsetof(Vertex, mWeights)       // Start position of the vertex attribute data
                          );
    
    // Unbind
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(0);
}

void Mesh::draw(Shader &shader){
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    
    for(unsigned int i=0; i<textures.size(); i++){
        // Activate proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);
        
        // Retrieve texture number ( the N in texture_diffuseN) to calculate the uniform name
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse"){
            number = std::to_string(diffuseNr++);
        }else if(name == "texture_specular"){
            number = std::to_string(specularNr++);
        }
        
        shader.setFloat(("material."+name+number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    
    // Reset Active Texture
    glActiveTexture(GL_TEXTURE_2D);
    
    // Draw Mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
