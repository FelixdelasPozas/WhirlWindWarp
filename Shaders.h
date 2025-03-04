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

const char* trailsVertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in float inWidth;

out vec4 vColor;
out float lineWidth;

void main()
{
    gl_Position = vec4(inPos, 0.0, 1.0);
    vColor = inColor;
    lineWidth = inWidth;
}
)";

const char* trailsGeometryShaderSource = R"(
#version 330

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vColor[];
in float lineWidth[];

out vec4 gColor;

void main()
{
    float r1 = lineWidth[0] / 150;
    float r2 = lineWidth[1] / 150;

    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec2 dir = normalize(p2.xy - p1.xy);
    vec2 normal = vec2(dir.y, -dir.x);

    vec4 offset1, offset2;
    offset1 = vec4(normal * r1 / 5, 0, 0);
    offset2 = vec4(normal * r2 / 5, 0, 0);

    vec4 coords[4];
    coords[0] = p1 + offset1;
    coords[1] = p1 - offset1;
    coords[2] = p2 + offset2;
    coords[3] = p2 - offset2;

    for (int i = 0; i < 4; ++i) {
        gl_Position = coords[i];
        // avoid if-else with step
        gColor = step(2, i) * (vColor[1] - vColor[0]) + vColor[0];
        EmitVertex();
    }
    EndPrimitive();
}
)";

const char* trailsFragmentShaderSource = R"(
#version 330 core
in vec4 gColor;

void main()
{
    if(gColor.w == 0.f) discard;
    gl_FragColor = gColor;
}
)";

const char* pointsVertexShaderSource = R"(
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

const char* pointsFragmentShaderSource = R"(
#version 330 core
in vec4 vColor;

void main()
{
    gl_FragColor = vColor;
}
)";

#endif