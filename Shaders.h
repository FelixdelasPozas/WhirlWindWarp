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
    gl_Position = vec4(inPos, 0, 1);
    gl_PointSize = max(1.f,inWidth);
    vColor = vec4(inColor.rgb, 1);
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

const char* ppVertexShaderSource =  R"(
#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1, 1);
    TexCoord = aPos * 0.5 + 0.5;
}
)";

const char* ppFragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
    gl_FragColor = texture(screenTexture, TexCoord);
}
)";

const float quadVertices[] = {
    -1.0f, 1.0f,  // Top-left
    -1.0f, -1.0f, // Bottom-left
    1.0f, -1.0f,  // Bottom-right
    1.0f, 1.0f    // Top-right
};

const unsigned int quadIndices[] = {0, 1, 2,
                                    0, 2, 3};

#endif