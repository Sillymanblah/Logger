cmake_minimum_required( VERSION 3.10 )
project( Logger )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED true )

add_executable(
	logstream_test
	src/tests/logstream_test.cpp
)