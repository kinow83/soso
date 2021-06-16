include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(worker_unit_test
    test_worker.cpp)

target_include_directories(worker_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${worker_SOURCE_DIR}/include
)

target_link_libraries(worker_unit_test
PRIVATE
    worker
    pthread
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(worker_unit_test
  PROPERTIES
    LABELS "unit test"
)
