cmake_minimum_required (VERSION 3.5.0)

project(WhirlWindWarp)

# Version Number
set (WHIRLWINDWARP_VERSION_MAJOR 1)
set (WHIRLWINDWARP_VERSION_MINOR 0)
set (WHIRLWINDWARP_VERSION_PATCH 0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -Wall -Wno-deprecated -std=c++17 -mconsole -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -Wall -Wno-deprecated -std=c++17 -mwindows -m64")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-I system") # fixes #include_next errors.

find_package(glfw3 REQUIRED)
find_package(TBB REQUIRED)

if(DEFINED MINGW)
  configure_file("${PROJECT_SOURCE_DIR}/resources.rc.in" "${PROJECT_BINARY_DIR}/resources.rc")
  configure_file("${PROJECT_SOURCE_DIR}/version.h.in" "${PROJECT_BINARY_DIR}/version.h")
  set(CORE_SOURCES ${CORE_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/resources.rc)
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff -o <OBJECT> -i <SOURCE>")
  enable_language(RC)
endif(DEFINED MINGW)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}          # Generated .h files
  ${CMAKE_CURRENT_BINARY_DIR}  # For wrap/ui files
  )

set (CORE_SOURCES
  # project files
  ${CORE_SOURCES}
  ${RESOURCES}
  ${CORE_UI}
  Main.cpp
  Particle.cpp
  Utils.cpp
  WhirlWindWarp.cpp
  external/gl_loader.cpp
)

set(CORE_EXTERNAL_LIBS
  glfw3
  opengl32
  TBB::tbb
)
  
add_executable(WhirlWindWarp ${CORE_SOURCES})
target_link_libraries (WhirlWindWarp ${CORE_EXTERNAL_LIBS})
#set_target_properties(WhirlWindWarp PROPERTIES OUTPUT_NAME WhirlWindWarp SUFFIX ".scr")
