cmake_minimum_required( VERSION 3.12 )

project( Logger VERSION 0.0.0 )

set( CMAKE_CXX_STANDARD_REQUIRED true )

set( CMAKE_CXX_STANDARD 20 )

enable_testing()

set(
	io_functions_test_folder
	tests/io_functions
)

add_executable(
	io_functions_test
	src/tests/io_functions.cpp
	src/io_functions.cpp
)

add_executable(
	basic_logger_test
	src/tests/basic_logger.cpp
	src/logstream.cpp
)

add_test(
	NAME IO_Functions
	COMMAND io_functions_test <input.txt >output.txt
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${io_functions_test_folder}/test_1
)