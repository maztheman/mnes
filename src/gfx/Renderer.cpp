#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "fragment.cpp"
#include "vertex.cpp"

// most of this stuff is not needed since imgui is drawing the texture, thanks ImGui!

void Renderer::initVertexBuffers()
{
  unsigned int VBO;
  float verficies[] = { 0.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,

    0.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    0.0f,
    1.0f,
    0.0f

  };

  glGenVertexArrays(1, &quadVAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verficies), verficies, GL_STATIC_DRAW);

  glBindVertexArray(quadVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Renderer::initMainWindowTexture()
{
  // unsigned int framebuffer;
  // glGenFramebuffers(1, &framebuffer);
  // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glGenTextures(1, &mainwindowTextureId);
  glBindTexture(GL_TEXTURE_2D, mainwindowTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, m_screenBuffer.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// Linear Filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// Linear Filtering
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainwindowTextureId, 0);
}

void Renderer::init()
{
  // initVertexBuffers();
  initMainWindowTexture();
  // createProgramAndShaders();
  // initUniforms();
}

void Renderer::updateMainWindowTexture()
{
  glBindTexture(GL_TEXTURE_2D, mainwindowTextureId);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, m_screenBuffer.data());
  GLenum erro = glGetError();
  if (erro == GL_NO_ERROR) updateTick++;
}


void Renderer::createProgramAndShaders()
{
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &VERTEX_SOURCE, nullptr);
  glCompileShader(vertexShader);

  GLint success;

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
    printf("[VertexShader]: Error %s\n", infoLog);
    exit(-1);
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &FRAGMENT_SOURCE, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
    printf("[FragmentShader]: Error %s\n", infoLog);
    exit(-1);
  }

  program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    printf("[Program]: Error %s\n", infoLog);
    exit(-1);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Renderer::initUniforms()
{
  glUseProgram(program);
  glUniform1i(glGetUniformLocation(program, UNIFORM_IMAGE), 0);
}

/*void Renderer::updateProjection(int width, int height)
{
    glm::mat4 projection = glm::ortho(0.0f, float(width), float(height), 0.0f, -1.0f, 1.0f);
    glUseProgram(program);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(float(width), float(height), 1.0f));

    //set the uniform "model"
    glUniformMatrix4fv(glGetUniformLocation(program, UNIFORM_MODEL), 1, false, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, UNIFORM_PROJECTION), 1, false, glm::value_ptr(projection));
}*/

/*void Renderer::draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainwindowTextureId);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
*/