#include "../logstream.hpp"
#include <iostream>
#include <ostream>

void test_logger_crash()
{
	logstream logger( "test_two.log", logger_base::level::ALL_LEVELS, logger_base::setting::PRINT_CRASH );

	logger << logger_base::level::INFO << "Testing if the logger can recognize a crash!";

	throw std::runtime_error( "This is a test exception!" );
}

int main()
{
	logstream logger( "test.log", logger_base::level::ALL_LEVELS, logger_base::setting::NONE_SET );

	logger << logger_base::level::DEBUG << "This is a debug print!";
	logger << logger_base::level::TRACE << "This is a trace print!";
	logger << logger_base::level::INFO << "This is an info print!";
	logger << logger_base::level::WARN << "This is a warning print!";
	logger << logger_base::level::ERROR << "This is an error print!";
	logger << logger_base::level::FATAL << "This is a fatal error print!";

	try { test_logger_crash(); }
	catch ( std::exception& e ) { std::cerr << "Crashed logger with exception:\n\t" << e.what() << std::endl; }
}