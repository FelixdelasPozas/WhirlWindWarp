/*
 File: Main.cpp
 Created on: 02/04/2017
 Author: Felix de las Pozas Alvarez

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Project
#include <WhirlWindWarp.h>
#include <version.h>
#include <Utils.h>
#include <Shaders.h>

// GLFW
//#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <external/gl_loader.h>
#include <GLFW/glfw3.h>

// C++
#include <iostream>

//----------------------------------------------------------------------------
int main(int argc, char *argv[]) 
{
  // TODO: read arguments. 
  
  Utils::Configuration config;
  Utils::loadConfiguration(config);
  config.show_trails = false;

  glfwSetErrorCallback(Utils::errorCallback);

  if (!glfwInit())
  {
    Utils::errorCallback(EXIT_FAILURE, "Failed to initialize GLFW");
  }

  int monitorCount = 0;
  const auto glfwmonitors = glfwGetMonitors(&monitorCount);
  if(!glfwmonitors)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "No monitors detected");
  }

  int xMin = std::numeric_limits<int>::max(), yMin = std::numeric_limits<int>::max();
  int virtualWidth = 0, virtualHeight = 0;

  for(int i = 0; i < monitorCount; ++i)
  {
    const auto glfwmonitor = glfwmonitors[i];
    int xPos, yPos;
    glfwGetMonitorPos(glfwmonitor, &xPos, &yPos);
    const auto res = glfwGetVideoMode(glfwmonitor);

    xMin = std::min(xMin, xPos);
    yMin = std::min(yMin, yPos);
    virtualWidth = std::max(virtualWidth, xPos + res->width);
    virtualHeight = std::max(virtualHeight, yPos + res->height);
  }

  const int numPoints = (virtualWidth * virtualHeight) / 400; // one point per 1000 pixels.

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, config.antialias ? 4 : 1);
  glfwWindowHint(GLFW_DECORATED, false);
  glfwWindowHint(GLFW_FLOATING, true);
  glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
  glfwWindowHint(GLFW_POSITION_X, xMin);
  glfwWindowHint(GLFW_POSITION_Y, yMin);
    
  auto window = glfwCreateWindow(virtualWidth, virtualHeight, "Monitor", nullptr, nullptr);
  if (!window)
  {
    glfwTerminate();
    const std::string msg = std::string("Failed to create GLFW window for monitor");
    Utils::errorCallback(EXIT_FAILURE, msg.c_str());
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, Utils::glfwKeyCallback);
  glfwSetCursorPosCallback(window, Utils::glfwMousePosCallback);
  glfwSetMouseButtonCallback(window, Utils::glfwMouseButtonCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  glfwSwapInterval(1);

  if(load_gl_functions() > 0)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "Failed to load OpenGL functions");
  }

  std::cout << "GL Vendor: " << glGetString(GL_VENDOR) << '\n';
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
  std::cout << "Version: " << glGetString(GL_VERSION) << '\n';
  std::cout << "Shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  WhirlWindWarp www(numPoints);
  auto vertices = www.buffer();

  glfwMakeContextCurrent(window);

  Utils::GL_program program("default");
  program.vert = Utils::loadShader(vertexShaderSource, GL_VERTEX_SHADER);
  program.frag = Utils::loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  Utils::initProgram(program);

  Utils::GL_program postprogram("post-processing");
  postprogram.vert = Utils::loadShader(ppVertexShaderSource, GL_VERTEX_SHADER);
  postprogram.frag = Utils::loadShader(ppFragmentShaderSource, GL_FRAGMENT_SHADER);

  Utils::initProgram(postprogram);

  // Create VAO and VBO
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numPoints * 7, vertices, GL_DYNAMIC_DRAW);

  const GLsizei stride = 7 * sizeof(float);

  // Position and color attributes for endpoint 1
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
  glEnableVertexAttribArray(0);

  // Color attribute
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Line/Point width
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Create VAO and VBOs for post-processing quad
  GLuint quadVAO, quadVBO, quadEBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glGenBuffers(1, &quadEBO);

  glBindVertexArray(quadVAO);

  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Setup framebuffer and texture to accumulate colors
  GLuint framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, virtualWidth, virtualHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "Framebuffer not complete!");
  }

  // Render loop
  bool finished = false;
  while (!finished)
  {
    www.advance(glfwGetTime());

    finished |= glfwWindowShouldClose(window);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);    
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    if(config.antialias)
      glEnable(GL_MULTISAMPLE);

    glEnable(GL_PROGRAM_POINT_SIZE);
      
    glUseProgram(program.program);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numPoints * 7, vertices, GL_DYNAMIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Line/Point width
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_POINTS, 0, numPoints);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(config.show_trails)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
      glBlendColor(1.f, 1.f, 1.f, 0.65f);
      glBlendEquation(GL_FUNC_ADD);
    }

    glUseProgram(postprogram.program);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(program.program);

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
  glDeleteBuffers(1, &quadEBO);
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &framebuffer);
  glDeleteProgram(postprogram.program);

  glfwDestroyWindow(window);

  glfwTerminate();

  Utils::saveConfiguration(config);

  return EXIT_SUCCESS;
}
