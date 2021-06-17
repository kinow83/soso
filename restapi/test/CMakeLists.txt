include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(restapi_unit_test
    test_restapi.cpp)

target_include_directories(restapi_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${restapi_SOURCE_DIR}/include
)

target_link_libraries(restapi_unit_test
PRIVATE
  restapi
  gtest_main
)

#include(GoogleTest)
#gtest_discover_tests(restapi_unit_test
#  PROPERTIES
#    LABELS "unit test"
#)
