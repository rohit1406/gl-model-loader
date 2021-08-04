//
//  Animation.cpp
//  ModelLoader
//
//  Created by Apple on 04/08/21.
//

#include "Animation.hpp"

Animation::Animation(){
    
}

Animation::Animation(const std::string &animationPath, Model *model){
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    mDuration = animation->mDuration;
    mTicksPerSecond = animation->mTicksPerSecond;
    ReadHeirarchyData(mRootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
}

Animation::~Animation(){
    
}

Bone* Animation::FindBone(const std::string &name){
    auto iter = std::find_if(mBones.begin(), mBones.end(), [&](const Bone& bone){
        return bone.GetBoneName() == name;
    });
    
    if(iter == mBones.end()){
        return nullptr;
    }else{
        return &(*iter);
    }
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model){
    int size = animation->mNumChannels;
    
    // TODO: Check
    auto boneInfoMap = model.GetBoneInfoMap();
    int boneCount = model.GetBoneCount();
    
    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;
        if(boneInfoMap.find(boneName) == boneInfoMap.end()){
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        mBones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
    }
    mBoneInfoMap = boneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src){
    assert(src);
    
    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;
    
    for(int i=0; i<src->mNumChildren; i++){
        AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
