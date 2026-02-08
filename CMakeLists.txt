cmake_minimum_required(VERSION 3.14)
project(myproject)

# C++20 for modules
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILE_FEATURES "-O0 -g -Wall -Wextra -Wswitch")



file(GLOB_RECURSE APP_SOURCES
    CONFIGURE_DEPENDS
    src/io/*.cpp
    src/*.cpp
)

add_executable(my_app ${APP_SOURCES})

