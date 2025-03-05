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

  glfwSetErrorCallback(Utils::errorCallback);

  if (!glfwInit())
  {
    Utils::errorCallback(EXIT_FAILURE, "Failed to initialize GLFW");
  }

  // TODO: get monitors, create contexts.
  int count = 0;
  const auto monitors = glfwGetMonitors(&count);
  int numPoints = 0;
  for(int i = 0; i < count; ++i)
  {
    const auto monitor = monitors[i];
    std::cout << "monitor " << glfwGetMonitorName(monitor) << std::endl;
    int width, height;
    glfwGetMonitorPos(monitor, &width, &height);
    std::cout << "pos " << width << "," << height << std::endl;
    auto res = glfwGetVideoMode(monitor);
    std::cout << "res " << res->width << "x" << res->height << std::endl;
    numPoints = (res->width * res->height) / 3500;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, config.antialias ? 4 : 1);
  GLFWwindow *window = glfwCreateWindow(1280, 1024, "Colored Lines", nullptr, nullptr);
  if (!window)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, Utils::glfwKeyCallback);
  glfwSetCursorPosCallback(window, Utils::glfwMousePosCallback);
  glfwSetMouseButtonCallback(window, Utils::glfwMouseButtonCallback);
  glfwSwapInterval(1);

  if(load_gl_functions() > 0)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "Failed to load OpenGL functions");
  }

  Utils::GL_program program("default");
  program.vert = Utils::loadShader(vertexShaderSource, GL_VERTEX_SHADER);
  program.frag = Utils::loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  Utils::initProgram(program);

  WhirlWindWarp www(numPoints);

  auto vertices = www.buffer();

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

  // FPS ------------------------------
  static float fpsTime = 0;
  static unsigned long fps = 0;
  // FPS ------------------------------

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDepthMask(GL_FALSE);

    if(config.antialias)
      glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);

    glEnable(GL_PROGRAM_POINT_SIZE);

    www.advance();

    glUseProgram(program.program);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numPoints * 7, vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_POINTS, 0, numPoints);
    glfwSwapBuffers(window);

    // FPS ------------------------------
    ++fps;
    const auto cTime = glfwGetTime();
    if(cTime - fpsTime > 1.f) 
    {
      std::cout << "fps " << fps << std::endl;
      fps = 0;
      fpsTime = cTime;
    }
    // FPS ------------------------------

    glfwPollEvents();
    glBindVertexArray(0);
  }

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(program.program);

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
