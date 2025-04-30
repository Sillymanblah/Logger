#include "../logging.hpp"

int main()
{
	logstream logger( "test.txt", logger_base::level::ALL_LEVELS, logger_base::setting::NONE_SET );

	logger << logger_base::level::DEBUG << "This is a debug print!";
	logger << logger_base::level::TRACE << "This is a trace print!";
	logger << logger_base::level::INFO << "This is an info print!";
	logger << logger_base::level::WARN << "This is a warning print!";
	logger << logger_base::level::ERROR << "This is an error print!";
	logger << logger_base::level::FATAL << "This is a fatal error print!";
}