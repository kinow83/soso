project(wifimon)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER /usr/bin/c++)
set(CMAKE_CXX_FLAGS "-m64 -std=c++17")
add_compile_options(-W -Wall -g -fpermissive)

include_directories(
    include
    ${component_SOURCE_DIR}/include
    ${echo_SOURCE_DIR}/include
    ${worker_SOURCE_DIR}/include
    ${chronos_SOURCE_DIR}/include
    components)

add_executable(wifimon
    src/wifimon.cpp
    src/component80211.cpp
    src/pcapmon.cpp
    src/wifiutils.cpp)

target_link_libraries(wifimon
PUBLIC
    component echo worker chronos
    fmt pcap pthread event
)

###############################
# for doxygen
###############################
option(BUILD_DOC "Build documentation" ON)
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    configure_file(${DOXYGEN_OUT} @ONLY)
    add_custom_target(
        wifimon_doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

