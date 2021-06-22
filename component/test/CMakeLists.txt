include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(component_unit_test
    test_component.cpp)


target_include_directories(component_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${component_SOURCE_DIR}/include
)

target_link_libraries(component_unit_test
PRIVATE
    gtest_main component
)

#include(GoogleTest)
#gtest_discover_tests(component_unit_test
#  PROPERTIES
#    LABELS "unit test"
#)