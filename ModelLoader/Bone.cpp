//
//  Bone.cpp
//  ModelLoader
//
//  Created by Apple on 04/08/21.
//

#include "Bone.hpp"

Bone::Bone(const std::string &name, int id, const aiNodeAnim *channel)
        : mName(name), mId(id), mLocalTransform(1.0f){
            // Positions Data
            mNumPositions = channel->mNumPositionKeys;
            for(int i=0; i<mNumPositions; i++){
                aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
                float timestamp = channel->mPositionKeys[i].mTime;
                
                KeyPosition data;
                data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
                data.timestamp = timestamp;
                mPositions.push_back(data);
            }
            
            // Rotation Data
            mNumRotations = channel->mNumRotationKeys;
            for(int i=0; i<mNumRotations; i++){
                aiQuaternion aiOrientation = channel->mRotationKeys[i].mValue;
                float timestamp = channel->mRotationKeys[i].mTime;
                
                KeyRotation data;
                data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
                data.timestamp = timestamp;
                mRotations.push_back(data);
            }
            
            // Scaling Data
            mNumScalings = channel->mNumScalingKeys;
            for(int i=0; i<mNumScalings; i++){
                aiVector3D aiScaling = channel->mScalingKeys[i].mValue;
                float timestamp = channel->mScalingKeys[i].mTime;
                
                KeyScale data;
                data.scale = AssimpGLMHelpers::GetGLMVec(aiScaling);
                data.timestamp = timestamp;
                mScales.push_back(data);
            }
}

/* Interpolates b/w positions,rotations & scaling keys based on the curren time of the
    animation and prepares the local transformation matrix by combining all keys tranformations */
void Bone::Update(float animationTime){
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    mLocalTransform = translation * rotation * scale;
}

/* Gets the current index on mKeyPositions to interpolate to based on the current
    animation time */
int Bone::GetPositionIndex(float animationTime){
    for(int i=0; i<mNumPositions-1; i++){
        if(animationTime < mPositions[i+1].timestamp){
            return i;
        }
    }
    assert(0);
    return 0;
}

/* Gets the current index on mKeyRotations to interpolate to based on the current
    animation time */
int Bone::GetRotationIndex(float animationTime){
    for(int i=0; i<mNumRotations-1; i++){
        if(animationTime < mRotations[i+1].timestamp){
            return i;
        }
    }
    assert(0);
    return 0;
}

/* Gets the current index on mKeyScalings to interpolate to based on the current
    animation time */
int Bone::GetScaleIndex(float animationTime){
    for(int i=0; i<mNumScalings-1; i++){
        if(animationTime < mScales[i+1].timestamp){
            return i;
        }
    }
    assert(0);
    return 0;
}


/* Gets normalized value for Lerp & Slerp*/
float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime){
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

/* figures out which position keys to interpolate b/w and performs the interpolation
    and returns the translation matrix */
glm::mat4 Bone::InterpolatePosition(float animationTime){
    if(mNumPositions == 1){
        return glm::translate(glm::mat4(1.0f), mPositions[0].position);
    }
    
    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(mPositions[p0Index].timestamp, mPositions[p1Index].timestamp, animationTime);
    
    glm::vec3 finalPosition= glm::mix(mPositions[p0Index].position, mPositions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

/* figures out which rotations keys to interpolate b/w and performs the interpolation
    and returns the rotation matrix */
glm::mat4 Bone::InterpolateRotation(float animationTime){
    if(mNumRotations == 1){
        auto rotation = glm::normalize(mRotations[0].orientation);
        return glm::mat4(rotation);     // TODO: Check
    }
    
    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(mRotations[p0Index].timestamp, mRotations[p1Index].timestamp, animationTime);
    
    glm::quat finalRotation= glm::slerp(mRotations[p0Index].orientation, mRotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::mat4(finalRotation);    // TODO: Check
}

/* figures out which scaling keys to interpolate b/w and performs the interpolation
    and returns the scale matrix */
glm::mat4 Bone::InterpolateScaling(float animationTime){
    if(mNumScalings == 1){
        return glm::scale(glm::mat4(1.0f), mScales[0].scale);
    }
    
    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(mScales[p0Index].timestamp, mScales[p1Index].timestamp, animationTime);
    
    glm::vec3 finalScale= glm::mix(mScales[p0Index].scale, mScales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}
