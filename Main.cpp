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
#include <Particle.h>

// GLFW
#include <GL/gl.h>
#include <GL/glext.h>
#include <external/gl_loader.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// C++
#include <iostream>

//----------------------------------------------------------------------------
int main(int argc, char *argv[]) 
{
  // TODO: read arguments. 
  
  Utils::Configuration config;
  Utils::loadConfiguration(config);

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

  const int numPoints = (virtualWidth * virtualHeight) / config.pixelsPerPoint;

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

  WhirlWindWarp www(numPoints, config);
  auto vertices = www.buffer();

  glfwMakeContextCurrent(window);

  Utils::GL_program points("default");
  points.vert = Utils::loadShader(vertexShaderSource, GL_VERTEX_SHADER);
  points.frag = Utils::loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  Utils::initProgram(points);

  Utils::GL_program trails("trails");
  trails.vert = Utils::loadShader(vertexShaderSourceTrails, GL_VERTEX_SHADER);
  trails.geom = Utils::loadShader(geometryShaderSource, GL_GEOMETRY_SHADER);
  trails.frag = Utils::loadShader(fragmentShaderSourceTrails, GL_FRAGMENT_SHADER);

  Utils::initProgram(trails);

  auto uratioX = glGetUniformLocation(trails.program, "ratioX");
  auto uratioY = glGetUniformLocation(trails.program, "ratioY");

  Utils::GL_program post("post-processing");
  post.vert = Utils::loadShader(ppVertexShaderSource, GL_VERTEX_SHADER);
  post.frag = Utils::loadShader(ppFragmentShaderSource, GL_FRAGMENT_SHADER);

  Utils::initProgram(post);

  // Create VAO and VBO
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  const auto multiplier = config.show_trails ? 2:1;
  const GLsizei stride = sizeof(Particle);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, multiplier * numPoints * stride, vertices, GL_DYNAMIC_DRAW);

  // Position attribute
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

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, virtualWidth, virtualHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "Framebuffer not complete!");
  }

  // openg coords are {-1,1} get ratio coords/pixels
  const float ratioX = 2.f / virtualWidth;
  const float ratioY = 2.f / virtualHeight;

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);  

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    www.advance();

    glBindFramebuffer(GL_FRAMEBUFFER, (config.motion_blur ? framebuffer : 0));    
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    if(config.antialias)
      glEnable(GL_MULTISAMPLE);

    if(config.show_trails)
    {
      glUseProgram(trails.program);  

      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, multiplier * numPoints * stride, vertices, GL_DYNAMIC_DRAW);

      // Position attribute
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
      glEnableVertexAttribArray(0);

      // Color attribute
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(2 * sizeof(float)));
      glEnableVertexAttribArray(1);

      // Line/Point width
      glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);

      glUniform1fv(uratioX, 1, &ratioX);
      glUniform1fv(uratioY, 1, &ratioY);

      glDrawArrays(GL_LINES, 0, numPoints * multiplier);
    }
      
    glUseProgram(points.program);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, multiplier * numPoints * stride, vertices, GL_DYNAMIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride * multiplier, (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride * multiplier, (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Line/Point width
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride * multiplier, (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_POINTS, 0, numPoints);

    if(config.motion_blur)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glEnable(GL_BLEND);
      glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
      glBlendColor(1.f, 1.f, 1.f, 0.75f);
      glBlendEquation(GL_FUNC_ADD);

      glUseProgram(post.program);

      glBindVertexArray(quadVAO);

      glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);

      // Position attribute
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
      glEnableVertexAttribArray(0);

      glBindTexture(GL_TEXTURE_2D, texture);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glDisable(GL_BLEND);
    }
    
    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glBindVertexArray(0);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(points.program);
  glDeleteProgram(trails.program);

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
  glDeleteBuffers(1, &quadEBO);
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &framebuffer);
  glDeleteProgram(post.program);

  glfwDestroyWindow(window);

  glfwTerminate();

  Utils::saveConfiguration(config);

  return EXIT_SUCCESS;
}
