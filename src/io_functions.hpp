#pragma once
#ifndef IO_FUNCTIONS_HPP
#define IO_FUNCTIONS_HPP

// STL Files
#include <ostream>
#include <istream>

// Print a binary value to the output stream.
template < class _Int, class _Elem, class _Traits >
inline std::basic_ostream< _Elem, _Traits >& binary( std::basic_ostream< _Elem, _Traits >& output, const _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	constexpr char bits_per_byte = 8;
	
	for ( int bit = sizeof( _Int ) * bits_per_byte; bit > 0; )
		output << ( ( value >> --bit ) & 1 );
	
	return output;
}

template < class _Int, class _Elem, class _Traits >
inline std::basic_ostream< _Elem, _Traits >& binary( std::basic_ostream< _Elem, _Traits >& output, _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	constexpr char bits_per_byte = 8;
	
	for ( int bit = sizeof( _Int ) * bits_per_byte; bit > 0; )
		output << ( ( value >> --bit ) & 1 );
	
	return output;
}

// Print a binary value to the output stream.
template < class _Int, class _Elem, class _Traits >
inline std::basic_ostream< _Elem, _Traits >& binary( std::basic_ostream< _Elem, _Traits >& output, _Int&& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	constexpr char bits_per_byte = 8;
	
	for ( int bit = sizeof( _Int ) * bits_per_byte; bit > 0; )
		output << ( ( value >> --bit ) & 1 );
	
	return output;
}

// Don't allow const types when reading in.
template < class _Int, class _Elem, class _Traits >
inline std::basic_istream< _Elem, _Traits >& binary( std::basic_istream< _Elem, _Traits >& input, const _Int& value ) = delete;

// Read a binary integer from the input stream.
template < class _Int, class _Elem, class _Traits >
inline std::basic_istream< _Elem, _Traits >& binary( std::basic_istream< _Elem, _Traits >& input, _Int& value )
{
	static_assert( std::is_integral_v< _Int >, "The function `binary` requires an integral type!" );
	
	constexpr char bits_per_byte = 8;

	value = 0;
	_Elem bit;
	
	for ( int count = 0; count < sizeof( _Int ) * bits_per_byte; ++count )
	{
		input.get( bit );
		switch ( bit )
		{
		// Recieved a 0 or 1 binary notation.
		case static_cast< _Elem >( '0' ):
			value = value << 1; // Shift all bits left 1
			break;
		case static_cast< _Elem >( '1' ):
			value = ( value << 1 ) + 1; // Shift all bits left 1 and add 1.
			break;

		// Natural end of a set of binary input, return.
		case static_cast< _Elem >( ' ' ):
		case static_cast< _Elem >( '\n' ):
			return input;

		// Unnatural end of binary input, reached an invalid character.
		default:
			input.setstate( std::ios_base::failbit );
			value = 0;
			return input;
		}
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

#endif // IO_FUNCTIONS_HPP