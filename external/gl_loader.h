/*
 File: gl_loader.h
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

// C++
#include <GL/gl.h>
#include <GL/glext.h>

/** \brief OpenGL function pointers or nullprt if unable to load.ABC
 *
 */
extern void* gl_function_pointers[]; 

/** \brief Loads the GL functions and returns the number of functions that failed to load. 
 *
 */ 
int load_gl_functions(); 
 
// GL functions definitions.
#define glCompileShader ((PFNGLCOMPILESHADERPROC)gl_function_pointers[0])
#define glGetShaderiv ((PFNGLGETSHADERIVPROC)gl_function_pointers[1])
#define glGetShaderInfoLog ((PFNGLGETSHADERINFOLOGPROC)gl_function_pointers[2])
#define glCreateProgram ((PFNGLCREATEPROGRAMPROC)gl_function_pointers[3])
#define glCreateShader ((PFNGLCREATESHADERPROC)gl_function_pointers[4])
#define glShaderSource ((PFNGLSHADERSOURCEPROC)gl_function_pointers[5])
#define glDeleteShader ((PFNGLDELETESHADERPROC)gl_function_pointers[6])
#define glGetProgramiv ((PFNGLGETPROGRAMIVPROC)gl_function_pointers[7])
#define glAttachShader ((PFNGLATTACHSHADERPROC)gl_function_pointers[8])
#define glLinkProgram ((PFNGLLINKPROGRAMPROC)gl_function_pointers[9])
#define glDeleteProgram ((PFNGLDELETEPROGRAMPROC)gl_function_pointers[10])
#define glUseProgram ((PFNGLUSEPROGRAMPROC)gl_function_pointers[11])
#define glGetUniformLocation ((PFNGLGETUNIFORMLOCATIONPROC)gl_function_pointers[12])
#define glGenVertexArrays ((PFNGLGENVERTEXARRAYSPROC)gl_function_pointers[13])
#define glBindVertexArray ((PFNGLBINDVERTEXARRAYPROC)gl_function_pointers[14])
#define glEnableVertexAttribArray ((PFNGLENABLEVERTEXATTRIBARRAYPROC)gl_function_pointers[15])
#define glGenBuffers ((PFNGLGENBUFFERSPROC)gl_function_pointers[16])
#define glBindBuffer ((PFNGLBINDBUFFERPROC)gl_function_pointers[17])
#define glBufferData ((PFNGLBUFFERDATAPROC)gl_function_pointers[18])
#define glVertexAttribPointer ((PFNGLVERTEXATTRIBPOINTERPROC)gl_function_pointers[19])
#define glDeleteBuffers ((PFNGLDELETEBUFFERSPROC)gl_function_pointers[20])
#define glDeleteVertexArrays ((PFNGLDELETEVERTEXARRAYSPROC)gl_function_pointers[21])
#define glUniform1f ((PFNGLUNIFORM1FPROC)gl_function_pointers[22])
#define glGenFramebuffers ((PFNGLGENFRAMEBUFFERSPROC)gl_function_pointers[23])
#define glFramebufferTexture2D ((PFNGLFRAMEBUFFERTEXTURE2DPROC)gl_function_pointers[24])
#define glCheckFramebufferStatus ((PFNGLCHECKFRAMEBUFFERSTATUSPROC)gl_function_pointers[25])
#define glBindFramebuffer ((PFNGLBINDFRAMEBUFFERPROC)gl_function_pointers[26])
#define glDeleteFramebuffers ((PFNGLDELETEFRAMEBUFFERSPROC)gl_function_pointers[27])
#define glBindAttribLocation ((PFNGLBINDATTRIBLOCATIONPROC)gl_function_pointers[28])
#define glGetProgramInfoLog ((PFNGLGETPROGRAMINFOLOGPROC)gl_function_pointers[29])
#define glBlendEquation ((PFNGLBLENDEQUATIONPROC)gl_function_pointers[30])
#define glBlendColor ((PFNGLBLENDCOLORPROC)gl_function_pointers[31])
#define glUniform1fv ((PFNGLUNIFORM1FVPROC)gl_function_pointers[32])

// GL debug function definitions.
#ifdef DEBUG
extern void* gl_debug_function_pointers[]; 

#define glGetTextureParameteriv ((PFNGLGETTEXTUREPARAMETERIVPROC)gl_debug_function_pointers[0]) 
#define glDetachShader ((PFNGLDETACHSHADERPROC)gl_debug_function_pointers[1]) 
#define glGenQueries ((PFNGLGENQUERIESPROC)gl_debug_function_pointers[2]) 
#define glEndQuery ((PFNGLENDQUERYPROC)gl_debug_function_pointers[3]) 
#define glBeginQuery ((PFNGLBEGINQUERYPROC)gl_debug_function_pointers[4]) 
#define glGetProgramiv ((PFNGLGETPROGRAMIVPROC)gl_debug_function_pointers[5]) 
#define glMapBuffer ((PFNGLMAPBUFFERPROC)gl_debug_function_pointers[6]) 
#define glUnmapBuffer ((PFNGLUNMAPBUFFERPROC)gl_debug_function_pointers[7]) 
#endif
