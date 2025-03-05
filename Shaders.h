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
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in float inWidth;

out vec4 vColor;

void main()
{
    gl_Position = vec4(inPos, 0.0, 1.0);
    gl_PointSize = max(1.f,inWidth);
    vColor = vec4(inColor.rgb, 1.0f);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec4 vColor;

void main()
{
    gl_FragColor = vColor;
}
)";

#endif