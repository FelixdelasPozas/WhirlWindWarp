#
# WhirlWindWarp CMake configuration.
#
project(WhirlWindWarp)

cmake_minimum_required (VERSION 2.8.6)

cmake_policy(SET CMP0020 OLD)
cmake_policy(SET CMP0043 OLD)

# Version Number
set (WHIRLWINDWARP_VERSION_MAJOR 1)
set (WHIRLWINDWARP_VERSION_MINOR 0)
set (WHIRLWINDWARP_VERSION_PATCH 0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Widgets)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated -std=c++11 -mwindows -m64")

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(CORE_EXTERNAL_LIBS ${CORE_EXTERNAL_LIBS} ${QT_QTTEST_LIBRARY})
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if(DEFINED MINGW)
  configure_file("${PROJECT_SOURCE_DIR}/resources.rc.in" "${PROJECT_BINARY_DIR}/resources.rc")
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
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Multimedia_INCLUDE_DIRS}
  )

# Add Qt Resource files
qt5_add_resources(RESOURCES
  rsc/resources.qrc
)

qt5_wrap_ui(CORE_UI
  # .ui for Qt
)
	
set (CORE_SOURCES
  # project files
  ${CORE_SOURCES}
  ${RESOURCES}
  ${CORE_UI}
  Main.cpp
  WhirlWindWarp.cpp
  Particle.cpp
  State.cpp
)

set(CORE_EXTERNAL_LIBS
  Qt5::Widgets
)
  
add_executable(WhirlWindWarp ${CORE_SOURCES})
target_link_libraries (WhirlWindWarp ${CORE_EXTERNAL_LIBS})
qt5_use_modules(WhirlWindWarp Widgets)
