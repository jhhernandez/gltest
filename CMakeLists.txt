cmake_minimum_required(VERSION 3.0)

project(gl)

#set(USE_CLANG TRUE)

set(PROJECT_DIR ${PROJECT_SOURCE_DIR})
set(PROJECT_INCLUDE_DIR ${PROJECT_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_DIR}/src)
set(PROJECT_BINARY_DIR ${PROJECT_DIR}/build)
set(PROJECT_LIB_DIR ${PROJECT_DIR}/lib)

include_directories(${PROJECT_INCLUDE_DIR})

if (USE_CLANG)
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif (USE_CLANG)

list(APPEND CMAKE_CXX_FLAGS "-std=c++20")

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

add_executable(gl main.cpp)

target_link_libraries(gl PRIVATE SDL3::SDL3 GL)

install(TARGETS gl RUNTIME DESTINATION bin)
