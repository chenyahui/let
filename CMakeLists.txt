cmake_minimum_required(VERSION 3.10)
project(let)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(let)

include_directories(./)
add_executable(let_test main.cpp)
target_link_libraries(let_test let)