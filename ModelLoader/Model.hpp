//
//  Model.hpp
//  ModelLoader
//
//  Created by Apple on 03/08/21.
//

#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <vector>

#include "Shader.hpp"
#include "Mesh.hpp"
#include "assimp_glm_helper.h"

struct BoneInfo{
    int id;             // Index in finalBoneMatrices
    glm::mat4 offset;   // offset matrix transforms vertex from model space to bone space
};

class Model{
public:
    // Properties
    bool gammaCorrection;
    
    // Functions
    // -- Constructors and Destructor
    Model(char *path, bool gamma = false);
    ~Model();
    
    // -- Getter Functions
    std::map<std::string, BoneInfo> GetBoneInfoMap(){ return mBoneInfoMap;}
    int GetBoneCount(){return mBoneCounter;}
    
    // -- Render Functions
    void draw(Shader &shader);
    
private:
    // Properties
    // -- Model data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    
    // -- Animation data
    std::map<std::string, BoneInfo> mBoneInfoMap;
    int mBoneCounter=0;
    
    // Functions
    
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    unsigned int textureFromFile(std::string fileName, std::string &directory, bool gamma=false);
    
    // -- Animation functions
    void setVertexBoneDataToDefault(Vertex &vertex);
    void setVertexBoneData(Vertex &vertex, int boneId, float weight);
    void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene);
};
#endif /* Model_hpp */
