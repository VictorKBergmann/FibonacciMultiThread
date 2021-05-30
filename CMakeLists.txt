cmake_minimum_required(VERSION 3.19)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(untitled fibonacci.cpp library.cpp library.h)