//
//  Shader.cpp
//  Breakout
//
//  Created by Apple on 28/06/21.
//

#include "Shader.hpp"

// -- Constructor and Destructor
Shader::Shader(const char* vertexLocation, const char* fragmentLocation, const char* geometryLocation){
    LOGGER("Creating shaders from "+std::string(vertexLocation)+", "+std::string(fragmentLocation));
    std::string vertexString = readFile(vertexLocation);
    std::string fragmentString = readFile(fragmentLocation);
    
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();
    
    //compileShader(vertexCode, fragmentCode);
    compile(vertexCode, fragmentCode);
}
Shader::~Shader(){
    
}

// -- Others
Shader& Shader::use(){
    glUseProgram(this->ID);
    return *this;
}

void Shader::compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource){
    unsigned int sVertex, sFragment, sGeometry;
    
    // SHADER CREATION
    // --VERTEX SHADER
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    this->checkCompileErrors(sVertex, "VERTEX");
    
    // --FRAGMENT SHADER
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    this->checkCompileErrors(sFragment, "FRAGMENT");
    
    // --GEOMETRY SHADER
    // Check if Geometry Shader source code is given
    if(geometrySource != nullptr){
        sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(sGeometry, 1, &geometrySource, NULL);
        glCompileShader(sGeometry);
        this->checkCompileErrors(sGeometry, "GEOMETRY");
    }
    
    // SHADER PROGRAM CREATION
    this->ID = glCreateProgram();
    
    // attach the shaders, sequence doesn't matter while attaching
    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    if(geometrySource != nullptr){
        glAttachShader(this->ID, sGeometry);
    }
    
    // link the shaders now, this will link the above attached shaders as per the OpenGL Pipeline
    glLinkProgram(this->ID);
    this->checkCompileErrors(this->ID, "PROGRAM");
    
    // delete the shader now as those are linked to our program object and no longer required
    glDeleteShader(sFragment);
    glDeleteShader(sVertex);
    if(geometrySource != nullptr){
        glDeleteShader(sGeometry);
    }
}

// -- Utilities
void Shader::setFloat(const char* name, float value, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform1f(uniformLocation, value);
}
void Shader::setInteger(const char* name, int value, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform1i(uniformLocation, value);
}
void Shader::setVector2f(const char* name, float x, float y, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform2f(uniformLocation, x, y);
}
void Shader::setVector2f(const char* name, const glm::vec2 &value, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform2f(uniformLocation, value.x, value.y);
}
void Shader::setVector3f(const char* name, float x, float y, float z, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform3f(uniformLocation, x, y, z);
}
void Shader::setVector3f(const char* name, const glm::vec3 &value, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform3f(uniformLocation, value.x, value.y, value.z);
}
void Shader::setVector4f(const char* name, float x, float y, float z, float w, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform4f(uniformLocation, x, y, z, w);
}
void Shader::setVector4f(const char* name, const glm::vec4 &value, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);
}
void Shader::setMatrix4(const char* name, const glm::mat4 &matrix, bool useShader){
    if(useShader){
        this->use();
    }
    GLuint uniformLocation = glGetUniformLocation(this->ID, name);
    glUniformMatrix4fv(uniformLocation,
                       1,                       // Count
                       false,                   // Transpose
                       glm::value_ptr(matrix)   // matrix is reference to glm::mat4, value_ptr returns address of the data in glm::mat4
                       );
}

void Shader::checkCompileErrors(unsigned int object, std::string type){
    int success;
    char infoLog[1024];
    // check shaders
    if(type != "PROGRAM"){
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(object,
                               1024,        // size of buffer
                               NULL,        // Length of string returned in infoLog
                               infoLog      // actual buffer in which logs are stored
                               );
            LOGGER("ERROR::SHADER:: Compile time error. Type - "+type+"\n"+infoLog);
        }
    }else{  // checks program object
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(object,
                                1024,        // size of buffer
                                NULL,        // Length of string returned in infoLog
                                infoLog      // actual buffer in which logs are stored
                                );
            LOGGER("ERROR::SHADER:: Link time error. Type - "+type+"\n"+infoLog);
        }
    }
}

std::string Shader::readFile(const char *fileLocation){
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);
    
    if(!fileStream.is_open()){
        LOGGER("Failed to read file "+std::string(fileLocation)+"! File doesn't exists.");
        return "";
    }
    
    std::string line = "";
    while(!fileStream.eof()){
        std::getline(fileStream, line);
        content.append(line+"\n");
    }
    
    fileStream.close();
    
    return content;
}
