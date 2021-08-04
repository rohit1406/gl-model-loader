//
//  main.cpp
//  Breakout
//
//  Created by Apple on 28/06/21.
//

#include <iostream>
#include <GL/glew.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Camera.hpp"
#include "Model.hpp"
#include "Animator.hpp"
#include "Animation.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// GLFW Callback functions declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
// Deltatime Variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, const char * argv[]) {
    LOGGER("Starting application");
    //Initialize GLFW
    if(!glfwInit()){
        LOGGER("Failed to initialise GLFW! Terminating GLFW.");
        glfwTerminate();
        return 1;
    }
    LOGGER("GLFW initialised.");
    
    // OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);
    LOGGER("OpenGL properties set with OpenGL version 3.3");
    
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    if(!window){
        LOGGER("Failed to create GLFW Window! Terminating GLFW.");
        glfwTerminate();
        return 1;
    }
    LOGGER("Main Window Created.");
    glfwMakeContextCurrent(window);
    
    //glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // GLEW: load all OpenGL function pointers
    if(glewInit() != GLEW_OK){
        LOGGER("Failed to initialise GLEW!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    LOGGER("GLEW Initialised.");
    
    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    LOGGER("Viewport set to "+std::to_string(SCREEN_WIDTH)+"*"+std::to_string(SCREEN_HEIGHT));
    //glfwSetWindowUserPointer(window, this);
    LOGGER("Window Initialisation Completed.");
    
    Shader ourShader("resources/shaders/model_loading.vs", "resources/shaders/model_loading.fs");
    Model ourModel("resources/models/backpack/backpack.obj");
    
    // Animation data
    Shader animationShader("resources/shaders/animation.vs", "resources/shaders/animation.fs");
    Model animatedModel("resources/models/vampire/dancing_vampire.dae");
    Animation danceAnimation("resources/models/vampire/dancing_vampire.dae", &animatedModel);
    Animator animator(&danceAnimation);
    
    while(!glfwWindowShouldClose(window)){
        // Calculat Delta time
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        // input
        // -----
        processInput(window);
        animator.UpdateAnimation(deltaTime);
        
        // Render
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        animationShader.use();
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        animationShader.setMatrix4("projection", projection);
        animationShader.setMatrix4("view", view);
        
        auto transforms = animator.GetFinalBoneMatrices();
        for(int i=0; i<transforms.size(); i++){
            std::string str = std::to_string(i);
            std::string uniformTransformation = std::string("finalBonesMatrices[")+ str +std::string("]");
            animationShader.setMatrix4(uniformTransformation.c_str(), transforms[i]);
        }
        
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
        animationShader.setMatrix4("model", model);
        animatedModel.draw(animationShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    
    glfwTerminate();
    LOGGER("Good Bye!");
    LOGGER("------------------------------------------------------");
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
    // When the user presses escape key, we set the window should close property to true, closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
