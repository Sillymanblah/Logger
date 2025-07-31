#pragma once
#ifndef LOGSTREAM_HPP
#define LOGSTREAM_HPP

#include <ostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <concepts>

// Temp
#include <iostream>

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

/// @brief Logging level is used to pass the current level being logged into the `basic_logstream` object to start a `basic_logstream_log`.
enum class logging_level : char
{
	trace = developer_settings::trace, // Follow the program path, trace to see which functions are called.
	debug = developer_settings::debug, // State information tracking to try to debug potential errors.

	info = system_settings::info << developer_settings_bits, // Logging information to the user for events or state changes.
	warn = system_settings::warn << developer_settings_bits, // Emitted when there is an issue in the state that does not affect program flow.
	error = system_settings::error << developer_settings_bits, // Issues that are dangerous to the program and cause some parts to be unable to execute.
	fatal = system_settings::fatal << developer_settings_bits, // Problems that result in a total program failure (either gracefully or ungracefully).
};

// Forwarding declaration to use in the below class.
template < class my_char, class my_traits, class my_mutex >
class basic_logstream;

/// @brief Concept for differentiation so we can ensure that there is a compile-time error to attempt to start another log from the current log.
template < class type_1, class type_2 >
concept different_from = !std::same_as< std::remove_cvref_t< type_1 >, std::remove_cvref_t< type_2 > >;

/// @brief Logstream wrapper class that locks the associated mutex of `basic_logstream` for the duration of its life.
/// When it gets destructed the `std::unique_lock` holding the `std::mutex` will automatically release the lock.
/// 
/// @tparam my_char The associated character type with the underlying stream object. 
/// @tparam my_traits The associated traits type of the underlying stream object.
/// @tparam my_mutex The associated mutex type of the `basic_logstream` so we can lock the mutex.
template < class my_char, class my_traits, class my_mutex >
class basic_logstream_log
{
private:
	using char_type = my_char;
	using traits_type = my_traits;
	using mutex_type = my_mutex;

	using stream_type = std::basic_ostream< my_char, my_traits >;
	using logstream_type = basic_logstream< my_char, my_traits, my_mutex >;
	using lock_type = std::unique_lock< my_mutex >;

	friend logstream_type;

protected:
	/// @brief Starts a log for the logstream and locks other logs from coming until this object is destroyed.
	///
	/// @param logstream A pointer to the associated `basic_logstream` which we lock and use to output.
	/// If that pointer is null, then this object will not lock any mutex and instead will just consume any output sent to it.
	basic_logstream_log( logstream_type* stream_ptr );

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
	basic_logstream_log& operator << ( arg_type arg )
	{
		// If the stream exits, pass the arg directly to the `std::basic_ostream` object to print.
		if ( stream ) *stream << arg;

		// Return `this` object for further operations.
		return *this;
	}

	basic_logstream_log& operator << ( std::ios_base& (* function)( std::ios_base& ) )
	{
		if ( stream ) *stream << function;

		return *this;
	}

	basic_logstream_log& operator << ( stream_type& (* function)( stream_type& ) )
	{
		if ( stream ) *stream << function;

		return *this;
	}

private:
	/// @brief A pointer to the underlying `basic_logstream` object which we hold a `lock` on.
	stream_type* stream;

	/// @brief The lock that is blocking the `std::mutex` from being locked in the contained `logstream`.
	lock_type lock;
};

/// @brief Logstream class is a wrapper around `std::basic_ostream` which protects its `operator <<` functions.
/// In doing so, it takes on only a single operator function of `operator << ( logging_level )`.
/// That operator attempts to lock the `mutex` within this `basic_logstream` during the construction of the `basic_logstream_log`,
/// if successful, it will then print out the log header and return the object created which exposes the underlying `operator <<` functions.
///
/// @tparam char_type The type of character this stream is responsible for, typically either `char` or `wchar_t`, but some OS's
/// allow for use of `char16_t`, `char32_t`, etc.
/// @tparam traits_type The traits class for mapping to and from `char_type`.
template < class my_char, class my_traits = std::char_traits< my_char >, class my_mutex = std::mutex >
class basic_logstream : virtual protected std::basic_ostream< my_char, my_traits >
{
protected:
	using char_type = my_char;
	using traits_type = my_traits;
	using mutex_type = my_mutex;

	using super = std::basic_ostream< my_char, my_traits >;
	using buffer_type = std::basic_streambuf< my_char, my_traits >;
	using my_log = basic_logstream_log< my_char, my_traits, my_mutex >;

	friend my_log;

	/// @brief Structure to get the parts of the settings from the whole, used in `settings_data`.
	struct settings_parts
	{
		developer_settings dev : developer_settings_bits;
		system_settings sys : system_settings_bits;
	};

	/// @brief Union to store the data for our settings which can be either grouped or in parts.
	union settings_data
	{
		settings_parts parts;
		char grouped : developer_settings_bits + system_settings_bits;
	};

public:
	basic_logstream( buffer_type* buffer, developer_settings dev, system_settings sys ) : super( buffer ), settings{ dev, sys } {}

private:
	void log_header( logging_level level )
	{
		/// TODO: Add startup details here.
	}

	bool is_logged( system_settings level )
	{ return level <= ( settings.parts.sys ); }

	bool is_logged( developer_settings level )
	{ return level & ( settings.parts.dev ); }

protected:
	bool is_logged( logging_level level )
	{
		switch ( level )
		{
			case logging_level::trace: return is_logged( trace );
			case logging_level::debug: return is_logged( debug );
			case logging_level::info: return is_logged( info );
			case logging_level::warn: return is_logged( warn );
			case logging_level::error: return is_logged( error );
			case logging_level::fatal: return is_logged( fatal );

			default: throw std::invalid_argument( "Logging level given to `basic_logstream` was invalid!" );
		}
	}

public:
	/// @brief The only available `operator <<` for `basic_logstream` which locks the thread until we can create a `basic_logstream_log`.
	/// 
	/// @param level The `logging_level` of this log.
	/// @return A `basic_logstream_log` for outputting the rest of the data.
	my_log operator << ( logging_level level )
	{
		// Pass this into the log capture to obtain the mutex.
		my_log log( is_logged( level ) ? this : nullptr );

		// Once we have created our log and locked the mutex within this thread, output the header.
		log_header( level );

		// Return the log we created for the user to do output operations.
		return log;
	}

protected:
	mutex_type mutex;

	/// @brief The settings associated with this logstream
	settings_data settings;
};

// Definition for the constructor of `basic_logstream_log` since we need the class `basic_logstream` to be initialized for this. 
template < class my_char, class my_traits, class my_mutex >
basic_logstream_log< my_char, my_traits, my_mutex >::basic_logstream_log( basic_logstream< my_char, my_traits, my_mutex >* stream_ptr )
	: stream( stream_ptr )
	{
		// If the stream pointer given to us is good, lock the associated mutex.
		if ( stream_ptr ) lock = lock_type( stream_ptr->mutex );
	}

using logstream = basic_logstream< char >;
using wlogstream = basic_logstream< wchar_t >;

#endif // LOGSTREAM_HPP
