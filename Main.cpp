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
#include <State.h>
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
int main() 
{
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
  for(int i = 0; i < count; ++i)
  {
    const auto monitor = monitors[i];
    std::cout << "monitor " << glfwGetMonitorName(monitor) << std::endl;
    int width, height;
    glfwGetMonitorPos(monitor, &width, &height);
    std::cout << "pos " << width << "," << height << std::endl;
    auto res = glfwGetVideoMode(monitor);
    std::cout << "res " << res->width << "x" << res->height << std::endl;
  }

  // Create a windowed mode window and its OpenGL context
  glfwWindowHint(GLFW_SAMPLES, config.antialias ? 4 : 1);
  GLFWwindow *window = glfwCreateWindow(800, 600, "Colored Lines", nullptr, nullptr);
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

  const auto failed = load_gl_functions();
  std::cout << "failed " << failed << " functions." << std::endl;

  // Compile shaders
  Utils::GL_program program("default");
  program.vert = Utils::loadShader(vertexShaderSource, GL_VERTEX_SHADER);
  program.frag = Utils::loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  // Create shader program
  Utils::initProgram(program);

  // Define line data with separate colors for each endpoint
  float vertices[] = {
      -1.f, -1.f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

  // Create VAO and VBO
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  // Position and color attributes for endpoint 1
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Position and color attributes for endpoint 2
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(9 * sizeof(float)));
  glEnableVertexAttribArray(3);

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);
    if(config.antialias) glEnable(GL_MULTISAMPLE);  
    glLineWidth(config.line_width);

    glUseProgram(program.program);

    // vertices[0] = vertices[1] = -1 + std::fabs(std::sin(glfwGetTime()));
    // vertices[7] = 1 - std::fabs(std::sin(glfwGetTime()));
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW );
    glDrawArrays(GL_LINES, 0, 2); // Draw two lines

    glfwSwapBuffers(window);
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

//-----------------------------------------------------------------
int unused(int argc, char *argv[])
{
  // QCommandLineParser uses unix-like '-' for flags, while windows uses '/'. There is no way for QCommandLineParser to adapt to it
  // so we'll need to change all characters first. Also the 'c' option can have an optional value, can be present or not, and
  // QCommandLineParser also doesn't support that so we'll need to truncate the argument to void that value (parent window handler?).
  for(int i = 1; i < argc; ++i)
  {
    bool dash = false;
    bool clear = false;
    for(char *j = argv[i]; *j; ++j)
    {
      if(*j == '/')
      {
        *j = '-';
        dash = true;
      }
      else
      {
        if(dash)
        {
          if(*j == 'c' || *j == 'C') clear = true;
          dash = false;
        }
        else
        {
          if(clear)
          {
            *j = '\0';
            break;
          }
        }
      }
    }
  }

  Utils::Configuration config;
  Utils::loadConfiguration(config);

  std::cout << config;

  //Utils::saveConfiguration(config);

  return 0;
}
