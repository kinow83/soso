include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(kairos_unit_test
    test_kairos.cpp)


target_include_directories(kairos_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${kairos_SOURCE_DIR}/include
)

target_link_libraries(kairos_unit_test
PRIVATE
    gtest_main
    kairos
)

include(GoogleTest)
gtest_discover_tests(kairos_unit_test
  PROPERTIES
    LABELS "unit test"
)