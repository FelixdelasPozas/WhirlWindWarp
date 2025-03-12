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

const float quadVertices[] = {
    -1.0f, 1.0f,  // Top-left
    -1.0f, -1.0f, // Bottom-left
    1.0f, -1.0f,  // Bottom-right
    1.0f, 1.0f    // Top-right
};

const unsigned int quadIndices[] = {0, 1, 2,
                                    0, 2, 3};

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
  const auto glfwmonitors = glfwGetMonitors(&count);
  if(!glfwmonitors)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "No monitors detected");
  }

  int xMin = std::numeric_limits<int>::max(), yMin = std::numeric_limits<int>::max();
  int virtualWidth = 0, virtualHeight = 0;
  Utils::Monitors monitors(count);

  for(int i = 0; i < count; ++i)
  {
    const auto glfwmonitor = glfwmonitors[i];
    int xPos, yPos;
    glfwGetMonitorPos(glfwmonitor, &xPos, &yPos);
    const auto res = glfwGetVideoMode(glfwmonitor);

    monitors[i].name = glfwGetMonitorName(glfwmonitor);
    monitors[i].xPos = xPos;
    monitors[i].yPos = yPos;
    monitors[i].width = res->width;
    monitors[i].height = res->height;

    xMin = std::min(xMin, xPos);
    yMin = std::min(yMin, yPos);
    virtualWidth = std::max(virtualWidth, xPos + res->width);
    virtualHeight = std::max(virtualHeight, yPos + res->height);
  }

  // compute factors and modifiers for the points to draw in this monitor.
  for(int i = 0; i < count; ++i)
  {
    monitors[i].xMultiplier = virtualWidth / monitors[i].width;
    monitors[i].yMultiplier = virtualHeight / monitors[i].height;
    monitors[i].xFactor = - (static_cast<float>(monitors[i].xPos)  / virtualWidth) * monitors[i].xMultiplier;
    monitors[i].yFactor = - (static_cast<float>(monitors[i].yPos)  / virtualHeight) * monitors[i].yMultiplier;
  }
  const int numPoints = (virtualWidth * virtualHeight) / 1000; // one point per 1000 pixels.

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
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

  Utils::GL_program postprogram("post-processing");
  postprogram.vert = Utils::loadShader(ppVertexShaderSource, GL_VERTEX_SHADER);
  postprogram.frag = Utils::loadShader(ppFragmentShaderSource, GL_FRAGMENT_SHADER);

  Utils::initProgram(postprogram);

  // uniforms to compute which points draw in witch monitor.
  const auto uxMult = glGetUniformLocation(program.program, "xMult");
  const auto uyMult = glGetUniformLocation(program.program, "yMult");
  const auto uxFactor = glGetUniformLocation(program.program, "xFactor");
  const auto uyFactor = glGetUniformLocation(program.program, "yFactor");

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

  GLuint colorTexture;
  glGenTextures(1, &colorTexture);
  glBindTexture(GL_TEXTURE_2D, colorTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    glfwTerminate();
    Utils::errorCallback(EXIT_FAILURE, "Framebuffer not complete!");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);    
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
	  glDisable(GL_DEPTH_TEST);

    if(config.antialias)
      glEnable(GL_MULTISAMPLE);

    glEnable(GL_PROGRAM_POINT_SIZE);

    www.advance();

    glUseProgram(program.program);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numPoints * 7, vertices, GL_DYNAMIC_DRAW);

    glUniform1fv(uxMult, 1, &(monitors[0].xMultiplier));
    glUniform1fv(uyMult, 1, &(monitors[0].yMultiplier));
    glUniform1fv(uxFactor, 1, &(monitors[0].xFactor));
    glUniform1fv(uyFactor, 1, &(monitors[0].yFactor));

    glDrawArrays(GL_POINTS, 0, numPoints);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_BLEND);
	  glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
	  glBlendColor(1.f, 1.f, 1.f, 0.9f);
	  glBlendEquation(GL_FUNC_ADD);    

    glUseProgram(postprogram.program);
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Swap buffers and poll events
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

  //Utils::saveScreenshotToFile("C:\\Users\\felix\\Downloads\\prueba.tga", 1280, 1024);

  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(program.program);

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
