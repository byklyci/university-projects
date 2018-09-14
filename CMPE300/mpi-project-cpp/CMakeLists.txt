cmake_minimum_required(VERSION 3.9)
project(mpi_project_cpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(mpi_project_cpp main.cpp)
include_directories(/usr/include/mpi/)
