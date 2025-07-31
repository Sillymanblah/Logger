#include "../logstream.hpp"
#include <iostream>

void do_random_logging( size_t thread_index, logstream& logger )
{
	constexpr size_t num_logs = 15;

	std::srand( thread_index * 151 );

	std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

	for ( size_t index = 1; index <= num_logs; ++index )
	{
		int level_int = std::rand() % static_cast< int >( logging_level::fatal );

		logging_level level = static_cast< logging_level >( level_int );

		auto& my_log = logger << level << "Logging the " << index;
		
		switch ( index )
		{
		case 1: my_log << "st"; break;
		case 2: my_log << "nd"; break;
		case 3: my_log << "rd"; break;
		default: my_log << "th"; break;
		}

		my_log << " log from thread #" << thread_index << " with level " << level_int << std::endl;
	}
}

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

	std::thread threads[ thread_count ];

	for ( size_t thread_num = 0; thread_num < thread_count; ++thread_num )
	{
		threads[ thread_num ] = std::thread( do_random_logging, thread_num, std::ref( my_logger ) );
	}

	for ( std::thread& thread : threads ) thread.join();

	return 0;
}