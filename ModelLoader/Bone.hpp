//
//  Bone.hpp
//  ModelLoader
//
//  Created by Apple on 04/08/21.
//

#ifndef Bone_hpp
#define Bone_hpp

#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext/quaternion_relational.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include "assimp_glm_helper.h"

struct KeyPosition{
    glm::vec3 position;
    float timestamp;
};

struct KeyRotation{
    glm::quat orientation;
    float timestamp;
};

struct KeyScale{
    glm::vec3 scale;
    float timestamp;
};

class Bone{
public:
    // Read Keyframes from aiNodeAnim
    Bone(const std::string &name, int id, const aiNodeAnim *channel);
    /* Interpolates b/w positions,rotations & scaling keys based on the curren time of the
        animation and prepares the local transformation matrix by combining all keys tranformations */
    void Update(float animationTime);
    
    glm::mat4 GetLocalTransform() { return mLocalTransform; }
    std::string GetBoneName() const { return mName; }
    int GetBoneID() { return mId; }
    
    /* Gets the current index on mKeyPositions to interpolate to based on the current
        animation time */
    int GetPositionIndex(float animationTime);
    
    /* Gets the current index on mKeyRotations to interpolate to based on the current
        animation time */
    int GetRotationIndex(float animationTime);
    
    /* Gets the current index on mKeyScalings to interpolate to based on the current
        animation time */
    int GetScaleIndex(float animationTime);
private:
    std::vector<KeyPosition> mPositions;
    std::vector<KeyRotation> mRotations;
    std::vector<KeyScale> mScales;
    int mNumPositions;
    int mNumRotations;
    int mNumScalings;
    
    glm::mat4 mLocalTransform;
    std::string mName;
    int mId;
    
    // Functions
    /* Gets normalized value for Lerp & Slerp*/
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    
    /* figures out which position keys to interpolate b/w and performs the interpolation
        and returns the translation matrix */
    glm::mat4 InterpolatePosition(float animationTime);
    
    /* figures out which rotations keys to interpolate b/w and performs the interpolation
        and returns the rotation matrix */
    glm::mat4 InterpolateRotation(float animationTime);
    
    /* figures out which scaling keys to interpolate b/w and performs the interpolation
        and returns the scale matrix */
    glm::mat4 InterpolateScaling(float animationTime);
};
#endif /* Bone_hpp */
