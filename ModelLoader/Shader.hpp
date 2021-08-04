//
//  Shader.hpp
//  Breakout
//
//  Created by Apple on 28/06/21.
//

#ifndef Shader_hpp
#define Shader_hpp

#include <stdio.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Logger.h"

class Shader{
public:
    // States
    unsigned int ID;
    
    // Functions
    // -- Constructor and Destructor
    Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
    ~Shader();
    
    // -- Others
    Shader &use();
    void compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
    
    // -- Utilities
    void setFloat(const char* name, float value, bool useShader = false);
    void setInteger(const char* name, int value, bool useShader = false);
    void setVector2f(const char* name, float x, float y, bool useShader = false);
    void setVector2f(const char* name, const glm::vec2 &value, bool useShader = false);
    void setVector3f(const char* name, float x, float y, float z, bool useShader = false);
    void setVector3f(const char* name, const glm::vec3 &value, bool useShader = false);
    void setVector4f(const char* name, float x, float y, float z, float w, bool useShader = false);
    void setVector4f(const char* name, const glm::vec4 &value, bool useShader = false);
    void setMatrix4(const char* name, const glm::mat4 &matrix, bool useShader = false);
    
private:
    std::string readFile(const char *fileLocation);
    void checkCompileErrors(unsigned int object, std::string type);
};
#endif /* Shader_hpp */
