cmake_minimum_required(VERSION 3.17)

project(Vreeman2002 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif(MSVC)

add_executable(test_temperatures test_temperatures.cpp)
add_test(NAME test_temperatures COMMAND test_temperatures)