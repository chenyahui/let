cmake_minimum_required(VERSION 3.10)
project(let)

set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS
        -g
        -DCHECK_PTHREAD_RETURN_VALUE
        -D_FILE_OFFSET_BITS=64
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wno-unused-parameter
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        -march=native
        -rdynamic
        )
        
add_subdirectory(let)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(examples)

include_directories(./libevent)