cmake_minimum_required(VERSION 3.0.0)
project(test VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

set(SOURCES src/main.cpp 
            tests/test_environment_test.cpp
            tests/test_json.cpp)

add_executable(sl_test ${SOURCES})

target_include_directories(sl_test PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(sl_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(sl_test PRIVATE ${CMAKE_SOURCE_DIR}/..)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_test(NAME environment_test COMMAND sl_test --run=environment_test)
add_test(NAME json_test COMMAND sl_test --run=json_test)

