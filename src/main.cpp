#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include "shader/shader.h"
#include "cameras/camera.h"

std::shared_ptr<cameras::Camera> camera;
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
        camera->Truck(0.0f, 1.f);
    if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
        camera->Truck(0.0f, -1.f);
    if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
        camera->Truck(-1.0f, 0.f);
    if (glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
        camera->Truck(1.0f, 0.f);
}

int main()
{
    glfwInit();
    // tell glfw the version of opengl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Viewer", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // make the window the current context
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600); // set the viewport to the whole window, left lower and right upper corner coordinates
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // set the callback function for window resize

    // camera setup
    camera = std::make_shared<cameras::PerspectiveCamera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
    {
        camera->Rotate(xpos, ypos);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        camera->Dolly(yoffset);
    });


    {
        Shader shader;
        if (shader.Initialize("shader/vs.glsl", "shader/fs.glsl"))
        {
            while(!glfwWindowShouldClose(window))
            {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                
                processInput(window);
                shader.Render(camera);
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }

    }
    glfwTerminate();
    
    return 0;
}

