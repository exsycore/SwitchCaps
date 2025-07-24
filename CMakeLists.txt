cmake_minimum_required(VERSION 3.19)
project(SwitchCaps LANGUAGES CXX VERSION 1.0)

set(CMAKE_WIN32_EXECUTABLE TRUE)

set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)

add_executable(${PROJECT_NAME} 
    Source/main.cpp
    resource.rc
)

target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")