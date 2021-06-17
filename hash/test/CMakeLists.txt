include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(hash_unit_test
    test_hash.cpp)

target_include_directories(hash_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${hash_SOURCE_DIR}/include
)

target_link_libraries(hash_unit_test
PRIVATE
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(hash_unit_test
  PROPERTIES
    LABELS "unit test"
)
