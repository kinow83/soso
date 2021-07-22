project(rich_example)

set(ORG_BUILD_TESTING ${BUILD_TESTING})
set(BUILD_TESTING OFF)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER /usr/bin/c++)
set(CMAKE_CXX_FLAGS "-m64 -std=c++17")
add_compile_options(-W -Wall -g -fpermissive)

include_directories(include)

add_executable(test_richevent
    test_richevent.cpp
)
target_link_libraries(test_richevent
PUBLIC
    richevent
)

add_executable(test_richpush
test_richpush.cpp
)
target_link_libraries(test_richpush
PUBLIC
    richevent
)
