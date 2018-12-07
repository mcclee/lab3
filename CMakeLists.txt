cmake_minimum_required(VERSION 3.12)
project(test)

set(CMAKE_CXX_STANDARD 14)

set(GLEW_H C:\\Program Files \(x86\)\\Windows Kits\\10\\Include\\10.0.17134.0\\um/GL)
set(HEAD C:\\code\\sourcepkg\\include)
set(GLM_H C:\\code\\sourcepkg\\glm)
include_directories(${GLEW_H} ${HEAD} ${GLM_H})

set(GLFW_LINK C:/code/sourcepkg/LIB/libglfw3.a)
link_libraries(${OPENGL} ${GLFW_LINK})

set(SOURCE_FILES main.cpp Obj_Loader.hpp ShaderLoader.h stb_image.h)
add_executable(test ${SOURCE_FILES} main.cpp glad.c ShaderLoader.cpp)

