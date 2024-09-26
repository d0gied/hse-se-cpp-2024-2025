cmake_minimum_required(VERSION 3.10)

# Set cpp version
set(CMAKE_CXX_STANDARD 20)

# Set the project name
project(A)

# Add an executable
add_executable(A A.cpp testA.cpp)