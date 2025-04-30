#pragma once
#ifndef IO_FUNCTIONS_HPP
#define IO_FUNCTIONS_HPP

// STL Files
#include <ostream>
#include <istream>
#include <iterator>
#include <cstdint>

size_t get_binary_cstring( char* buf, size_t size, const uint64_t& value );

template < class _Elem, class _Traits >
inline bool Fill( std::basic_streambuf< _Elem, _Traits >* stream_buffer, const _Elem& fill_char, const std::streamsize& count )
{
	for ( std::streamsize index = 0; index < count; ++index )
		if ( stream_buffer->sputc( fill_char ) == _Traits::eof() )
			return false;
	return true;
}

template < class _Elem, class _Traits >
inline bool Put( std::basic_streambuf< _Elem, _Traits >* stream_buffer, const _Elem* buffer, const std::streamsize& count )
{
	for( std::streamsize index = 0; index < count; ++index )
		if ( stream_buffer->sputc( buffer[index] ) == _Traits::eof() )
			return false;
	return true;
}

template < class _Elem, class _Traits >
inline bool fill( std::basic_ostream< _Elem, _Traits >& output, const std::streamsize& count )
{ return Fill( output.rdbuf(), output.fill(), count ); }

template < class _Elem, class _Traits >
inline bool put( std::basic_ostream< _Elem, _Traits >& output, const _Elem* buffer, const std::streamsize& count )
{ return Put( output.rdbuf(), buffer, count ); }

/*
 - @section `binary` i/o functionals
 - @{
 */

// This is a helper function, do not call it directly.
template < class _Int, class _Elem, class _Traits >
inline void do_binary_print( std::basic_ostream< _Elem, _Traits >& output, const _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `do_binary_print` requires an integral type!" );
	
	constexpr size_t bit_count = 8 * sizeof( _Int );
	constexpr size_t max_size = bit_count + 2; // 2 for the prefix 0b.

	const size_t prefix_size = output.flags() & std::ios_base::showbase ? 2 : 0; // Prefix size, 2 or 0 depending on whether base is shown.
	
	char buffer[ max_size ] = { '0', ( output.flags() & std::ios_base::uppercase ) ? 'B' : 'b' }; // Buffer to hold the binary string, greedily add prefix since it may be overwritten or used.
	const size_t output_size = prefix_size + get_binary_cstring( buffer + prefix_size, bit_count, value ); // Fill the buffer with the binary string, and get the size of the buffer after filling.
	
	// Prepare stream and check state.
	typename std::basic_ostream< _Elem, _Traits >::sentry my_sentry( output ); // We must hold this lifetime so that the unitbuf flag is handled correctly.
	if ( !my_sentry ) return;
	std::ios_base::iostate state = std::ios_base::goodbit;
	
	std::basic_string< _Elem > printed( output_size, _Elem( 0 ) ); // Prefix using the proper char type.
	std::use_facet< std::ctype< _Elem > >( output.getloc() ).widen( buffer, buffer + output_size, printed.data() ); // Convert the prefix to the proper char type.
	
	std::streamsize fillspace = output.width( 0 ) - output_size;

	const std::ios_base::fmtflags adjustfield = output.flags() & std::ios_base::adjustfield;
	
	// Right align the output, by filling the left side with the fill character.
	if ( adjustfield != std::ios_base::left && adjustfield != std::ios_base::internal )
	{
		if ( !fill( output, fillspace ) ) state = std::ios_base::badbit; // If the fill fails, set badbit.
		fillspace = 0;
	}

	// Print the prefix, prints nothing if showbase bit is off.
	if ( !put( output, printed.data(), prefix_size ) ) state = std::ios_base::badbit; // If the fill fails, set badbit.
	
	// Internal fill, fills space between the prefix and the binary string.
	if ( adjustfield == std::ios_base::internal )
	{
		if ( !fill( output, fillspace ) ) state = std::ios_base::badbit; // If the fill fails, set badbit.
		fillspace = 0;
	}

	// Print the binary string.
	if ( !put( output, printed.data() + prefix_size, output_size - prefix_size) ) state = std::ios_base::badbit; // If the put fails, set badbit.
	
	// Left align the output, by filling the right side with the fill character.
	if( !fill( output, fillspace ) ) state = std::ios_base::badbit; // If the fill fails, set badbit.

	output.setstate( state ); // Set the state of the output stream.
}

// Print a binary value to the output stream.
template < class _Int, class _Elem, class _Traits >
inline std::basic_ostream< _Elem, _Traits >& binary( std::basic_ostream< _Elem, _Traits >& output, const _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	do_binary_print( output, value );
	
	return output;
}

template < class _Int, class _Elem, class _Traits >
inline std::basic_ostream< _Elem, _Traits >& binary( std::basic_ostream< _Elem, _Traits >& output, _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	do_binary_print( output, value );
	
	return output;
}

// Print a binary value to the output stream.
template < class _Int, class _Elem, class _Traits >
inline std::basic_ostream< _Elem, _Traits >& binary( std::basic_ostream< _Elem, _Traits >& output, _Int&& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	do_binary_print( output, value );
	
	return output;
}

// Don't allow const types when reading in.
template < class _Int, class _Elem, class _Traits >
inline std::basic_istream< _Elem, _Traits >& binary( std::basic_istream< _Elem, _Traits >& input, const _Int& value ) = delete;

template < class _Int, class _Elem, class _Traits >
inline void do_binary_read( std::basic_istream< _Elem, _Traits >& input, _Int& value )
{
	static_assert( std::is_integral_v< uint64_t >, "The function `do_binary_read` requires an integral type!" );
	
	constexpr char bits_per_byte = 8;
	
	// Prepare the input stream for a read operation, if a failure occurs return instantly.
	typename std::basic_istream< char >::sentry my_sentry( input );
	if ( !my_sentry ) return;

	std::basic_streambuf< char >* stream_buffer = input.rdbuf();
	value = 0;
	
	for ( int count = 0; count < sizeof( uint64_t ) * bits_per_byte; ++count )
	{
		switch ( stream_buffer->sgetc() )
		{
		case '0':
			value = value << 1; // Shift all bits left 1
			break;
		case '1':
			value = ( value << 1 ) + 1; // Shift all bits left 1 and add 1.
			break;

		case ' ':
		case '\n':
			return;

		case std::char_traits< char >::eof():
			input.setstate( ( count == 0 ) ? std::ios_base::eofbit | std::ios_base::failbit : std::ios_base::eofbit );
			return;

		default:
			input.setstate( std::ios_base::failbit );
			value = 0;
			return;
		}
		stream_buffer->sbumpc();
	}
	
	return;
}

// Read a binary integer from the input stream.
template < class _Int, class _Elem, class _Traits >
inline std::basic_istream< _Elem, _Traits >& binary( std::basic_istream< _Elem, _Traits >& input, _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	constexpr char bits_per_byte = 8;
	
	// Prepare the input stream for a read operation, if a failure occurs return instantly.
	typename std::basic_istream< _Elem, _Traits >::sentry my_sentry( input );
	if ( !my_sentry ) return input;

	std::basic_streambuf< _Elem, _Traits >* stream_buffer = input.rdbuf();
	value = 0;
	
	for ( int count = 0; count < sizeof( _Int ) * bits_per_byte; ++count )
	{
		switch ( _Traits::to_char_type( stream_buffer->sgetc() ) )
		{
		// Recieved a 0 or 1 binary notation.
		case static_cast< _Elem >( '0' ):
			value = value << 1; // Shift all bits left 1
			break;
		case static_cast< _Elem >( '1' ):
			value = ( value << 1 ) + 1; // Shift all bits left 1 and add 1.
			break;

		// Natural end of a binary input, return.
		case static_cast< _Elem >( ' ' ):
		case static_cast< _Elem >( '\n' ):
			return input;

		// End of file, if we read nothing, set failbit and eofbit, otherwise just set eofbit.
		case _Traits::eof():
			input.setstate( ( count == 0 ) ? std::ios_base::eofbit | std::ios_base::failbit : std::ios_base::eofbit );
			return input;

		// Unnatural end of binary input, reached an invalid character.
		default:
			input.setstate( std::ios_base::failbit );
			value = 0;
			return input;
		}
		stream_buffer->sbumpc();
	}
	
	// Reached maximum depth of bits for the type of `_Int`, return.
	return input;
}

// Struct to capture the binary reference to perform a standard operator << or >> read/write.
template < class _Int >
struct binary_streamable
{
	static_assert( std::is_integral_v< _Int >, "The struct `binary_streamable` requires an integral type!" );
	
	template < class _Int_2, class _Char, class _Traits >
	friend std::basic_ostream< _Char, _Traits >& operator << ( std::basic_ostream< _Char, _Traits >& stream, binary_streamable< _Int_2 >&& obj )
	{ return binary( stream, obj.value ); }

	template < class _Int_2, class _Char, class _Traits >
	friend std::basic_istream< _Char, _Traits >& operator >> ( std::basic_istream< _Char, _Traits >& stream, binary_streamable< _Int_2 >&& obj )
	{ return binary( stream, obj.value ); }

	_Int& value;
};

// Create an object that will read/print the number in binary format to the stream.
template < class _Int >
inline binary_streamable< _Int > binary( _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	return binary_streamable< _Int >{ value };
}

// Struct to capture a constant copy of the integer to print using the standard operator << write method.
template < class _Int >
struct binary_printable
{
	static_assert( std::is_integral_v< _Int >, "The struct `binary_printable` requires an integral type!" );
	
	template < class _Int_2, class _Char, class _Traits >
	friend std::basic_ostream< _Char, _Traits >& operator << ( std::basic_ostream< _Char, _Traits >& stream, binary_printable< _Int_2 >&& obj )
	{ return binary( stream, obj.value ); }

	const _Int value;
};

// Print a binary value to an output stream.
template < class _Int >
inline binary_printable< _Int > binary( const _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	return binary_printable< _Int >{ value };
}

// Print a binary value to an output stream.
template < class _Int >
inline binary_printable< _Int > binary( _Int&& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	return binary_printable< _Int >{ value };
}

/*
 - @}
 -
 - @section `binary` I/O facet(s)
 - @{
 */

 // TODO: More research on facets, and then create one that can override std::num_get and std::num_put for our binary printing so we can use it to set binary to be the default.

 /*
  - @}
  */
#endif // IO_FUNCTIONS_HPP