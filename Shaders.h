/*
 File: Shaders.h
 Created on: 26/02/2025
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

#ifndef _SHADERS_H_
#define _SHADERS_H_

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos1;
layout(location = 1) in vec4 aColor1;

out vec4 ourColor;

void main()
{
    gl_Position = vec4(aPos1, 1.0);
    ourColor = aColor1;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec4 ourColor;
out vec4 FragColor;

void main()
{
    FragColor = ourColor;
}
)";

#endif