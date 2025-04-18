/*
 File: Main.cpp
 Created on: 02/04/2017
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
#include <WhirlWindWarp.h>
#include <version.h>
#include <Utils.h>
#include <Shaders.h>
#include <Particle.h>
#include <resources.h>

// GLFW
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// C++
#include <GL/gl.h>
#include <GL/glext.h>
#include <external/gl_loader.h>
#define WIN32_LEAN_AND_MEAN
#include <winnt.h>
#include <windef.h>
#include <winuser.h>
#include <iostream>
#include <scrnsave.h>
#include <winuser.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

enum class Mode: char { SAVER = 0, CHILD = 1, CONFIG = 2 };
static Mode g_mode = Mode::CONFIG;

//---------------------------------------------------------------------------------------
LRESULT WINAPI ScreenSaverProc (HWND hwnd, UINT iMsg, WPARAM wparam, LPARAM lparam)
{
    return DefWindowProc (hwnd, iMsg, wparam, lparam);
}

//---------------------------------------------------------------------------------------
void ScreenSaver()
{
    int virtualWidth = 0;
    int virtualHeight = 0;
    int xMin = 0;
    int yMin = 0;

    constexpr GLsizei stride = sizeof(Particle);

    Utils::Configuration config;
    Utils::loadConfiguration(config);

    glfwSetErrorCallback(Utils::errorCallback);

    if (!glfwInit()) {
       Utils::errorCallback(EXIT_FAILURE, "Failed to initialize GLFW");
    }

    int monitorCount = 0;
    const auto glfwmonitors = glfwGetMonitors(&monitorCount);
    if (!glfwmonitors) {
        glfwTerminate();
        Utils::errorCallback(EXIT_FAILURE, "No monitors detected");
    }

    xMin = std::numeric_limits<int>::max();
    yMin = std::numeric_limits<int>::max();

    for (int i = 0; i < monitorCount; ++i) {
        const auto glfwmonitor = glfwmonitors[i];
        int xPos, yPos;
        glfwGetMonitorPos(glfwmonitor, &xPos, &yPos);
        const auto res = glfwGetVideoMode(glfwmonitor);

        xMin = std::min(xMin, xPos);
        yMin = std::min(yMin, yPos);
        virtualWidth = std::max(virtualWidth, xPos + res->width);
        virtualHeight = std::max(virtualHeight, yPos + res->height);
    }

    const int numPoints = (virtualWidth * virtualHeight) / config.pixelsPerPoint;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, config.antialias ? 4 : 1);
    glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
    glfwWindowHint(GLFW_POSITION_X, xMin);
    glfwWindowHint(GLFW_POSITION_Y, yMin);

    WhirlWindWarp www(numPoints, config);
    const float* vertices = www.buffer();

    GLFWwindow* window = glfwCreateWindow(virtualWidth, virtualHeight, "Monitor", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        const std::string msg = std::string("Failed to create GLFW window for monitor");
        Utils::errorCallback(EXIT_FAILURE, msg.c_str());
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, Utils::glfwKeyCallback);
    glfwSetWindowFocusCallback(window, Utils::glfwFocusCallback);
    glfwSetCursorPosCallback(window, Utils::glfwMousePosCallback);
    glfwSetMouseButtonCallback(window, Utils::glfwMouseButtonCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetWindowOpacity(window, 1.f);
    glfwSwapInterval(1);

    if (load_gl_functions() > 0) {
        glfwTerminate();
        Utils::errorCallback(EXIT_FAILURE, "Failed to load OpenGL functions");
    }

    // std::cout << "GL Vendor: " << glGetString(GL_VENDOR) << '\n';
    // std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    // std::cout << "Version: " << glGetString(GL_VERSION) << '\n';
    // std::cout << "Shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glfwMakeContextCurrent(window);

    Utils::GL_program points = Utils::GL_program("default");
    points.vert = Utils::loadShader(vertexShaderSource, GL_VERTEX_SHADER);
    points.frag = Utils::loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    Utils::initProgram(points);

    Utils::GL_program trails = Utils::GL_program("trails");
    trails.vert = Utils::loadShader(vertexShaderSourceTrails, GL_VERTEX_SHADER);
    trails.geom = Utils::loadShader(geometryShaderSource, GL_GEOMETRY_SHADER);
    trails.frag = Utils::loadShader(fragmentShaderSourceTrails, GL_FRAGMENT_SHADER);

    Utils::initProgram(trails);

    const GLint uratioX = glGetUniformLocation(trails.program, "ratioX");
    const GLint uratioY = glGetUniformLocation(trails.program, "ratioY");

    Utils::GL_program post = Utils::GL_program("post-processing");
    post.vert = Utils::loadShader(ppVertexShaderSource, GL_VERTEX_SHADER);
    post.frag = Utils::loadShader(ppFragmentShaderSource, GL_FRAGMENT_SHADER);

    Utils::initProgram(post);

    // Create VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    const int multiplier = config.show_trails ? 2 : 1;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, multiplier * numPoints * stride, vertices, GL_DYNAMIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Line/Point width
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Create VAO and VBOs for post-processing quad
    GLuint quadVAO, quadVBO, quadEBO;    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup framebuffer and texture to accumulate colors
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, virtualWidth, virtualHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glfwTerminate();
        Utils::errorCallback(EXIT_FAILURE, "Framebuffer not complete!");
    }

    // openg coords are {-1,1} get ratio coords/pixels to pass it as uniforms in the line shaders.
    float ratioX = 2.f / virtualWidth;
    float ratioY = 2.f / virtualHeight;

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, virtualWidth, virtualHeight, 0.0f, 0.0f, 1.0f);

    while(!glfwWindowShouldClose(window)) {
        glfwFocusWindow(window);
        www.advance();

        glBindFramebuffer(GL_FRAMEBUFFER, (config.motion_blur ? framebuffer : 0));
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        if (config.antialias) {
            glEnable(GL_MULTISAMPLE);
        }

        if (config.show_trails) {
            glUseProgram(trails.program);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, multiplier * numPoints * stride, vertices, GL_DYNAMIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
            glEnableVertexAttribArray(0);

            // Color attribute
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Line/Point width
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glUniform1fv(uratioX, 1, &ratioX);
            glUniform1fv(uratioY, 1, &ratioY);

            glDrawArrays(GL_LINES, 0, numPoints * multiplier);
        }

        glUseProgram(points.program);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, multiplier * numPoints * stride, vertices, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride * multiplier, (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride * multiplier, (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Line/Point width
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride * multiplier, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glDrawArrays(GL_POINTS, 0, numPoints);

        if (config.motion_blur) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
            glBlendColor(1.f, 1.f, 1.f, 0.75f);
            glBlendEquation(GL_FUNC_ADD);

            glUseProgram(post.program);

            glBindVertexArray(quadVAO);

            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);

            // Position attribute
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindTexture(GL_TEXTURE_2D, texture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glDisable(GL_BLEND);
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glBindVertexArray(0);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(points.program);
    glDeleteProgram(trails.program);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteProgram(post.program);

    glfwDestroyWindow(window);
    glfwTerminate();

    PostQuitMessage(0);
}

//---------------------------------------------------------------------------------------
BOOL CALLBACK ConfigureDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR PointSizes[3][2] =  
    {
        TEXT("1"), TEXT("2"), TEXT("3")
    };

    TCHAR PointDensity[3][7] =  
    {
        TEXT("Low"), TEXT("Medium"), TEXT("High")
    };

    TCHAR comboBuffer[16];

    static HWND hCheckAlias;
    static HWND hCheckBlur;
    static HWND hCheckTrails;
    static HWND hComboPointSize;
    static HWND hComboPointDensity;
    static Utils::Configuration config;
  
    switch(message) 
    { 
        case WM_INITDIALOG: 
        {
            Utils::loadConfiguration(config);
 
            // Initialize the redraw speed scroll bar control.
            hCheckAlias = GetDlgItem(hwnd, IDC_CHECKBOX1);
            hCheckBlur = GetDlgItem(hwnd, IDC_CHECKBOX2);
            hCheckTrails = GetDlgItem(hwnd, IDC_CHECKBOX3);
            hComboPointSize = GetDlgItem(hwnd, IDC_COMBO1);
            hComboPointDensity = GetDlgItem(hwnd, IDC_COMBO2);

            SendMessage(hCheckAlias, BM_SETCHECK, config.antialias ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(hCheckBlur, BM_SETCHECK, config.motion_blur ? BST_CHECKED: BST_UNCHECKED, 0);
            SendMessage(hCheckTrails, BM_SETCHECK, config.show_trails ? BST_CHECKED: BST_UNCHECKED, 0);

            for(int i = 0; i < 3; ++i)
            {
                memset(&comboBuffer,0,sizeof(comboBuffer)); 
                wcscpy_s(comboBuffer, sizeof(comboBuffer)/sizeof(TCHAR),  (TCHAR*)PointSizes[i]);
                SendMessage(hComboPointSize,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)comboBuffer); 
            }

            for(int i = 0; i < 3; ++i)
            {
                memset(&comboBuffer,0,sizeof(comboBuffer)); 
                wcscpy_s(comboBuffer, sizeof(comboBuffer)/sizeof(TCHAR),  (TCHAR*)PointDensity[i]);
                SendMessage(hComboPointDensity,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)comboBuffer); 
            }            

            // Send the CB_SETCURSEL message to display an initial item in the selection field  
            const int pSize = config.point_size - 1;
            SendMessage(hComboPointSize, CB_SETCURSEL, (WPARAM)pSize, 0);
            const int pDensity = config.pixelsPerPoint == 2000 ? 0 : (config.pixelsPerPoint == 1000 ? 1 : 2);
            SendMessage(hComboPointDensity, CB_SETCURSEL, (WPARAM)pDensity, 0);
            
            return TRUE; 
        }
        break;
        case WM_COMMAND: 
            switch(LOWORD(wParam)) 
            { 
                case IDC_OK: 
                {
                    const int pSize = ::SendMessage(hComboPointSize, CB_GETCURSEL, 0, 0);
                    const int pDensity = ::SendMessage(hComboPointDensity, CB_GETCURSEL, 0, 0);

                    //config.point_size = ::SendMessage(hComboPointSize, CB_GETCURSEL, nullptr, nullptr);
                    const auto aliasCheck = ::SendMessage(hCheckAlias, BM_GETCHECK, 0, 0);
                    const auto blurCheck = ::SendMessage(hCheckBlur, BM_GETCHECK, 0, 0);
                    const auto trailsCheck = ::SendMessage(hCheckTrails, BM_GETCHECK, 0, 0);

                    config.point_size = pSize + 1;
                    config.pixelsPerPoint = pDensity == 0 ? 2000 : (pDensity == 1 ? 1000 : 500);
                    config.antialias = aliasCheck;
                    config.motion_blur = blurCheck;
                    config.show_trails = trailsCheck;

                    Utils::saveConfiguration(config);
                    EndDialog(hwnd, IDOK); 
                    return TRUE; 
                }
                case IDCANCEL: 
                    EndDialog(hwnd, IDCANCEL); 
                    return TRUE; 
            } 
            break;
        case WM_DESTROY:
        case WM_CLOSE:
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
        default:
            return FALSE;            
    } 

    return TRUE; 
}

//---------------------------------------------------------------------------------------
BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int)
{
    try {
        LPCTSTR cmdline = lpCmdLine;

        bool finished = false;
        while (*cmdline != '\0' && !finished) {
            switch (*cmdline) {
                case TEXT(' '):
                case TEXT('-'):
                case TEXT('/'):
                    cmdline++;
                    break;

                case TEXT('p'): // Show preview in window (handle in cmd)
                case TEXT('P'):
                {
                    g_mode = Mode::CHILD;
                    finished = true;
                    break;
                }
                case TEXT('s'): // Start in fullscreen
                case TEXT('S'):
                    g_mode = Mode::SAVER;
                    finished = true;
                    break;

                case TEXT('c'): // Show configuration dialog
                case TEXT('C'):
                    g_mode = Mode::CONFIG;
                    finished = true;
                    break;

                default:
                    break;
            }
        }

        switch(g_mode)
        {
            case Mode::CHILD:
            case Mode::SAVER:
                ScreenSaver();
                break;
            default:
            case Mode::CONFIG:
                DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_CONFIGURE), NULL, (DLGPROC)ConfigureDialogProc);
                break;
        }
    } catch (const std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

