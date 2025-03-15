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

// Points shaders
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
    vColor = inColor;
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

// Lines shaders
const char* vertexShaderSourceTrails = R"(
#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in float inWidth;

out vec4 vColor;
out float lineWidth;

void main()
{
    gl_Position = vec4(inPos, 0, 1);
    vColor = inColor;
    lineWidth = max(1.f,inWidth);
}
)";

const char* geometryShaderSource = R"(
#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vColor[];
in float lineWidth[];

out vec4 gColor;

uniform float ratioX;
uniform float ratioY;

void main()
{
    float r1 = lineWidth[0];
    float r2 = lineWidth[1];

    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec2 dir = normalize(p2.xy - p1.xy);
    vec2 normal = vec2(dir.y, -dir.x) * vec2(ratioX, ratioY);

    vec4 offset1 = vec4(normal * r1 / 2.f, 0, 0);
    vec4 offset2 = vec4(normal * r2 / 2.f, 0, 0);

    gColor = vColor[0];

    gl_Position = p1 + offset1;
    EmitVertex();
    gl_Position = p1 - offset1;
    EmitVertex();
    
    gColor = vec4(vColor[1].rgb, 0.75f);

    gl_Position = p2 + offset2;
    EmitVertex();
    gl_Position = p2 - offset2;
    EmitVertex();
    
    EndPrimitive();
}
)";

const char* fragmentShaderSourceTrails = R"(
#version 330 core
in vec4 gColor;

void main()
{
    gl_FragColor = gColor;
}
)";

// Post-processing shaders
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