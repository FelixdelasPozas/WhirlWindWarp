/*
 File: Utils.cpp
 Created on: 23/06/2024
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
#include <Utils.h>

// GLFW
#include <external/gl_loader.h>
#include <GLFW/glfw3.h>

// C++
#include <windows.h>
#include <winreg.h>
#include <iostream>
#include <string>
#include <winuser.h>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <memory>
#include <vector>

LPCSTR KEY_BASEKEY   = "Software\\Felix de las Pozas Alvarez\\WhirlWindWarp";
LPCSTR KEY_LINEWIDTH = "LineWidth";
LPCSTR KEY_ANTIALIAS = "Antialias";
LPCSTR KEY_POINTSIZE = "PointSize";
LPCSTR KEY_SHOWTRAIL = "ShowTrail";

//----------------------------------------------------------------------------
std::ostream &Utils::operator<<(std::ostream &os, const Monitor &monitor)
{
  os << "Monitor -----------" << std::endl
     << "x pos: " << monitor.xPos << '\n'
     << "y pos: " << monitor.yPos << '\n'
     << "width: " << monitor.width << '\n'
     << "height: " << monitor.height << '\n'
     << "x mult: " << monitor.xMultiplier << '\n'
     << "y mult: " << monitor.yMultiplier << '\n'
     << "x factor: " << monitor.xFactor << '\n'
     << "y factor: " << monitor.yFactor << std::endl;

  return os;
}

//----------------------------------------------------------------------------
std::ostream &Utils::operator<<(std::ostream &os, const Configuration &config)
{
  os << "Config -----------" << std::endl
     << "line width : " << config.line_width << '\n'
     << "point size : " << config.point_size << '\n'
     << "antialias  : " << (config.antialias ? "true" : "false") << '\n'
     << "show trails: " << (config.show_trails ? "true" : "false") << std::endl;

  return os;
}

//----------------------------------------------------------------------------
void Utils::loadConfiguration(Configuration &config)
{
  HKEY default_key;
  auto status = RegOpenKeyExA(HKEY_CURRENT_USER, KEY_BASEKEY, 0, KEY_QUERY_VALUE, &default_key);

  if (status == ERROR_SUCCESS)
  {
    DWORD dataVal = 0;
    
    auto readRegistryValue = [&status, &dataVal, &default_key](LPCSTR key)
    {
      DWORD size = sizeof(DWORD);
      return RegGetValueA(default_key, "", key, RRF_RT_DWORD, nullptr, &dataVal, &size);
    };

    if(ERROR_SUCCESS == readRegistryValue(KEY_LINEWIDTH))
      config.line_width = dataVal;

    if(ERROR_SUCCESS == readRegistryValue(KEY_ANTIALIAS))
      config.antialias = (dataVal == true);

    if(ERROR_SUCCESS == readRegistryValue(KEY_POINTSIZE))
      config.point_size = dataVal;

    if(ERROR_SUCCESS == readRegistryValue(KEY_SHOWTRAIL))
      config.show_trails = (dataVal == true);

    RegCloseKey(default_key);
  }
  else
    std::cerr << "loadConfiguration: unable to open main key" << std::endl;
}

//----------------------------------------------------------------------------
void Utils::saveConfiguration(const Configuration &config)
{
  HKEY default_key;
  auto status = RegCreateKeyExA(HKEY_CURRENT_USER, KEY_BASEKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &default_key, NULL);
  if (status == ERROR_SUCCESS)
  {
    auto saveRegistryValue = [&status, &default_key](LPCSTR key, DWORD value)
    {
      return RegSetValueExA(default_key, key, 0, REG_DWORD, (BYTE *)(&value), sizeof(DWORD));
    };

    saveRegistryValue(KEY_LINEWIDTH, config.line_width);
    saveRegistryValue(KEY_ANTIALIAS, config.antialias ? 0 : 1);
    saveRegistryValue(KEY_POINTSIZE, config.point_size);
    saveRegistryValue(KEY_SHOWTRAIL, config.show_trails ? 0 : 1);

    RegCloseKey(default_key);
  }
  else
    std::cerr << "saveConfiguration: unable to open main key" << std::endl;
}

//----------------------------------------------------------------------------
void Utils::errorCallback(int error, const char *description)
{
  std::cout << "\n--\n" << description << "\n--\n" << std::endl;
  std::string msg = "Error code: " + std::to_string(error) + "\nDescription: " + description;
  std::string title = "Error";
  MessageBoxA(nullptr, msg.c_str(), title.c_str(), MB_OK);
  std::exit(-1);
}

//----------------------------------------------------------------------------
void Utils::glfwKeyCallback(GLFWwindow *window, int, int , int, int )
{
  glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//----------------------------------------------------------------------------
void Utils::glfwMousePosCallback(GLFWwindow *window, double xpos, double ypos)
{
  static double x = -1;
  static double y = -1;

  if(x == -1) x = xpos;
  if(y == -1) y = ypos;
  if(x != xpos || y != ypos)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//----------------------------------------------------------------------------
void Utils::glfwMouseButtonCallback(GLFWwindow *window, int, int, int)
{
  glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//----------------------------------------------------------------------------
GLint Utils::loadShader(const char *source, GLenum type)
{
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar **)&source, nullptr);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
    const std::string shaderType = type == GL_VERTEX_SHADER ? "Vertex":"Fragment";

		GLint logLen = -1;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
    memset(logString, 0, logLen);
		glGetShaderInfoLog(shader, logLen, nullptr, logString);
    
		const std::string errorString = shaderType + std::string(" shader compilation failed : ") + (logLen > 0 ? logString : "empty");
		delete[] logString;

		glDeleteShader(shader);
		errorCallback(EXIT_FAILURE, errorString.c_str());
	}

	return shader;
}

//----------------------------------------------------------------------------
void Utils::initProgram(GL_program &program, attribList attribs)
{
	program.program = glCreateProgram();

  if(program.vert != static_cast<unsigned int>(-1))
	  glAttachShader(program.program, program.vert);

  if(program.geom != static_cast<unsigned int>(-1))
    glAttachShader(program.program, program.geom);

  if(program.frag != static_cast<unsigned int>(-1))    
	  glAttachShader(program.program, program.frag);

  for(auto &[pos, attribName]: attribs)
    glBindAttribLocation(program.program, pos, attribName.c_str());

	glLinkProgram(program.program);

	int linked;
	glGetProgramiv(program.program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint logLen;
		glGetProgramiv(program.program, GL_INFO_LOG_LENGTH, &logLen);

		char* logString = new char[logLen];
    memset(logString, 0, logLen);
		glGetProgramInfoLog(program.program, logLen, NULL, logString);
    const std::string errorString = std::string("GL program link failed: ") + (logLen > 0 ? logString : "empty");
		delete[] logString;

		glDeleteProgram(program.program);
		errorCallback(EXIT_FAILURE, errorString.c_str());
	}  
}

//----------------------------------------------------------------------------
void Utils::saveScreenshotToFile(const std::string &filename, int windowWidth, int windowHeight)
{
  const int numberOfPixels = windowWidth * windowHeight * 3;
  auto pixels = std::make_unique<std::vector<unsigned char>>(numberOfPixels);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_FRONT);
  glReadPixels(0, 0, windowWidth, windowHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels->data());

  std::ofstream outputFile(filename.c_str(), std::ofstream::out);
  short header[] = {0, 2, 0, 0, 0, 0, (short)windowWidth, (short)windowHeight, 24};
  outputFile.write(reinterpret_cast<const char *>(header), sizeof(header));
  outputFile.write(reinterpret_cast<const char *>(pixels->data()), numberOfPixels);
  outputFile.close();

  std::cout << "Finish writing to file: " << filename << std::endl;
}

//----------------------------------------------------------------------------
std::wstring Utils::s2ws(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

//--------------------------------------------------------------------
Utils::NumberGenerator::NumberGenerator(const float min, const float max)
: m_distribution{min, max}
{
  std::srand(std::time(0));
  m_generator.discard(std::rand() % 200);
}

//--------------------------------------------------------------------
const float Utils::NumberGenerator::get()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_distribution(m_generator);
}

//--------------------------------------------------------------------
Utils::hsv Utils::rgb2hsv(Utils::rgb in)
{
  hsv out;
  float min, max, delta;

  min = in.r < in.g ? in.r : in.g;
  min = min < in.b ? min : in.b;

  max = in.r > in.g ? in.r : in.g;
  max = max > in.b ? max : in.b;

  out.v = max; // v
  delta = max - min;
  if (delta < 0.00001)
  {
    out.s = 0;
    out.h = 0; // undefined, maybe nan?
    return out;
  }
  if (max > 0.0)
  {                        // NOTE: if Max is == 0, this divide would cause a crash
    out.s = (delta / max); // s
  }
  else
  {
    // if max is 0, then r = g = b = 0
    // s = 0, h is undefined
    out.s = 0.0;
    out.h = NAN; // its now undefined
    return out;
  }
  if (in.r >= max)                 // > is bogus, just keeps compilor happy
    out.h = (in.g - in.b) / delta; // between yellow & magenta
  else if (in.g >= max)
    out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
  else
    out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan

  out.h *= 60.0; // degrees

  if (out.h < 0.0)
    out.h += 360.0;

  return out;
}

//--------------------------------------------------------------------
Utils::rgb Utils::hsv2rgb(Utils::hsv in)
{
  float hh, p, q, t, ff;
  long i;
  rgb out;

  if (in.s <= 0.0)
  { // < is bogus, just shuts up warnings
    out.r = in.v;
    out.g = in.v;
    out.b = in.v;
    return out;
  }
  hh = in.h;
  if (hh >= 360.0)
    hh = 0.0;
  hh /= 60.0;
  i = static_cast<long>(hh);
  ff = hh - i;
  p = in.v * (1.0 - in.s);
  q = in.v * (1.0 - (in.s * ff));
  t = in.v * (1.0 - (in.s * (1.0 - ff)));

  switch (i)
  {
  case 0:
    out.r = in.v;
    out.g = t;
    out.b = p;
    break;
  case 1:
    out.r = q;
    out.g = in.v;
    out.b = p;
    break;
  case 2:
    out.r = p;
    out.g = in.v;
    out.b = t;
    break;
  case 3:
    out.r = p;
    out.g = q;
    out.b = in.v;
    break;
  case 4:
    out.r = t;
    out.g = p;
    out.b = in.v;
    break;
  case 5:
  default:
    out.r = in.v;
    out.g = p;
    out.b = q;
    break;
  }
  return out;
}
