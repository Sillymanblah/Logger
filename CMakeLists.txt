cmake_minimum_required( VERSION 3.10 )
project( Logger )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED true )

add_library(
	logger STATIC
	src/log_base.cpp
	src/log_settings.cpp
	src/time_fix.cpp
)

add_executable(
	logstream_test
	src/tests/logstream_test.cpp
)

target_link_libraries(
	logstream_test
	logger
)