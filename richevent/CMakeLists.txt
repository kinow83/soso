project(richevent)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER /usr/bin/c++)
set(CMAKE_CXX_FLAGS "-m64 -std=c++17")
add_compile_options(-W -Wall -g -fpermissive)

include_directories(include)

find_library(
    libczmq-dev
REQUIRED
)

add_library(richevent SHARED)
target_sources(richevent
PRIVATE
    src/richevent_reader.cpp
    src/richevent_writer.cpp
PUBLIC
    include/richevent_reader.h
    include/richevent_writer.h
)

target_include_directories (richevent
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(richevent
PUBLIC
    czmq
)

add_subdirectory(examples)
