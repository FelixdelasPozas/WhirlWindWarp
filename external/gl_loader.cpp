/*
 File: gl_loader.cpp
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

// Project
#include "gl_loader.h"

// C++
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

/** \brief Loads and returns the address of the given function name from the opengl32.dll library.
 * \param name Function name.
 *
 */
void *GetAnyGLFuncAddress(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);
	if (p == 0 || (p == (void *)0x1) || (p == (void *)0x2) || (p == (void *)0x3) || (p == (void *)-1))
	{
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}

	return p;
}

/** \brief Names of GL functions to load.
 *
 */
const char *gl_function_names[] = {
	"glActiveTexture",
	"glDebugMessageCallbackAMD",
	"glDebugMessageEnableAMD",
	"glDebugMessageInsertAMD",
	"glGetDebugMessageLogAMD",
	"glDebugMessageCallback",
	"glDebugMessageControl",
	"glGetActiveUniformBlockName",
	"glGetActiveUniformBlockiv",
	"glGetActiveUniformsiv",
	"glGetActiveUniformName",
	"glCompileShader",
	"glGetShaderiv",
	"glGetShaderInfoLog",
	"glCreateProgram",
	"glProgramBinary",
	"glCreateShader",
	"glShaderSource",
	"glDeleteShader",
	"glGetProgramiv",
	"glAttachShader",
	"glProgramParameteri",
	"glLinkProgram",
	"glDeleteProgram",
	"glUseProgram",
	"glGetUniformLocation",
	"glGetUniformBlockIndex",
	"glGetProgramBinary",
	"glGenVertexArrays",
	"glBindVertexArray",
	"glEnableVertexAttribArray",
	"glGenBuffers",
	"glBindBuffer",
	"glBufferData",
	"glVertexAttribPointer",
	"glDeleteBuffers",
	"glDeleteVertexArrays",
	"glUniform1f",
	"glUniform3fv",
	"glUniform4fv",
	"glUniformMatrix4fv",
	"glBindBufferBase",
	"glDispatchCompute",
	"glMemoryBarrier",
	"glMultiDrawArrays",
	"glDrawBuffers",
	"glGenFramebuffers",
	"glFramebufferTexture2D",
	"glCheckFramebufferStatus",
	"glBindFramebuffer",
	"glDeleteFramebuffers",
	"glGetQueryObjectiv",
	"glGetQueryObjectui64v",
	"glTexImage2DMultisample",
	"glGenerateMipmap",
	"glUniform1i",
	"glGetAttribLocation",
	"glBufferSubData",
	"glUniform4iv",
	"glBindAttribLocation",
	"glGetProgramInfoLog",
	"glBlendEquation",
	"glBlendFuncSeparate",
	"glBlendColor",
	"glUniform1fv"
};

/** \brief Array of GL function pointers.
 *
 */
void *gl_function_pointers[sizeof(gl_function_names) / sizeof(const char *)];

#ifdef DEBUG
/** \brief Names of GL debug functions to load.
 *
 */
const char *gl_debug_function_names[] = {
	"glGetTextureParameteriv",
	"glDetachShader",
	"glGenQueries",
	"glEndQuery",
	"glBeginQuery",
	"glGetProgramiv",
	"glMapBuffer",
	"glUnmapBuffer"
};

/** \brief Array of GL debug function pointers.
 *
 */
void *gl_debug_function_pointers[sizeof(gl_function_names) / sizeof(const char *)];
#endif

//----------------------------------------------------------------------------
int load_gl_functions()
{
	int failed = 0;
	for (unsigned long long i = 0; i < sizeof(gl_function_names) / sizeof(const char *); i++)
	{
		const char *name = gl_function_names[i];
		void *ptr = GetAnyGLFuncAddress(name);
		gl_function_pointers[i] = ptr;
		if (!ptr)
		{
			std::cerr << "Failed to load GL func '" << name << "'" << std::endl;
			failed++;
		}
	}

#ifdef DEBUG
	for (unsigned long long  i = 0; i < sizeof(gl_debug_function_names) / sizeof(const char *); i++)
	{
		const char *name = gl_debug_function_names[i];
		void *ptr = GetAnyGLFuncAddress(name);
		gl_debug_function_pointers[i] = ptr;
		if (ptr == NULL)
		{
			trace("Failed to load debug GL func '%s'", name);
			failed++;
		}
	}
#endif

	return failed;
}
