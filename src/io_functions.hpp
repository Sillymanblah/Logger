#pragma once
#ifndef IO_FUNCTIONS_HPP
#define IO_FUNCTIONS_HPP

// Standard library files
#include <ostream>
#include <istream>
#include <cstdint>

template < class _Arg, class _Elem, class _Traits >
class istream_capture;

template < class _Arg, class _Elem, class _Traits >
istream_capture< _Arg, _Elem, _Traits > operator >> ( std::basic_istream< _Elem, _Traits >& stream, void ( * function)( std::basic_ostream< _Elem, _Traits >&, _Arg ) );

template < class _Arg, class _Elem, class _Traits >
class istream_capture
{
public:
	using istream		= std::basic_istream< _Elem, _Traits >;
	using function_arg	= _Arg;
	using function_ptr	= void (*)( istream&, function_arg );

private:
	istream_capture() = delete;
	istream_capture( istream& stream, function_ptr function ) : function( function ), stream( stream ) {}

public:
	template < class _Arg_new, class _Elem_new, class _Traits_new >
	friend istream_capture< _Arg_new, _Elem_new, _Traits_new > operator >> ( std::basic_istream< _Elem_new, _Traits_new >& stream, void ( * function)( std::basic_ostream< _Elem, _Traits >&, _Arg_new ) )
	{ return istream_capture< _Arg_new, _Elem_new, _Traits_new >( stream, function ); }

	istream& operator >> ( function_arg& argument )
	{
		function( stream, argument );
		return stream;
	}
	istream& operator >> ( function_arg&& argument )
	{
		function( stream, argument );
		return stream;
	}

private:
	function_ptr function;
	istream& stream;
};

template < class _Arg, class _Elem, class _Traits >
class ostream_capture;

template < class _Arg, class _Elem, class _Traits >
ostream_capture< _Arg, _Elem, _Traits > operator << ( std::basic_ostream< _Elem, _Traits >& stream, void ( * function )( std::basic_ostream< _Elem, _Traits >&, _Arg ) );

template < class _Arg, class _Elem, class _Traits >
class ostream_capture
{
public:
	using ostream		= std::basic_ostream< _Elem, _Traits >;
	using function_arg	= _Arg;
	using function_ptr	= void (*)( ostream&, function_arg );

private:
	ostream_capture() = delete;
	ostream_capture( ostream& stream, function_ptr function ) : function( function ), stream( stream ) {}
	
public:
	template < class _Arg_new, class _Elem_new, class _Traits_new >
	friend ostream_capture< _Arg_new, _Elem_new, _Traits_new > operator << ( std::basic_ostream< _Elem_new, _Traits_new >& stream, void ( * function )( std::basic_ostream< _Elem_new, _Traits_new >&, _Arg_new ) )
	{ return ostream_capture< _Arg_new, _Elem_new, _Traits_new >( stream, function ); }
	
	ostream& operator << ( const function_arg& argument )
	{
		function( stream, argument );
		return stream;
	}
	ostream& operator << ( function_arg&& argument )
	{
		function( stream, argument );
		return stream;
	}
	
private:
	function_ptr function;
	ostream& stream;
};

// Unimplemented so far
template < class _Arg, class _Elem, class _Traits = std::char_traits< _Elem > >
class iostream_capture
{
	static_assert( false, "This class has not been implemented yet!" );
};

// The functions are provided how they are because they provide the ability to perform type-deduction.
// If we were to use template functions (which I had tried), trying to use the function:
// `friend ostream_capture< _Arg_new, _Elem_new, _Traits_new > operator << ( std::basic_ostream< _Elem_new, _Traits_new >& stream, void ( * function )( std::basic_ostream< _Elem_new, _Traits_new >&, _Arg_new ) )`
// Would fail type deduction even though it should be possible to resolve using the same types from the ostream to match the selected template function.

// Prints a `uint8_t` to the the `std::ostream&` in binary format.
void u8_binary( std::ostream& output, uint8_t value );
// Prints a `int8_t` to the the `std::ostream&` in binary format.
void i8_binary( std::ostream& output, int8_t value );
// Prints a `uint16_t` to the the `std::ostream&` in binary format.
void u16_binary( std::ostream& output, uint16_t value );
// Prints a `int16_t` to the the `std::ostream&` in binary format.
void i16_binary( std::ostream& output, int16_t value );
// Prints a `uint32_t` to the the `std::ostream&` in binary format.
void u32_binary( std::ostream& output, uint32_t value );
// Prints a `int32_t` to the the `std::ostream&` in binary format.
void i32_binary( std::ostream& output, int32_t value );
// Prints a `uint64_t` to the the `std::ostream&` in binary format.
void u64_binary( std::ostream& output, uint64_t value );
// Prints a `int64_t` to the the `std::ostream&` in binary format.
void i64_binary( std::ostream& output, int64_t value );

// Standardized function that can handle all unsigned integral types by converting them to the `uint64_t` type and then printing them in binary format to the `std::ostream&`.
void ubinary( std::ostream& output, uint64_t value );
// Standardized function that can handle all signed integral types by converting them to the `int64_t` type and then printing them in binary format to the `std::ostream&`.
void binary( std::ostream& output, int64_t value );

// Prints a `uint8_t` to the the `std::wostream&` in binary format.
void u8_binary( std::wostream& output, uint8_t value );
// Prints a `int8_t` to the the `std::wostream&` in binary format.
void i8_binary( std::wostream& output, int8_t value );
// Prints a `uint16_t` to the the `std::wostream&` in binary format.
void u16_binary( std::wostream& output, uint16_t value );
// Prints a `int16_t` to the the `std::wostream&` in binary format.
void i16_binary( std::wostream& output, int16_t value );
// Prints a `uint32_t` to the the `std::wostream&` in binary format.
void u32_binary( std::wostream& output, uint32_t value );
// Prints a `int32_t` to the the `std::wostream&` in binary format.
void i32_binary( std::wostream& output, int32_t value );
// Prints a `uint64_t` to the the `std::wostream&` in binary format.
void u64_binary( std::wostream& output, uint64_t value );
// Prints a `int64_t` to the the `std::wostream&` in binary format.
void i64_binary( std::wostream& output, int64_t value );

// Standardized function that can handle all unsigned integral types by converting them to the `uint64_t` type and then printing them in binary format to the `std::wostream&`.
void ubinary( std::wostream& output, uint64_t value );
// Standardized function that can handle all signed integral types by converting them to the `int64_t` type and then printing them in binary format to the `std::wostream&`.
void binary( std::wostream& output, int64_t value );

#endif // IO_FUNCTIONS_HPP