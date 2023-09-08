//https://www.glfw.org/docs/latest/quick.html
//http://graphics.stanford.edu/data/3Dscanrep/

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>

//static const struct
//{
//    float x, y;
//    float r, g, b;
//} vertices[3] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },        // Red
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },        // Green
//    {   0.f,  0.6f, 0.f, 0.f, 1.f }         // Blue
//};

struct sVertex
{
    float x, y;
    float r, g, b;
};

const unsigned int NUM_OF_VERTICES = 6;
sVertex vertices[NUM_OF_VERTICES] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },        // Red
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },        // Green
    {   0.f,  0.6f, 0.f, 0.f, 1.f },         // Blue
    /* Same, just +1 in the x axis */
    {  0.4f, -0.4f, 0.f, 1.f, 0.f },        // Red
    {  1.6f, -0.4f, 0.f, 0.f, 1.f },        // Green
    {  1.0f,  0.6f, 1.f, 0.f, 0.f }         // Blue
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    std::cout << "About to blow you mind with OpenGL!" << std::endl;

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int size_of_sVertex = sizeof(sVertex);
    unsigned int number_of_bytes_in_array = size_of_sVertex * NUM_OF_VERTICES;

    glBufferData(GL_ARRAY_BUFFER, number_of_bytes_in_array, vertices, GL_STATIC_DRAW);


    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Delete shader as it's already loaded on the program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);

    int i = 2;
    float temp = -.05f;
    while (!glfwWindowShouldClose(window))
    {
		//if (vertices[0].r != 1.0f && vertices[0].g != 1.0f && vertices[0].b != 1.0f)
		//{
		//	vertices[i % 3].r -= 0.05f;
		//	vertices[i % 3].g += 0.05f;
		//	vertices[(i + 1) % 3].g -= 0.05f;
		//	vertices[(i + 1) % 3].b += 0.05f;
		//	vertices[(i + 2) % 3].b -= 0.05f;
		//	vertices[(i + 2) % 3].r += 0.05f;
		//}
		//else
		//{
  //          //std::cout << "test" << std::endl;
		//	i++;
		//	i = 3 ? i = 0 : NULL;
		//	vertices[i % 3].r -= 0.05f;
		//	vertices[i % 3].g += 0.05f;
		//	vertices[(i + 1) % 3].g -= 0.05f;
		//	vertices[(i + 1) % 3].b += 0.05f;
		//	vertices[(i + 2) % 3].b -= 0.05f;
		//	vertices[(i + 2) % 3].r += 0.05f;
		//}
        /*if (vertices[0].g < 1.0f && vertices[0].g > 0.0f)
        {
            vertices[0].g += temp;
        }
        else
        {
            temp *= -1.0f;
            vertices[0].g += temp;
        }*/
        //std::cout << i << "    " << vertices[0].r << std::endl;
        glBufferData(GL_ARRAY_BUFFER, number_of_bytes_in_array, vertices, GL_STATIC_DRAW);





        float ratio;
        int width, height;
//        mat4x4 m, p, mvp;
        glm::mat4 m, p, v, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);                    // Clears the screen

        //         mat4x4_identity(m);
        m = glm::mat4(1.0f);

        //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
                                        (float)glfwGetTime(),
                                        glm::vec3(0.0f, 0.0, 1.0f));

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        p = glm::perspective(0.6f,
                             ratio,
                             0.1f,
                             1000.0f);

        v = glm::mat4(1.0f);

//        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        //cameraEye.z += 0.001f;

        v = glm::lookAt(cameraEye,
                        cameraTarget,
                        upVector);

       //mat4x4_mul(mvp, p, m);
        mvp = p * v * m;

        glUseProgram(program);
        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

//        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT /*GL_LINE*/ /*GL_FILL*/);
        glPointSize(10.0f);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}