include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(gtest)


# testcase #1
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

# testcase #2
add_executable(mysql_unit_test
    test_mysql.cpp)

target_include_directories(mysql_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${worker_SOURCE_DIR}/include
    ${kairos_SOURCE_DIR}/include
)

target_link_libraries(mysql_unit_test
PRIVATE
    worker
    pthread
    mysqlcppconn
    gtest_main
    kairos
)

# testcase #3
add_executable(cv_unit_test
    test_cv.cpp)

target_include_directories(cv_unit_test
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${worker_SOURCE_DIR}/include
)

target_link_libraries(cv_unit_test
PRIVATE
    worker
    pthread
    gtest_main
)

include(GoogleTest)
#gtest_discover_tests(worker_unit_test
#  PROPERTIES
#    LABELS "unit test"
#)
