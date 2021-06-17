include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(echo_unit_test
    test_echo.cpp)

target_include_directories(echo_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${Echo_SOURCE_DIR}/include
)

target_link_libraries(echo_unit_test
PRIVATE
    echo
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(echo_unit_test
  PROPERTIES
    LABELS "unit test"
)
