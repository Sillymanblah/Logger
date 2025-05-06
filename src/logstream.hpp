#pragma once
#ifndef LOGSTREAM_HPP
#define LOGSTREAM_HPP

// Ours
#include "logger_core.hpp"

// STL
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include <format>

template < class _Elem >
class basic_logstream;

class logger_base
{
public:
	// Captures logging levels for settings and for calls to the `log` function
	enum level : BYTE
	{
		DEBUG	= 1 << 0,	// Marks debug logs.
		TRACE	= 1 << 1,	// Marks debug logs.
		INFO	= 1 << 2,	// Marks information logs.
		WARN	= 1 << 3,	// Marks warning logs.
		ERROR	= 1 << 4,	// Marks error logs.
		FATAL	= 1 << 5,	// Marks fatal error logs.
	
		NO_LEVELS	= 0,							// Sets no log levels as active, this is not useful unless you need to disable logging conditionally or temporarily.
		PROGRAM		= DEBUG | TRACE,				// Sets program debug and tracing logs as active, opposite of `DEFAULT`.
		DEFAULT		= INFO | WARN | ERROR | FATAL,	// Sets standard log levels active, opposite of `PROGRAM`.
		ISSUES		= WARN | ERROR | FATAL,			// Sets issue log levels as active (warnings, errors, and fatal errors).
		DANGERS		= ERROR | FATAL,				// Sets only critical logs to active.
		ALL_LEVELS	= PROGRAM | DEFAULT,			// Sets all log levels to active.
	};
	// enum class level : BYTE

	static constexpr uint8_t level_count = 6;

	// Logger settings to determine logger behavior.
	enum setting : BYTE
	{
		PRINT_CRASH	= 1 << 0,	// When the logger is destructed, print any active exceptions as fatal errors. NOTE: Might not work for static objects.
		MOVE_DATA	= 1 << 1,	// When the logger moves files, it will copy over all data in the current file.
	
		NONE_SET	= 0,						// No settings enabled.
		ALL_SET		= PRINT_CRASH | MOVE_DATA,	// Both settings are enabled.
	};
	// enum class setting : BYTE

	static constexpr uint8_t setting_count = 2;

protected:
	// Verifies that the log level settings are valid, can be any combination of the log levels.
	// Returns the same level settings if valid, otherwise throws an exception.
	static const level& LOGGER_DLL_API LOGGER_DLL_CALL verify_level_settings( const level& level_settings );
	
	// Verifies that the log settings are valid, can be any combination of the log settings.
	// Returns the same logger settings if valid, otherwise throws an exception.
	static const setting& LOGGER_DLL_API LOGGER_DLL_CALL verify_logger_settings( const setting& logger_settings );
	
public:
	// Constructor for the logger base class, this is used by the logstream class to set up the logger.
	logger_base( level log_level, setting logger_settings ) : config{ verify_level_settings( std::move( log_level ) ), verify_logger_settings( std::move( logger_settings ) ) } {}

	logger_base( const logger_base& ) = delete;
	logger_base& operator = ( const logger_base& ) = delete;
	logger_base( logger_base&& ) = default;
	logger_base& operator = ( logger_base&& ) = default;

protected:
	/**
		@brief		Verifies that the log level used for a log is valid (logs must use one of the basic types).

		@details	This function is used to verify that the log level is one of the following: `DEBUG`, `TRACE`, `INFO`, `WARN`, `ERROR`, or `FATAL`
					before it is used to log a message. This is used to prevent invalid log levels from reaching the logger and there being a conflict in level printing.
					This function does not check if the log level is active, for that functionality @see `is_logging_level( const level& )` and @see `is_logging_level_active( const level&, const level& )`.
	
		@param		logging_level A constant reference to the `level` to verify.
		@returns	Returns a constant reference to the same logging level if it is exactly one of the basic logging levels.
		@throws		Throws an exception of `std::invalid_argument` if the logging level is not one of: `DEBUG`, `TRACE`, `INFO`, `WARN`, `ERROR`, or `FATAL`.
	 */
	static const level& LOGGER_DLL_API LOGGER_DLL_CALL verify_logging_level( const level& logging_level );

	/**
		@brief		Helper function of `is_logging_level( const level& )`, compares a logging level to a set of logging levels to determine if the logging level is active.

		@details	This function only checks if there is a matching bit between `logging_level` and `levels`, it does not check if the logging level is valid or not.
					For that functionality @see `verify_logging_level( const level& )`.
		
		@param		logging_level A constant reference to the `level` to verify.
		@returns	Returns a constant reference to the same logging level if it is exactly one of the basic logging levels.
		@throws		Throws an exception of `std::invalid_argument` if the logging level is not one of: `DEBUG`, `TRACE`, `INFO`, `WARN`, `ERROR`, or `FATAL`.
	 */
	static constexpr bool LOGGER_DLL_API LOGGER_DLL_CALL is_contained( const BYTE& checked, const BYTE& base );

	// Compares the logging level with the current logger settings and returns the logging level if valid.
	bool LOGGER_DLL_API LOGGER_DLL_CALL is_logging_level( const level& logging_level ) const;

	// Checks if the logging level is valid and active.
	bool LOGGER_DLL_API LOGGER_DLL_CALL is_loggable( const level& logging_level ) const;

	// Checks if a setting is active.
	bool LOGGER_DLL_API LOGGER_DLL_CALL check_setting( const setting& checked_settings ) const;

private:
	// Configuration settings for the logger to track the logging levels and settings.
	struct config_settings
	{
		level level : level_count;
		setting settings : setting_count;
	} config;

protected:
	static basic_logstream< char >& my_log;
};

/**
 *	@class logstream_log< logger_base::level, class... logger_base::log_args >
 *	
 *	@brief Builds logs to pass the the contained logstream object, created by a call
 */
template < logger_base::level _Level, class... _Log_args >
class basic_logstream_log
{
private:

};

// Loose wrapper around the std::ofstream class, used to log messages to a file, has the `std::ios_base::unitbuf` flag set by default, this may be turned off and you can instead use std::endl for your own control.
// Also note, that support is not provided for overloading the _Traits type for this class, since it the current timestamp operation requires `std::char_traits< _Elem >`.
template < class _Elem >
class basic_logstream : public std::basic_ofstream< _Elem, std::char_traits< _Elem > >, public logger_base
{
public:
	using char_type 	= _Elem;
	using traits_type	= std::char_traits< char_type >;
	using int_type		= typename traits_type::int_type;
	using pos_type		= typename traits_type::pos_type;
	using off_type		= typename traits_type::off_type;
	
	using output_stream = std::basic_ostream< char_type, traits_type >;
	using file_stream 	= std::basic_ofstream< char_type, traits_type >;
	using file_buffer 	= std::basic_filebuf< char_type, traits_type >;

public:
	// It does not makes sense to have a default constructor for this class, since a logger must have a file to log data to.
	basic_logstream() = delete;

private:
	void log_levels()
	{
		constexpr size_t length = 25;
		constexpr const char_type* levels[ level_count ] =
		{
			"DEBUG                    ",
			"TRACE                    ",
			"INFORMATION              ",
			"WARNING                  ",
			"ERROR                    ",
			"FATAL                    "
		};

		if ( this != &my_log ) my_log << TRACE << "Printing the logging levels...";

		for ( size_t index = 0; index < level_count; ++index )
		{
			this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( '\t' ) );
			this->rdbuf()->sputn( levels[ index ], length );
			this->rdbuf()->sputn( ": ", 2 );
			const char_type* enabled = this->is_loggable( static_cast< level >( 1 << index ) ) ? "ENABLED" : "DISABLED";
			this->rdbuf()->sputn( enabled, std::strlen( enabled ) );
			this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( '\n' ) );
		}

		if ( this != &my_log ) my_log << TRACE << "Finished logging the logging levels...";
	}

	void log_settings()
	{
		constexpr size_t length = 25;
		constexpr const char_type* settings[ setting_count ] =
		{
			"AUTOMATIC CRASH LOGGING  ",
			"MOVE DATA WITH LOGGER    "
		};

		if ( this != &my_log ) my_log << TRACE << "Printing the logger settings...";
		
		for ( size_t index = 0; index < setting_count; ++index )
		{
			this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( '\t' ) );
			this->rdbuf()->sputn( settings[ index ], length );
			this->rdbuf()->sputn( ": ", 2 );
			const char_type* enabled = this->check_setting( static_cast< setting >( 1 << index ) ) ? "ENABLED" : "DISABLED";
			this->rdbuf()->sputn( enabled, std::strlen( enabled ) );
			this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( '\n' ) );
		}

		if ( this != &my_log ) my_log << TRACE << "Finished logging the logger settings...";
	}

	void do_startup()
	{
		constexpr char_type start_message[] = "[SYSTEM]: Beginning a new log with the following settings:\n";
		constexpr size_t start_message_length = sizeof( start_message ) / sizeof( char_type ); // -1 to remove the null terminator.

		if ( this != &my_log ) my_log << TRACE << "Logging the system startup message...";

		this->rdbuf()->sputn( start_message, start_message_length - 1 );

		this->log_levels();
		this->log_settings();
	}

	void startup()
	{
		if ( this != &my_log ) my_log << TRACE << "Entering the startup logging function for the new logger...";

		// Prepare the stream for output, if a failure occurs return instantly.
		typename output_stream::sentry my_sentry( *this );
		if ( !my_sentry )
		{
			if ( this != &my_log ) my_log << ERROR << "Logger was invalid and unable to log the configuration settings!";
			throw std::ios_base::failure( "Logger startup and initial system log failed!" );
		}

		if ( this != &my_log ) my_log << INFO << "Logger ready, beginning to log configuration settings...";

		this->do_startup();
	}
	
	template < class _Other_elem >
	static std::string convert_to_bytes( const std::basic_string< _Other_elem >& formatted_string )
	{
		std::string byte_string;
		byte_string.reserve( formatted_string.length() );
		std::use_facet< std::ctype< _Other_elem > >( std::locale() ).narrow( formatted_string.data(), formatted_string.data() + formatted_string.length(), byte_string.data() );

		return byte_string;
	}

	static void print_new_logger( const char* file_name )
	{ my_log << INFO << "A new logstream has opened with the file name: " << file_name << '\n'; }
	
	static void print_new_logger( const std::string& file_name )
	{ print_new_logger( file_name.c_str() ); }

	static void print_new_logger( const wchar_t* file_name )
	{ print_new_logger( std::wstring( file_name ) ); }
	
	static void print_new_logger( const std::wstring& file_name )
	{ print_new_logger( convert_to_bytes( file_name ) ); }

public:
	basic_logstream( const char* file_name, level&& log_level = level::DEFAULT, setting&& log_settings = setting::ALL_SET ) :
		file_stream( file_name ), logger_base( log_level, log_settings )
	{
		if ( this != &my_log ) print_new_logger( file_name );

		this->setf( std::ios_base::unitbuf );
		this->startup();
	}

	basic_logstream( const std::string& file_name, level&& log_level = level::DEFAULT, setting&& log_settings = setting::ALL_SET ) :
		file_stream( file_name ), logger_base( log_level, log_settings )
	{
		if ( this != &my_log ) print_new_logger( file_name );

		this->setf( std::ios_base::unitbuf );
		this->startup();
	}
	
	basic_logstream( const wchar_t* file_name, level&& log_level = level::DEFAULT, setting&& log_settings = setting::ALL_SET ) :
		file_stream( file_name ), logger_base( log_level, log_settings )
	{
		if ( this != &my_log ) print_new_logger( file_name );

		this->setf( std::ios_base::unitbuf );
		this->startup();
	}
		
	basic_logstream( const std::wstring& file_name, level&& log_level = level::DEFAULT, setting&& log_settings = setting::ALL_SET ) :
		file_stream( file_name ), logger_base( log_level, log_settings )
	{
		if ( this != &my_log ) print_new_logger( file_name );

		this->setf( std::ios_base::unitbuf );
		this->startup();
	}

	basic_logstream( const basic_logstream& ) = delete;
	basic_logstream( basic_logstream&& );
	basic_logstream& operator = ( const basic_logstream& ) = delete;
	basic_logstream& operator = ( basic_logstream&& );

	std::basic_string< char_type > time_format( const char_type* format )
	{
		if ( this != &my_log ) my_log << TRACE << "Updating a logger's time format...";

		std::basic_string< char_type > previous_format( this->timestamp_format );
		this->timestamp_format = format;
		return previous_format;
	}

private:
	static constexpr const char_type* level_tag( level&& log_level )
	{
		switch ( verify_logging_level( log_level ) )
		{
			case level::DEBUG: return "DEBUG";
			case level::TRACE: return "TRACE";
			case level::INFO: return "INFO";
			case level::WARN: return "WARNING";
			case level::ERROR: return "ERROR";
			case level::FATAL: return "FATAL";

			// This will never happen since we verify the logging level before this function is called, and that will throw an exception if it is not one of the above.
			// This is just to make the compiler shut up (I despise warnings).
			default: return "";
		}
	}

	std::basic_string< _Elem > timestamp()
	{
		using namespace std::chrono;
		zoned_time timestamp( current_zone(), system_clock::now() );
		return std::vformat( this->getloc(), "{:" + this->timestamp_format + '}', std::make_format_args( timestamp ) );
	}

protected:
	void start_log( level&& log_level )
	{
		const char_type* tag = level_tag( std::move( log_level ) );

		if ( tag == "" && this != &my_log ) my_log << ERROR << "Invalid section of code reached in the `level_tag` function!";

		// Print the current time to the log stream.
		this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( '\n' ) );
		this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( '[' ) );
		std::basic_string< char_type > current_timestamp = this->timestamp();
		this->rdbuf()->sputn( current_timestamp.c_str(), current_timestamp.length() );
		this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( ' ' ) );
		this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( '-' ) );
		this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( ' ' ) );
		this->rdbuf()->sputn( tag, std::strlen( tag ) );
		this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( ']' ) );
		this->rdbuf()->sputc( std::use_facet< std::ctype< char_type > >( this->getloc() ).widen( ' ' ) );
	}

private:
	void log_exception_msg( const char* message )
	{
		this->start_log( level::FATAL );
		this->rdbuf()->sputn( message, traits_type::length( message ) );
	}

	// Logs the exception and any nested exceptions
	void log_exception( const std::exception& exc )
	{
		// Log the current exception as a fatal exception.
		this->log_exception_msg( exc.what() );

		// Try to rethrow nested exceptions.
		try { std::rethrow_if_nested( exc ); }

		// Catch any nested exceptions and repeat the process.
		catch( const std::exception& nested ) { this->log_exception( nested ); }
	}

	void log_exception_stack()
	{
		// Rethrow the current exception in a try block so we can catch it and log it.
		try	{ std::rethrow_exception( std::current_exception() ); }

		// Nested exception specific case since we want to rethrow all nested exceptions.
		catch( const std::exception& exc ) { this->log_exception( exc ); }

		// Anything that is not a standard exception, mark it as unknown.
		catch( ... ) { this->log_exception_msg( "Unknown exception!" ); }
	}

	void log_forced_shutdown()
	{
		constexpr char_type shutdown_message[] = "\n\n[SYSTEM]: Logger was forced to shutdown due to an uncaught exception in the current scope!";
		constexpr size_t shutdown_message_length = sizeof( shutdown_message ) / sizeof( char_type );

		this->rdbuf()->sputn( shutdown_message, shutdown_message_length - 1 );
	}

public:
	~basic_logstream()
	{
		if ( this != &my_log ) my_log << TRACE << "A logger has reached the end of its life...";

		if ( std::uncaught_exceptions() == 0 )
		{
			if ( this != &my_log ) my_log << TRACE << "The logger is being closed gracefully!";
			return;
		}

		if ( this != &my_log ) my_log << INFO << "The logger was forced to shutdown due to an uncaught exception!";
		
		typename output_stream::sentry my_sentry( *this );
		if ( !my_sentry )
		{
			if ( this != &my_log ) my_log << ERROR << "The logger's state was invalid and could not log a forced shutdown message!";
		}
		else
		{
			if ( this != &my_log ) my_log << INFO << "Logger is still valid, logging a fatal shutdown message...";
			this->log_forced_shutdown();
			if ( this->check_setting( setting::PRINT_CRASH ) ) this->log_exception_stack();
		}

		if ( this != &my_log ) my_log << TRACE << "Closing the logger and its underlying stream...";
	}

	// Operator function to print the log level and current time to the log stream.
	basic_logstream& operator << ( level&& logging_level )
	{
		typename output_stream::sentry my_sentry( *this );

		if ( my_sentry && this->is_loggable( logging_level ) ) this->start_log( std::move( logging_level ) );

		return *this;
	}

private:
	const std::basic_string< _Elem > timestamp_format = "%F %r"; // Default time format for the log stream. (YYYY-MM-DD HH:MM:SS AM/PM)
};

template < class _Elem >
class Time_Format
{
public:
	explicit Time_Format( const _Elem* format ) : format( format ) {}
	explicit Time_Format( const std::basic_string< _Elem >& format ) : format( format ) {}
	explicit Time_Format( std::basic_string< _Elem >&& format ) : format( format ) {}
	explicit Time_Format( const std::basic_string_view< _Elem >& format ) : format( format ) {}
	explicit Time_Format( std::basic_string_view< _Elem >&& format ) : format( format ) {}

	friend basic_logstream< _Elem >& operator << ( basic_logstream< _Elem >& output, Time_Format< _Elem >&& format )
	{
		output.time_format( std::move( format.format ) );
		return output;
	}

private:
	std::basic_string< _Elem > format;
};

template < class _Elem >
Time_Format< _Elem > time_format( const _Elem* format )
{ return Time_Format< _Elem >( format ); }
template < class _Elem >
Time_Format< _Elem > time_format( const std::basic_string< _Elem >& format )
{ return Time_Format< _Elem >( format ); }
template < class _Elem >
Time_Format< _Elem > time_format( std::basic_string< _Elem >&& format )
{ return Time_Format< _Elem >( format ); }
template < class _Elem >
Time_Format< _Elem > time_format( const std::basic_string_view< _Elem >& format )
{ return Time_Format< _Elem >( format ); }
template < class _Elem >
Time_Format< _Elem > time_format( std::basic_string_view< _Elem >&& format )
{ return Time_Format< _Elem >( format ); }

using logstream = basic_logstream< char >;
using wlogstream = basic_logstream< wchar_t >;

#endif // LOGSTREAM_HPP