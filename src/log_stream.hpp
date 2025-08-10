#pragma once
#ifndef LOGSTREAM_HPP
#define LOGSTREAM_HPP

// Direct
#include "time_fix.hpp"
#include "log_base.hpp"

// STL
#include <ostream>
#include <thread>
#include <mutex>
#include <concepts>
#include <iomanip>

/// @brief Concept for differentiation so we can ensure that there is a compile-time error to attempt to start another log from the current log.
template < class type_1, class type_2 >
concept different_from = !std::same_as< std::remove_cvref_t< type_1 >, std::remove_cvref_t< type_2 > >;

/// @brief Logstream exposes logging operations which behave similarly to `std::basic_ostream`.
///
/// @details Logstream class is a wrapper around @ref `std::basic_ostream` which protects its `operator <<` functions.
/// In doing so, it takes on only a single operator function of @ref `operator << ( logging_level )`.
/// That operator attempts to lock the @ref `basic_logstream::mutex_type` within this `basic_logstream` during the construction of the @ref `basic_logstream::log`,
/// if successful, it will then print out the log header and return the `log` created which exposes the underlying `operator <<` functions.
///
/// @tparam my_char The type of character this stream is responsible for, typically either `char` or `wchar_t`, but some OS's
/// allow for use of `char16_t`, `char32_t`, etc.
/// @tparam my_traits The traits class for mapping to and from `char_type`.
/// @tparam my_mutex The mutex type for this logstream which must be lockable by using `std::unique_lock< my_mutex >( mutex_obj )`.
/// @tparam my_size The size of the associated buffer inside the `basic_logbuf` this class uses to buffer the data.
template < class my_char, class my_traits, class my_mutex, size_t my_size >
class basic_logstream : public log_base
{
protected:
	using char_type = my_char;
	using traits_type = my_traits;
	using mutex_type = my_mutex;

	static constexpr size_t buffer_size = my_size;
	
	using buffer_type = std::basic_streambuf< char_type, traits_type >;
	using format_type = std::basic_string_view< char_type >;
	using lock_type = std::unique_lock< mutex_type >;

	using my_log = log_base;
	using my_buffer = basic_logbuf< char_type, traits_type, buffer_size >;

	// Forwarding declaration of a log class inside of basic_logstream so it has access to private members of this class.
	class log;

	/// @brief The default time format for a logstream, might expose it or make it mutable.
	static constexpr format_type default_format = "%D %T";

public:
	basic_logstream( format_type time_format = default_format ) :
		my_log(),
		buffer(),
		time_format( time_format )
	{}

	basic_logstream( log_settings settings, format_type time_format = default_format ) :
		my_log( settings ),
		buffer(),
		time_format( time_format )
	{}

	basic_logstream( log_settings settings, typename my_buffer::buffer_set&& buffers, format_type time_format = default_format ) :
		my_log( settings ),
		buffer( std::move( buffers ) ),
		time_format( time_format )
	{}

	basic_logstream( const basic_logstream& other ) = delete;

	basic_logstream& operator = ( const basic_logstream& other ) = delete;

private:
	move( basic_logstream&& other )
	{
		// Before we start, lock their mutex so that we aren't interrupting any operations to avoid race conditions.
		lock_type other_lock( other.mutex );

		// Move our parent class using it's move assignment.
		// Ideally in the constructor case we would like to be able just use the move constructor, but we would be unable to lock their mutex before that.
		static_cast< my_log& >( *this ) = std::move( other );
		
		// Move the buffer.
		this->buffer = std::move( other.buffer );

		// Move the time format.
		this->time_format = std::move( other.time_format );

		// Reset the other buffer to point to null.
		other.buffer = nullptr;
	}

public:
	basic_logstream( basic_logstream&& other )
	{ this->move( std::move( other ) ); }

	basic_logstream& operator = ( basic_logstream&& other )
	{
		// Lock our stream so any current operations are stopped until we finish the move.
		lock_type my_lock( this->mutex );

		// Perform the move.
		this->move( std::move( other ) );
	}

protected:
	/// @brief Print's the initial information such as timestamp and logging level string to start a log.
	/// Do not call this method unless a log is actually starting!
	///
	/// @param level The level to use for the string (ex: "INFO").
	void start_log( log& output, logging_level level )
	{ output << std::put_time( this->log_start(), time_format.data() ) << " [" << level_string( level ) <<  "] - "; }

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
		this->start_log( new_log, level );

		// Return the log we created for the user to do output operations.
		return std::move( new_log );
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

	/// @brief The buffer that this logstream uses handle writing operations.
	my_buffer* buffer;

	/// @brief The time format to use when logging the time.
	format_type time_format;
};

/// @brief Logstream wrapper class that locks the associated mutex of `basic_logstream` for the duration of its life.
/// When it gets destructed the `std::unique_lock` holding the `std::mutex` will automatically release the lock.
/// 
/// @tparam my_char The associated character type with the underlying stream object. 
/// @tparam my_traits The associated traits type of the underlying stream object.
/// @tparam my_mutex The associated mutex type of the `basic_logstream` so we can lock the mutex.
template < class my_char, class my_traits, class my_mutex, size_t my_size >
class basic_logstream< my_char, my_traits, my_mutex, my_size >::log : public std::basic_ostream< my_char, my_traits >
{
private:
	using char_type = my_char;
	using traits_type = my_traits;
	using mutex_type = my_mutex;

	using my_stream = std::basic_ostream< my_char, my_traits >;
	using parent = basic_logstream< my_char, my_traits, my_mutex >;

public:
	/// @brief Starts a log for the logstream and locks other logs from coming until this object is destroyed.
	///
	/// @param logstream A pointer to the associated `basic_logstream` which we lock and use to output.
	/// If that pointer is null, then this object will not lock any mutex and instead will just consume any output sent to it.
	log( parent* logstream )
	: my_stream( ( logstream != nullptr ? logstream->buffer : nullptr ) ) // NOTE: This might be an error, if so figure out a better way to handle it, perhaps by reverting to storing the stream in the parent class.
	{
		// If the stream pointer given to us is good, lock the associated mutex.
		if ( logstream ) lock = lock_type( logstream->mutex );
	}

	log( log&& ) = default;

private:
	/// @brief The lock that is blocking the `std::mutex` from being locked in the contained `logstream`.
	lock_type lock;
};

#endif // LOGSTREAM_HPP
