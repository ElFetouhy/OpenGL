#include <glew.h>
#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
// #include "Texture.h"

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    //glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    {
    float positions[]{
        -0.5f, -0.5f, //0.0f, 0.0f,
         0.5f, -0.5f, //1.0f, 0.0f,
         0.5f,  0.5f, //1.0f, 1.0f,
        -0.5f,  0.5f //0.0f, 1.0f
    };

    unsigned int indices[]{
        0, 1, 2,
        2, 3, 0
    };

    // GLCall(glEnable(GL_BLEND));
    // GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));

    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));

    VertexBufferLayout layout;
    // layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);


    IndexBuffer ib(indices, 6);

    Shader shader("..\\res\\shaders\\Shader.glsl");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
    // C:\Users\User\Desktop\VisualStudio\C++\OpenGl\res\shaders\Shader.glsl

    // Texture texture("..\\res\\textures\\logo.png"); //res\textures\logo.png
    // texture.Bind(1);
    // shader.SetUniform1i("u_Texture", 1);

    //Unbind everything 
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();
    // texture.Unbind();

    // Renderer renderer;

    float r = 0.0f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        // renderer.Clear();
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        va.Bind();
        ib.Bind();
        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // renderer.Draw(va, ib, shader);
        GLCall( glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if(r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;

        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // ib.Unbind();
    } // IBO error loop fix by creating scope 
    glfwTerminate();
    return 0;
}