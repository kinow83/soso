include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(TimeDiffer_unit_test
    test_time_differ.cpp)


target_include_directories(TimeDiffer_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${TimeDiffer_SOURCE_DIR}/include
)

target_link_libraries(TimeDiffer_unit_test
PRIVATE
    gtest_main TimeDiffer
)

include(GoogleTest)
gtest_discover_tests(TimeDiffer_unit_test
  PROPERTIES
    LABELS "unit test"
)