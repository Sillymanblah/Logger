#pragma once
#ifndef LOG_BUFFER_HPP
#define LOG_BUFFER_HPP

/// STL
#include <streambuf>
#include <set>

/// @brief A type of @ref `std::basic_streambuf` that handles a logging stream's buffer and pushing to multiple streams at once.
///
/// @tparam my_char The character type of the `basic_logbuf`.
/// @tparam my_traits The traits class to use with this `basic_logbuf` for formatting characters.
/// @tparam my_size A `size_t` argument with the desired size (in bytes) of the buffer of this class, defaults to 1KB.
///
/// @note This class does not override `overflow` so, ensure that `my_size` is big enough to hold an entire log.
/// If that condition is met, there should be no issues since a `basic_logbuf` is intentionally synced after every log to keep the output up-to-date with the system.
template < class my_char, class my_traits = std::char_traits< my_char >, size_t my_size = 1024 >
class basic_logbuf : public std::basic_streambuf< my_char, my_traits >
{
protected:
	using char_type = my_char;
	using char_ptr = char_type*;
	using traits_type = my_traits;

	using buffer_type = std::basic_streambuf< char_type, traits_type >;

	using int_type = typename buffer_type::int_type;
	using pos_type = typename buffer_type::pos_type;
	using off_type = typename buffer_type::off_type;

	using buffer_set = std::set< buffer_type* >;

	static constexpr size_t buffer_size = my_size;
	static constexpr size_t buffers_count = my_count;

private:
	/// @brief Initializes the super class' put area with our buffer by calling @ref `std::basic_streambuf::setp( char_type*, char_type* )`.
	init() { buffer_type::setp( buffer, buffer + buffer_size + 1 ); }

public:
	/// @brief Default constructor for @ref `basic_logbuf` which initializes our underlying @ref `std::basic_streambuf` with our buffer and sets our output buffers to null.
	basic_logbuf() : buffer_type(), buffers() { this->init(); }

	/// @brief Constructs a @ref `basic_logbuf` which initializes the underlying @ref `std::basic_streambuf` and the set of @ref `basic_logbuf::buffers`.
	/// @param buffers A set of buffers to move into our set of buffers to output to. If there is a `nullptr` in here, it will be removed.
	basic_logbuf( const buffer_set& buffers ) : buffer_type(), buffers( buffers )
	{
		// Initialize our buffer.
		this->init();

		// Delete any `nullptr` buffers that were passed in since they are invalid.
		this->buffers.erase( nullptr );
	}

	/// @brief Constructs a @ref `basic_logbuf` which initializes the underlying @ref `std::basic_streambuf` and the set of @ref `basic_logbuf::buffers`.
	/// @param buffers A set of buffers to move into our set of buffers to output to. If there is a `nullptr` in here, it will be removed.
	basic_logbuf( buffer_set&& buffers ) : buffer_type(), buffers( buffers )
	{
		// Initialize our buffer.
		this->init();

		// Delete any `nullptr` buffers that were passed in since they are invalid.
		this->buffers.erase( nullptr );
	}

	basic_logbuf( buffer_type* buffer ) : buffer_type(), buffers()
	{
		// Initialize our buffer.
		this->init();

		// Add our buffer if it is not `nullptr`.
		if ( buffer != nullptr ) this->add_buffer( buffer );
	}

private:
	/// @brief A helper function to get the 
	///
	/// @param from 
	/// @return 
	char_ptr put_position( std::ios_base::seekdir from )
	{
		switch ( from )
		{
		case std::ios_base::beg: return this->pbase();
		case std::ios_base::cur: return this->pptr();
		case std::ios_base::end: return this->epptr();
		
		default: return nullptr;
		}
	}

private:
	/// @brief Helper function to sync the data to the buffer and force it to sync with it's underlying stream.
	///
	/// @param buffer The buffer to put the characters into and synchronize.
	/// @param start The start of the character buffer to put.
	/// @param count The number of characters we are putting to the stream.
	static bool sync_buffer( buffer_type* buffer, char_type* start, std::streamsize count )
	{
		// Attempt to put the characters into the buffer, if we cannot fit them all return failure.
		if ( buffer->sputn( start, count ) != count ) return false;

		// Force the buffer to synchronize.
		return ( buffer->pubsync() == 0 );
	}
	
protected:
	/// @brief Performs the job of syncronizing this buffer with the related buffers that it is managing.
	///
	/// @return A value of `0` if we successfully passed the data off to ALL other buffers, otherwise `-1`.
	int sync() override
	{
		// Get the start and end of the characters we are pushing to the streams.
		char_ptr start = this->pbase(), end = this->pptr();

		// Get the number of characters we are pushing to the streams.
		std::streamsize count = end - start;

		// Flag to track our success.
		bool success = true;

		// Then for each stream we are storing in the list of buffers, try to synchronize it with our data and return `-1` if it fails.
		for ( buffer_type* buffer : buffers ) success &= sync_buffer( buffer, start, count );

		// If we were successful, return `0`, otherwise `-1`.
		return success - 1;
	}

public:
	/// @brief A function to add a stream buffer to the list of associated stream buffers this log outputs to.
	///
	/// @param buffer A pointer to a @ref `std::basic_streambuf< char_type, traits_type >` to add to the set of buffers.
	/// @return A boolean indicating if the buffer was added to the set (returns `false` only if the buffer is already used).
	///
	/// @note A call to this function with a `std::basic_streambuf` which is the base of another `basic_logbuf` is a mistake and invalid.
	/// Operations performed at this level are not thread safe and thus using a pointer to another `basic_logbuf` defeats the entire purpose of @ref `basic_logstream`.
	/// Additionally, this class is not responsible for managing the lifetime of those buffers, so if they need to be cleaned up, that is the responsibility of the developer.
	/// Lastly, if `nullptr` is passed to this function it is ignored entirely.
	bool add_buffer( buffer_type* buffer )
	{ if ( buffer != nullptr ) buffers.insert( buffer ); }

	/// @brief An overload of @ref `basic_logbuf::add_buffer( @ref buffer_type* )` which specifically marks using another log buffer inside a log buffer as invalid.
	/// This makes it so that implicit casting shouldn't happen and instead the compiler will throw an error.
	///
	/// @param log_buffer A pointer to another `basic_logbuf`.
	/// @return Nothing, since this is a deleted function, for more information see @ref `size_t add_buffer( @ref buffer_type* )`.
	bool add_buffer( basic_logbuf* log_buffer ) = delete;

	/// @brief A function to remove a previously added buffer from the list of associated stream buffers.
	///
	/// @param buffer A pointer to the buffer object to remove.
	/// @return A pointer to the @ref `buffer_type` that was removed or null if it could not be found.
	buffer_type* remove_buffer( buffer_type* buffer )
	{ return ( buffers.erase( buffer ) ) ? buffer : nullptr; }

private:
	/// @brief The character buffer this stream object uses for it's put area.
	my_char buffer[ buffer_size ];

	/// @brief The set of buffer pointers that we use to flush to.
	buffer_set buffers;
};

#endif // LOG_BUFFER_HPP