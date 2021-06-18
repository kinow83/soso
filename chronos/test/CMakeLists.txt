include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(chrono_unit_test
    test_chrono.cpp)


target_include_directories(chrono_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${chrono_SOURCE_DIR}/include
)

target_link_libraries(chrono_unit_test
PRIVATE
    gtest_main
    chronos
)

include(GoogleTest)
gtest_discover_tests(chrono_unit_test
  PROPERTIES
    LABELS "unit test"
)