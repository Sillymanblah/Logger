#pragma once
#ifndef LOGSTREAM_HPP
#define LOGSTREAM_HPP

// Direct
#include "time_fix.hpp"

// STL
#include <ostream>
#include <thread>
#include <mutex>
#include <concepts>
#include <iomanip>

/// @brief Developer settings are individually toggleable.
enum developer_settings : char
{

	none = 0b00, // No dev settings enabled, each has to be explicitly enabled.
	trace = 0b01, // Enable only trace messages.
	debug = 0b10, // Enable only debug messages.
	all = trace & debug, // Enable both types of messages.
};

constexpr size_t developer_settings_bits = 2;

/// @brief Runtime settings are treated as level settings, i.e. everything below the current setting is on automatically.
enum system_settings : char
{
	info = 0b001, // Only info logs show up.
	warn = 0b010, // Warnings and information displayed.
	error = 0b011, // Non-fatal errors and other messages displayed.
	fatal = 0b100, // All system messages are displayed.
};

constexpr size_t system_settings_bits = 3;

/// @brief Structure to get the parts of the settings from the whole, used in `settings_data`.
struct settings
{
	developer_settings dev : developer_settings_bits;
	system_settings sys : system_settings_bits;

	settings( developer_settings dev, system_settings sys ) : dev( dev ), sys( sys ) {}
};

/// @brief Logging level is used to pass the current level being logged into the `basic_logstream` object to start a `basic_logstream_log`.
enum class logging_level : char
{
	trace, // Follow the program path, trace to see which functions are called.
	debug, // State information tracking to try to debug potential errors.

	info, // Logging information to the user for events or state changes.
	warn, // Emitted when there is an issue in the state that does not affect program flow.
	error, // Issues that are dangerous to the program and cause some parts to be unable to execute.
	fatal, // Problems that result in a total program failure (either gracefully or ungracefully).
};

/// @brief Helper function to get the associated logging level string for the logging levels.
/// @param level The level to get the string for.
/// @return A pointer to a character array containing the logging level name, or an empty string if an invalid value is passed in.
inline constexpr const char* level_string( logging_level level )
{
	switch ( level )
	{
		case logging_level::trace: return "TRACE";
		case logging_level::debug: return "DEBUG";
		case logging_level::info: return "INFO";
		case logging_level::warn: return "WARN";
		case logging_level::error: return "ERROR";
		case logging_level::fatal: return "FATAL";
		default: return "";
	}
}

/// @brief Concept for differentiation so we can ensure that there is a compile-time error to attempt to start another log from the current log.
template < class type_1, class type_2 >
concept different_from = !std::same_as< std::remove_cvref_t< type_1 >, std::remove_cvref_t< type_2 > >;

/// @brief Logstream class is a wrapper around `std::basic_ostream` which protects its `operator <<` functions.
/// In doing so, it takes on only a single operator function of `operator << ( logging_level )`.
/// That operator attempts to lock the `mutex` within this `basic_logstream` during the construction of the `basic_logstream_log`,
/// if successful, it will then print out the log header and return the object created which exposes the underlying `operator <<` functions.
///
/// @tparam char_type The type of character this stream is responsible for, typically either `char` or `wchar_t`, but some OS's
/// allow for use of `char16_t`, `char32_t`, etc.
/// @tparam traits_type The traits class for mapping to and from `char_type`.
template < class my_char, class my_traits, class my_mutex >
class basic_logstream : protected std::basic_ostream< my_char, my_traits >
{
protected:
	using char_type = my_char;
	using traits_type = my_traits;
	using mutex_type = my_mutex;
	
	using buffer_type = std::basic_streambuf< my_char, my_traits >;
	using format_type = std::basic_string_view< my_char >;
	using lock_type = std::unique_lock< my_mutex >;

	using super = std::basic_ostream< my_char, my_traits >;

	// Forwarding declaration of a log class inside of basic_logstream so it has access to private members of this class.
	class log;

	/// @brief The default time format for a logstream, might expose it or make it mutable.
	static constexpr format_type default_format = "%D %T";

public:
	/// @brief 
	/// @param buffer 
	/// @param settings 
	/// @param time_format 
	basic_logstream( buffer_type* buffer, settings settings, format_type time_format = default_format ) :
		super( buffer ),
		log_settings( settings ),
		time_format( time_format )
	{}

	/// @brief 
	/// @param buffer 
	/// @param dev 
	/// @param sys 
	/// @param time_format 
	basic_logstream( buffer_type* buffer, developer_settings dev, system_settings sys, format_type time_format = default_format ) :
		basic_logstream( buffer, settings( dev, sys ) ) // Delegate to the other function.
	{}

private:
	/// @brief An overload for specifically the system settings, which returns true if the sys level is logged.
	///
	/// @param level The system logging level.
	/// @return `true` if `level` is logged, `false` otherwise.
	bool is_logged( system_settings level )
	{ return level <= ( this->log_settings.sys ); }

	/// @brief An overload for specifically the developer settings, which returns true if the dev level is logged.
	///
	/// @param level The developer logging level.
	/// @return `true` if `level` is logged, `false` otherwise.
	bool is_logged( developer_settings level )
	{ return level & ( this->log_settings.dev ); }

protected:
	/// @brief Tells whether a logging level is currently being logged by the system, used to decide when to output.
	///
	/// @param level The desired level of an output log.
	/// @return `true` if `level` is being logged, `false` otherwise.
	bool is_logged( logging_level level )
	{
		switch ( level )
		{
			case logging_level::trace: return this->is_logged( developer_settings::trace );
			case logging_level::debug: return this->is_logged( developer_settings::debug );
			case logging_level::info: return this->is_logged( system_settings::info );
			case logging_level::warn: return this->is_logged( system_settings::warn );
			case logging_level::error: return this->is_logged( system_settings::error );
			case logging_level::fatal: return this->is_logged( system_settings::fatal );

			default: throw std::invalid_argument( "Logging level given to `basic_logstream` was invalid!" );
		}
	}
private:
	/// @brief Gets a pointer to a `std::tm` struct to use to put the time into the log.
	/// @note This function stores the `std::time_t` object in the class variable `last_time` to be able to recall it later.
	///
	/// @return A `std::tm*` holding the current local time.
	std::tm* get_current_time()
	{
		std::time( &this->last_time );
		standardized::localtime_s( &this->last_time, &this->last_time_data );
		return &this->last_time_data;
	}

protected:
	/// @brief Print's the initial information such as timestamp and logging level string to start a log.
	/// Do not call this method unless a log is actually starting!
	///
	/// @param level The level to use for the string (ex: "INFO").
	void start_log( logging_level level )
	{ *this << std::put_time( get_current_time(), time_format.data() ) << ' ' << level_string( level ) <<  " - "; }

public:
	/// @brief The only available `operator <<` for `basic_logstream` which locks the thread until we can create a `basic_logstream_log`.
	/// 
	/// @param level The `logging_level` of this log.
	/// @return A `basic_logstream_log` for outputting the rest of the data.
	log operator << ( logging_level level )
	{
		// If we are not logging this data currently, create a log consumer.
		if ( !this->is_logged( level ) ) return log( nullptr );

		// Otherwise, pass `this` into the log to lock the mutex and reveal std::ostream.
		log new_log( this );

		// Start the log with any initial data.
		this->start_log( level );

		// Return the log we created for the user to do output operations.
		return std::move( new_log );
	}

	/// @brief Update the current settings with new settings.
	///
	/// @param log_settings The settings that will be set after this function call.
	/// @return The old settings.
	settings update_settings( settings log_settings )
	{
		// Before performing any operations, gain control of the mutex.
		lock_type lock( this->mutex );

		// Perform the update.
		settings old = this->log_settings;
		this->log_settings = log_settings;
		return old;
	}

	/// @brief Update the current time format with a new one.
	/// @param time_format The new time format to replace the old one.
	/// @return The old time format.
	format_type update_time_format( format_type time_format )
	{
		// Before performing any operations, gain control of the mutex.
		lock_type lock( this->mutex );

		// Perform the update.
		format_type old = this->time_format;
		this->time_format = time_format;
		return time_format;
	}

	/// @brief Get the time of the last log (for whatever reason it may be needed)
	/// @return The time that the most recent log was started.
	std::time_t last_log_time()
	{
		// Before performing any operations, gain control of the mutex.
		lock_type lock( this->mutex );

		// Return the data since it is stable with the mutex being locked.
		return this->last_time;
	}

private:
	/// @brief The mutex that is used to take control of this logstream.
	mutex_type mutex;

	/// @brief The settings associated with this logstream
	settings log_settings;

	/// @brief The start time of the last log, used to store the time temporarily for `start_log` and `std::format`.
	std::time_t last_time;

	/// @brief The time struct for the last log's start time, also used for the storage duration to push data to the stream.
	/// The primary difference is this member is not offered by a getter, one must create their own `std::tm` object if they need one.
	std::tm last_time_data;

	/// @brief The time format to use when logging the time.
	format_type time_format;
};

/// @brief Logstream wrapper class that locks the associated mutex of `basic_logstream` for the duration of its life.
/// When it gets destructed the `std::unique_lock` holding the `std::mutex` will automatically release the lock.
/// 
/// @tparam my_char The associated character type with the underlying stream object. 
/// @tparam my_traits The associated traits type of the underlying stream object.
/// @tparam my_mutex The associated mutex type of the `basic_logstream` so we can lock the mutex.
template < class my_char, class my_traits, class my_mutex >
class basic_logstream< my_char, my_traits, my_mutex >::log
{
private:
	using char_type = my_char;
	using traits_type = my_traits;
	using mutex_type = my_mutex;

	using stream_type = std::basic_ostream< my_char, my_traits >;
	using parent = basic_logstream< my_char, my_traits, my_mutex >;

public:
	/// @brief Starts a log for the logstream and locks other logs from coming until this object is destroyed.
	///
	/// @param logstream A pointer to the associated `basic_logstream` which we lock and use to output.
	/// If that pointer is null, then this object will not lock any mutex and instead will just consume any output sent to it.
	log( parent* parent_ptr )
	: stream( parent_ptr )
	{
		// If the stream pointer given to us is good, lock the associated mutex.
		if ( parent_ptr ) lock = lock_type( parent_ptr->mutex );
	}

	log( log&& ) = default;

public:
	/// @brief Takes an argument to pass to the underlying stream object inside of `basic_logstream` to perform stream output.
	///
	/// @tparam arg_type The type of argument intended to pass to the underlying `std::ostream operator <<` call,
	/// which must have a function defined for it otherwise this call is ill-formed. Note that this type is restricted
	/// from being a `logging_level` type as that would deadlock the thread (and thus the program).
	///
	/// @param arg The argument to pass to the underlying `operator <<`.
	/// @return A reference to `this` for chaining output operations.
	template < different_from< logging_level > arg_type >
	parent::log& operator << ( arg_type arg )
	{
		// If the stream exits, pass the arg directly to the `std::basic_ostream` object to print.
		if ( stream ) *stream << arg;

		// Return `this` object for further operations.
		return *this;
	}

	/// @brief Function wrapper to pass function pointers to the stream object.
	///
	/// @param function A function pointer that performs a stream operation on `std::ios_base`.
	/// @return A reference to `this` log for chaining output.
	parent::log& operator << ( std::ios_base& ( *function )( std::ios_base& ) )
	{
		// Pass the pointer to the stream, this is the same as calling function( *stream );
		if ( stream ) *stream << function;

		// Return `this` object for further operations.
		return *this;
	}

	/// @brief Function wrapper to pass function pointers to the stream object.
	///
	/// @param function A function pointer that performs a stream operation on the underlying `std::basic_ostream`.
	/// @return A reference to `this` log for chaining output.
	parent::log& operator << ( stream_type& ( *function )( stream_type& ) )
	{
		// Pass the pointer to the stream, this is the same as calling function( *stream );
		if ( stream ) *stream << function;

		// Return `this` object for further operations.
		return *this;
	}

private:
	/// @brief A pointer to the underlying `basic_logstream` object which we hold a `lock` on.
	stream_type* stream;

	/// @brief The lock that is blocking the `std::mutex` from being locked in the contained `logstream`.
	lock_type lock;
};

#endif // LOGSTREAM_HPP
