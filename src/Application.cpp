#include <glew.h>
#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "GLErrorHandler.h"

#include "Shader.h"

#include "Renderer.h"
#include "Texture.h"

#include "glm/glm.hpp"

#include "gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"

#include "Mesh.h"

bool tabWasPressed = false;
bool isFocused = false;

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float deltaTime);

std::array<Vertex, 4> createTetraheadron(float x, float y, float z);
std::array<Vertex, 8> createCube(float x, float y, float z);

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 90.0f;

int main(void)
{
    GLFWwindow *window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    const char *glsl_version = "#version 130";
    int width = 800, height = 600;
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));
    GLCall(glEnable(GL_DEPTH_TEST));
    {

        std::vector<unsigned int> indices = {
            // Tetrahedron
            0, 1, 3,
            1, 2, 3,
            2, 0, 3,

            // Cube
            // Front
            4, 5, 6,
            6, 7, 4,
            // Top
            7, 6, 10,
            10, 11, 7,
            // Back
            8, 9, 10,
            10, 11, 8,
            // Bottom
            4, 5, 9,
            9, 8, 4,
            // Left
            4, 7, 11,
            11, 8, 4,
            // Right
            5, 6, 10,
            10, 9, 5
        };

        Shader shader("C:\\Users\\User\\Desktop\\VisualStudio\\C++\\OpenGL\\res\\shaders\\Basic.shader");
        shader.Bind();
        // shader.SetUniform4f("u_Color", 0.3f, 1.0f, 0.6f, 1.0f);

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();

        // test::Test* currentTest = nullptr;
        // test::TestMenu* testMenu = new test::TestMenu(currentTest);
        // currentTest = testMenu;

        // testMenu->RegisterTest<test::TestClearColor>("Clear Color");
        // testMenu->RegisterTest<test::TestTexture2D>("2D Texture");

        // (1/ (float)glfwGetTime()) *

        // /*  */
        // // Camara
        // //Position
        // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        // //Direction
        // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        // //Right axis (x)
        // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        // //Up axis (y)
        // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
        // // glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
        // //                              glm::vec3(0.0f, 0.0f, 0.0f),

        // //                              glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
        // model = glm::scale(model, glm::vec3(1.0f));
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        // model = glm::translate(model, lightPos);
        // model = glm::scale(model, glm::vec3(0.2f));
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));

        // glm::mat4 proj = glm::mat4(1.0f);

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        float cubeFloats[3] = {-0.5f, -0.5f, -2.0f};
        float tetraheadronFloats[3] = {-0.5f, 0.0f, 0.0f};

        auto tetraVerts = createTetraheadron(tetraheadronFloats[0], tetraheadronFloats[1], tetraheadronFloats[2]);
        auto cubeVerts = createCube(cubeFloats[0], cubeFloats[1], cubeFloats[2]);
        std::vector<Vertex> allVerts;
        allVerts.insert(allVerts.end(), tetraVerts.begin(), tetraVerts.end());
        allVerts.insert(allVerts.end(), cubeVerts.begin(), cubeVerts.end());
        Mesh mesh(allVerts, indices);

        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            // glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            processInput(window, deltaTime);

            auto tetraVerts = createTetraheadron(tetraheadronFloats[0], tetraheadronFloats[1], tetraheadronFloats[2]);
            auto cubeVerts = createCube(cubeFloats[0], cubeFloats[1], cubeFloats[2]);
            allVerts.clear();
            allVerts.insert(allVerts.end(), tetraVerts.begin(), tetraVerts.end());
            allVerts.insert(allVerts.end(), cubeVerts.begin(), cubeVerts.end());
            // mesh.updateVerts(allverts);
            glBufferSubData(GL_ARRAY_BUFFER, 0, allVerts.size() * sizeof(Vertex), allVerts.data());

            glm::mat4 proj = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
            shader.SetUniformMat4f("projection", proj);

            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            shader.SetUniformMat4f("view", view);

            //  model = glm::rotate(model, glm::radians(-1.0f),  glm::vec3(0.0f, 1.0f, 0.0f));
            shader.SetUniformMat4f("model", model);

            renderer.Clear();
            // renderer.Draw(vao, ibo, shader);
            renderer.DrawMesh(mesh);

            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Window");
            ImGui::DragFloat3("Cube Slider", cubeFloats, 0.1f);
            ImGui::DragFloat3("Tetraheadron Slider", tetraheadronFloats, 0.1f);
            // if(currentTest)
            // {
            // //     currentTest->OnUpdate(0.0f);
            // //     currentTest->OnRender();
            // //     ImGui::Begin("Test");
            // //     if(currentTest != testMenu && ImGui::Button("<-"))
            // //     {
            // //         delete currentTest;
            // //         currentTest = testMenu;
            // //     }
            // //     currentTest->OnImGuiRender();
            // //     ImGui::End();
            // // }
            ImGui::End();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // delete currentTest;
        // if(currentTest != testMenu){
        //     delete testMenu;
        // }
    }
    // IBO error loop fix by creating scope

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window, float deltaTime)
{
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    int tabState = glfwGetKey(window, GLFW_KEY_TAB);

    if (tabState == GLFW_PRESS && !tabWasPressed)
    {

        isFocused = !isFocused;
        glfwSetInputMode(window, GLFW_CURSOR, isFocused ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        tabWasPressed = true;
    }
    else if (tabState == GLFW_RELEASE)
    {
        tabWasPressed = false;
    }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{

    if (isFocused)
    {

        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

std::array<Vertex, 4> createTetraheadron(float x, float y, float z)
{
    float size = 1.0f;
    Vertex v0;
    v0.Position = glm::vec3(x , y , z );
    v0.Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);

    Vertex v1;
    v1.Position = glm::vec3(x + size, y, z);
    v1.Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);

    Vertex v2;
    v2.Position = glm::vec3(x + (size/2), y, z-0.87f );
    v2.Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);

    Vertex v3;
    v3.Position = glm::vec3(x + (size/2), y +0.82f , z-0.29f);
    v3.Color = glm::vec4(0.18f, 0.6f, 0.96f, 1.0f);

    return {v0, v1, v2, v3};
}

std::array<Vertex, 8> createCube(float x, float y, float z)
{

    float size = 1.0f;
    float depth = -1.0f;
    Vertex v4;
    v4.Position = glm::vec3(x, y, z);
    v4.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex v5;
    v5.Position = glm::vec3(x + size, y, z);
    v5.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex v6;
    v6.Position = glm::vec3(x + size, y + size, z);
    v6.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex v7;
    v7.Position = glm::vec3(x, y + size, z);
    v7.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex v8;
    v8.Position = glm::vec3(x, y, z + depth);
    v8.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    Vertex v9;

    v9.Position = glm::vec3(x + size, y, z + depth);
    v9.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex v10;
    v10.Position = glm::vec3(x + size, y + size, z + depth);
    v10.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Vertex v11;
    v11.Position = glm::vec3(x, y + size, z + depth);
    v11.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    return {v4, v5, v6, v7, v8, v9, v10, v11};
}