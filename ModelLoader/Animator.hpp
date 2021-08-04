//
//  Animator.hpp
//  ModelLoader
//
//  Created by Apple on 04/08/21.
//

#ifndef Animator_hpp
#define Animator_hpp

#include <stdio.h>

#include "Animation.hpp"

class Animator{
public:
    // Functions
    Animator(Animation* Animation);
    void UpdateAnimation(float dt);
    void PlayAnimation(Animation* pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
    
    // -- Getters
    std::vector<glm::mat4> GetFinalBoneMatrices()
        {
            return mFinalBoneMatrices;
        }
    
private:
    std::vector<glm::mat4> mFinalBoneMatrices;
        Animation* mCurrentAnimation;
        float mCurrentTime;
        float mDeltaTime;
};
#endif /* Animator_hpp */
