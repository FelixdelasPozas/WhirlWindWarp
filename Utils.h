/*
 File: Utils.h
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

#ifndef _UTILS_H_
#define _UTILS_H_

// C++
#include <iostream>

class GLFWwindow;

namespace Utils
{
  /** @struct Configuration
   * @brief Configuration data struct.
   */
  struct Configuration
  {
    int line_width;   /** line width size. */
    int point_size;   /** point size. */
    bool antialias;   /** true if antialias enabled and false otherwise. */
    bool show_trails; /** true to show the particle trails and false otherwise. */

    /**
     * @brief Configuration constructor. 
     */
    Configuration(): line_width{1}, point_size{2}, antialias{true}, show_trails{true} {};
  };

  /**
   * @brief Dump Configuration information, for debugging purposes.
   * @param os Stream
   * @param config Configuration struct reference.
   * @return The stream.
   */
  std::ostream& operator<<(std::ostream& os, const struct Configuration &config);

  /**
   * @brief Loads the application configuration from the windows registry. 
   * @param[out] config Configuration struct reference. 
   */
  void loadConfiguration(Configuration &config);
  
  /**
   * @brief Saves the application configuration to the windows registry. 
   * @param[out] config Configuration struct reference. 
   */
  void saveConfiguration(const Configuration &config);

  /** \brief Helper method to convert a string to a wide-char string.
   * \param[in] str String reference. 
   *
   */
  std::wstring s2ws(const std::string& str);

  /** \brief  Error callback for errors.
   * \param error Error code.
   * \param description Error description.
   *
   */
  void errorCallback(int error, const char* description);

  /** \brief Key callback for glfw key processing.
   * \param[in] window GLFW window pointer.
   *
   */
  void glfwKeyCallback(GLFWwindow* window, int /* key */, int /* scancode */, int /* action */, int /* mods */);

  /** \brief Mouse movement callback for glfw.
   * \param[in] window GLFW window pointer.
   *
   */
  void glfwMousePosCallback(GLFWwindow *window, double /* xpos */, double /* ypos */);

  /** \brief Mouse buttons callback for glfw.
   * \param[in] window GLFW window pointer.
   *
   */
  void glfwMouseButtonCallback(GLFWwindow *window, int /* button */, int /* action */, int /* mods */);
}

#endif // _UTILS_H_

