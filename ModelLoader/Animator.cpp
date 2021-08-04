//
//  Animator.cpp
//  ModelLoader
//
//  Created by Apple on 04/08/21.
//

#include "Animator.hpp"

Animator::Animator(Animation* animation){
    mCurrentTime = 0.0f;
    mCurrentAnimation = animation;
    
    mFinalBoneMatrices.reserve(100);
    
    for(int i=0; i<100; i++){
        mFinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::UpdateAnimation(float dt){
    mDeltaTime = dt;
    if(mCurrentAnimation){
        mCurrentTime += mCurrentAnimation->GetTicksPerSecond() * dt;
        mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->GetDuration());
        CalculateBoneTransform(&mCurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation* pAnimation){
    mCurrentAnimation = pAnimation;
    mCurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform){
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    
    Bone *bone = mCurrentAnimation->FindBone(nodeName);
    
    if(bone){
        bone->Update(mCurrentTime);
        nodeTransform = bone->GetLocalTransform();
    }
    
    glm::mat4 globalTransform = parentTransform * nodeTransform;
    
    auto boneInfoMap = mCurrentAnimation->GetBoneIDMap();
    if(boneInfoMap.find(nodeName) != boneInfoMap.end()){
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        mFinalBoneMatrices[index] = globalTransform * offset;
    }
    
    for(int i=0; i<node->childrenCount; i++){
        CalculateBoneTransform(&node->children[i], globalTransform);
    }
}
