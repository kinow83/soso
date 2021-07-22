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
    src/richevent.cpp
PUBLIC
    include/richevent.h
)
target_include_directories (richevent
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(richevent
PUBLIC
    czmq
)

# for doxygen
option(BUILD_DOC "Build documentation" ON)
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    configure_file(${DOXYGEN_OUT} @ONLY)
    add_custom_target(
	    richevent_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

add_subdirectory(examples)
