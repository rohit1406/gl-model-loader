//
//  Animation.hpp
//  ModelLoader
//
//  Created by Apple on 04/08/21.
//

#ifndef Animation_hpp
#define Animation_hpp

#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <map>

#include "assimp_glm_helper.h"
#include "Model.hpp"
#include "Bone.hpp"

struct AssimpNodeData{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation{
public:
    Animation();
    Animation(const std::string &animationPath, Model *model);
    ~Animation();
    
    Bone* FindBone(const std::string &name);
    
    inline float GetTicksPerSecond() { return mTicksPerSecond; }

    inline float GetDuration() { return mDuration;}

    inline const AssimpNodeData& GetRootNode() { return mRootNode; }

    inline const std::map<std::string,BoneInfo>& GetBoneIDMap()
    {
        return mBoneInfoMap;
    }
    
private:
    // Properties
    float mDuration;
    int mTicksPerSecond;
    std::vector<Bone> mBones;
    AssimpNodeData mRootNode;
    std::map<std::string, BoneInfo> mBoneInfoMap;

    // Functions
    void ReadMissingBones(const aiAnimation* animation, Model& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};
#endif /* Animation_hpp */
