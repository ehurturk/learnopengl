cmake_minimum_required(VERSION 3.11)
project(LearnOpenGL)
set (CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(ASSIMP REQUIRED)

include_directories(${ASSIMP_INCLUDE_DIR})

file(GLOB_RECURSE proj_sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE proj_app_src ${CMAKE_SOURCE_DIR}/apps/*.cpp)
file(GLOB_RECURSE IMGUI_src ${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp)
file(GLOB_RECURSE proj_include ${CMAKE_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE proj_app_include ${CMAKE_SOURCE_DIR}/apps/*.hpp)


set(GLFW_LIB /usr/local/lib/libglfw.3.3.dylib)
# set(YAML_LIB ${CMAKE_SOURCE_DIR}/vendor/yamlcpp/libyaml-cpp.a)
set(TO_BE_LINKED ${OPENGL_LIBRARIES} ${GLFW_LIB} ${ASSIMP_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/apps)
add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/thirdparty/glad.c ${IMGUI_src} ${CMAKE_SOURCE_DIR}/src/thirdparty/stb_image.c ${proj_sources} ${proj_app_src} ${CMAKE_SOURCE_DIR}/src/Window.cpp ${proj_include} ${proj_app_include})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor ${YAML_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/vendor/imgui/ ${proj_include}  ${CMAKE_SOURCE_DIR}/include/thirdparty/stb_image.h ${proj_app_include})
target_link_libraries(${CMAKE_PROJECT_NAME} ${TO_BE_LINKED})
