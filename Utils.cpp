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


LPCSTR KEY_BASEKEY   = "Software\\Felix de las Pozas Alvarez\\WhirlWindWarp";
LPCSTR KEY_LINEWIDTH = "LineWidth";
LPCSTR KEY_ANTIALIAS = "Antialias";
LPCSTR KEY_POINTSIZE = "PointSize";
LPCSTR KEY_SHOWTRAIL = "ShowTrail";

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
  std::wstring msg = L"Error code: " + std::to_wstring(error) + L"\nDescription: " + s2ws(description);
  std::wstring title = L"Error";
  MessageBoxW(nullptr, msg.c_str(), title.c_str(), MB_OK);
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
		GLint logLen = -1;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		char *logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, nullptr, logString);
		const std::string errorString = (logLen > 0 ? logString : "empty");
		delete[] logString;

		glDeleteShader(shader);
		errorCallback(EXIT_FAILURE, logString);
	}

	return shader;
}

//----------------------------------------------------------------------------
void Utils::initProgram(GL_program &program, attribList attribs)
{
  assert(program.vert != static_cast<unsigned int>(-1) && program.frag != static_cast<unsigned int>(-1));
	program.program = glCreateProgram();

	glAttachShader(program.program, program.vert);
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
		glGetProgramInfoLog(program.program, logLen, NULL, logString);
    const std::string errorString = (logLen > 0 ? logString : "empty");
		delete[] logString;

		glDeleteProgram(program.program);
		errorCallback(EXIT_FAILURE, errorString.c_str());
	}  
}

//----------------------------------------------------------------------------
std::wstring Utils::s2ws(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
