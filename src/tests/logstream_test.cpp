#include "../logstream.hpp"
#include <iostream>

int main()
{
	constexpr size_t thread_count = 10;

	logstream my_logger( std::cerr.rdbuf(), developer_settings::none, system_settings::warn );

	std::cout << "Testing logging levels with the settings:\nDEV: NONE\nSYS: WARNINGS" << std::endl;

	my_logger << logging_level::trace << "This is a TRACE log...\n";
	my_logger << logging_level::debug << "This is a DEBUG log...\n";
	my_logger << logging_level::info << "This is an INFO log...\n";
	my_logger << logging_level::warn << "This is a WARN log...\n";
	my_logger << logging_level::error << "This is an ERROR log...\n";
	my_logger << logging_level::fatal << "This is a FATAL log...\n";

	my_logger << logging_level::trace << "Starting " << thread_count << " threads to test with multithreading..." << std::flush;

	return 0;
}