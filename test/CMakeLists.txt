cmake_minimum_required(VERSION 3.0.0)
project(test VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

set(SOURCES src/main.cpp
            tests/test_ptr.cpp
            tests/test_tools.cpp
            tests/test_environment_test.cpp
            tests/test_simple_object.cpp
            tests/test_user_functions.cpp)

add_executable(sl_test ${SOURCES})

target_include_directories(sl_test PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(sl_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(sl_test PRIVATE ${CMAKE_SOURCE_DIR}/..)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_test(NAME "environment_test.operations" COMMAND sl_test --run=environment_test/operations)
add_test(NAME "environment_test.simple_containers" COMMAND sl_test --run=environment_test/simple_containers)
add_test(NAME "environment_test.composed_containers" COMMAND sl_test --run=environment_test/composed_containers)
add_test(NAME "environment_test.simple_tuple" COMMAND sl_test --run=environment_test/simple_tuple)
add_test(NAME "tools_test.test_zip_vector" COMMAND sl_test --run=tools_test/test_zip_vector)
add_test(NAME "tools_test.test_zip_map" COMMAND sl_test --run=tools_test/test_zip_map)
add_test(NAME "tools_test.test_zip_vector_map" COMMAND sl_test --run=tools_test/test_zip_vector_map)
add_test(NAME "simple_tests.test_object_with_simple_elements" COMMAND sl_test --run=simple_tests/test_object_with_simple_elements)
add_test(NAME "simple_tests.test_object_with_std_elements" COMMAND sl_test --run=simple_tests/test_object_with_std_elements)
add_test(NAME "test_user_functions.test_serialize_function" COMMAND sl_test --run=test_user_functions/test_serialize_function)
add_test(NAME "test_user_functions.test_deserialize_function" COMMAND sl_test --run=test_user_functions/test_deserialize_function)
add_test(NAME "test_user_functions.test_serialize_and_deserialize_function" COMMAND sl_test --run=test_user_functions/test_serialize_and_deserialize_function)
add_test(NAME "test_user_functions.test_with_version_function" COMMAND sl_test --run=test_user_functions/test_with_version_function)
add_test(NAME "test_user_functions.test_deserialize_with_version_function" COMMAND sl_test --run=test_user_functions/test_deserialize_with_version_function)
add_test(NAME "test_ptr.simple_double_ptr" COMMAND sl_test --run=test_ptr/simple_double_ptr)
add_test(NAME "test_ptr.simple_double_ptr_multiple_ptr" COMMAND sl_test --run=test_ptr/simple_double_ptr_multiple_ptr)
add_test(NAME "test_ptr.simple_double_ptr_array" COMMAND sl_test --run=test_ptr/simple_double_ptr_array)
add_test(NAME "test_ptr.simple_double_ptr_multiple_ptr_array" COMMAND sl_test --run=test_ptr/simple_double_ptr_multiple_ptr_array)
