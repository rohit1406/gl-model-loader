//
//  Model.cpp
//  ModelLoader
//
//  Created by Apple on 03/08/21.
//

#include "Model.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Model::Model(char *path, bool gamma): gammaCorrection(gamma){
    
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);
    loadModel(path);
}

Model::~Model(){
    
}

void Model::draw(Shader &shader){
    for(unsigned int i=0; i<meshes.size(); i++){
        meshes[i].draw(shader);
    }
}

void Model::loadModel(std::string path){
    LOGGER("Loading model: "+path);
    // Load all the mesh data using assimp importer
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        const char* errorStr = importer.GetErrorString();
        LOGGER("ERROR::ASSIMP:: Failed to load model: "+std::string(errorStr));
    }
    
    // Extract the model directory which we will need later while loading texture
    directory = path.substr(0, path.find_last_of('/'));
    
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene){
    // Process all the node's meshes
    for(unsigned int i=0; i< node->mNumMeshes; i++){
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    
    // Then do the same for it's children
    for(unsigned int i=0; i< node->mNumChildren; i++){
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        Vertex vertex;
        setVertexBoneDataToDefault(vertex);
        // Process vertex position, normal, texcoords, etc
        // -- Positions
        glm::vec3 temp;
        temp.x = mesh->mVertices[i].x;
        temp.y = mesh->mVertices[i].y;
        temp.z = mesh->mVertices[i].z;
        vertex.position = temp;
        
        // -- Normals
        temp.x = mesh->mNormals[i].x;
        temp.y = mesh->mNormals[i].y;
        temp.z = mesh->mNormals[i].z;
        vertex.normal = temp;
        
        // -- Texture Coordinates
        if(mesh->mTextureCoords[0]){
            glm::vec2 vec;
            // Assimp allows a model to have up to 8 different texture coordinates per vertex. We're not going to use 8, we only care about the first set of texture coordinates.
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            
        }else{
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        
        /*
        // tangent
        if(mesh->mTangents){
            temp.x = mesh->mTangents[i].x;
            temp.y = mesh->mTangents[i].y;
            temp.z = mesh->mTangents[i].z;
            vertex.tangent = temp;
        }
        // bitangent
        if(mesh->mBitangents){
            temp.x = mesh->mBitangents[i].x;
            temp.y = mesh->mBitangents[i].y;
            temp.z = mesh->mBitangents[i].z;
            vertex.bitangent = temp;
        }*/
        
        // Add created vertex to the list
        vertices.push_back(vertex);
    }
    
    // Process Indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    // Process Materials
    if(mesh->mMaterialIndex >= 0){
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        
        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material,
                                            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
    }
    
    extractBoneWeightForVertices(vertices, mesh, scene);
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName){
    std::vector<Texture> textures;
    for(unsigned int i=0; i< mat->GetTextureCount(type); i++){
        aiString path;
        mat->GetTexture(type, i, &path);
        
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), path.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        
        if(!skip){
            Texture texture;
            texture.id = textureFromFile(path.C_Str(), directory);
            texture.type = typeName;
            texture.path = path.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::textureFromFile(std::string filename, std::string &directory, bool gamma){
    filename = directory + '/' + filename;
    LOGGER("Loading Texture "+filename);
    unsigned int textureId;
    glGenTextures(1, &textureId);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureId);
       glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
       glGenerateMipmap(GL_TEXTURE_2D);

       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       stbi_image_free(data);
    }else{
        LOGGER("ERROR::IMAGELOADING:: "+filename);
        stbi_image_free(data);
    }
    return textureId;
}

void Model::setVertexBoneDataToDefault(Vertex &vertex){
    for(unsigned int i=0; i<MAX_BONE_INFLUENCE; i++){
        vertex.mBoneIds[i] = -1;
        vertex.mWeights[i] = 0.0f;
    }
}

void Model::extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene){
    for(int boneIndex=0; boneIndex < mesh->mNumBones; boneIndex++){
        int boneId = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if(mBoneInfoMap.find(boneName) == mBoneInfoMap.end()){
            BoneInfo newBoneInfo;
            newBoneInfo.id = mBoneCounter;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            mBoneInfoMap[boneName] = newBoneInfo;
            boneId = mBoneCounter;
            mBoneCounter++;
        }else{
            boneId = mBoneInfoMap[boneName].id;
        }
        
        assert(boneId != -1);
        
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeight = mesh->mBones[boneIndex]->mNumWeights;
        for(int weightIndex = 0; weightIndex<numWeight; weightIndex++){
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            setVertexBoneData(vertices[vertexId], boneId, weight);
        }
    }
}

void Model::setVertexBoneData(Vertex &vertex, int boneId, float weight){
    for(int i=0; i<MAX_BONE_INFLUENCE; i++){
        if(vertex.mBoneIds[i] < 0){
            vertex.mWeights[i] = weight;
            vertex.mBoneIds[i] = boneId;
            break;
        }
    }
}
