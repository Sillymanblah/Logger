// Direct
#include "../log_stream.hpp"
#include "../log_types.hpp"

// STL
#include <iostream>

constexpr const char* number_suffix( size_t number )
{
	switch ( number )
	{
	case 1: return "st";
	case 2: return "nd";
	case 3: return "rd";
	default: return "th";
	}
}

void do_random_logging( size_t thread_index, logstream& logger )
{
	constexpr size_t num_logs = 15;

	std::srand( thread_index * 151 );

	for ( size_t index = 1; index <= num_logs; ++index )
	{
		int level_int = std::rand() % static_cast< int >( logging_level::fatal );

		logging_level level = static_cast< logging_level >( level_int );

		std::this_thread::sleep_for( std::chrono::milliseconds( rand() % 100 ) );

		logger << level << "Logging the " << index << number_suffix( index ) << " log from thread #" << thread_index << " with level " << level_int << std::endl;
	}
}

int main()
{
	constexpr size_t thread_count = 10;

	logstream my_logger( std::cerr.rdbuf(), developer_settings::trace, system_settings::warn );

	std::cout << "Testing logging levels with the settings:\nDEV: TRACE\nSYS: WARNINGS" << std::endl;

	my_logger << logging_level::trace << "This is a TRACE log..." << std::endl;
	my_logger << logging_level::debug << "This is a DEBUG log..." << std::endl;
	my_logger << logging_level::info << "This is an INFO log..." << std::endl;
	my_logger << logging_level::warn << "This is a WARN log..." << std::endl;
	my_logger << logging_level::error << "This is an ERROR log..." << std::endl;
	my_logger << logging_level::fatal << "This is a FATAL log..." << std::endl;

	my_logger << logging_level::trace << "Starting " << thread_count << " threads to test with multithreading..." << std::endl;

	std::thread threads[ thread_count ];

	for ( size_t thread_num = 0; thread_num < thread_count; ++thread_num )
	{
		threads[ thread_num ] = std::thread( do_random_logging, thread_num, std::ref( my_logger ) );
	}

	for ( std::thread& thread : threads ) thread.join();

	return 0;
}