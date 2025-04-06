#include "io_functions.hpp"

template < class _Int, class _Elem, class _Traits >
inline void basic_binary( std::basic_ostream< _Elem, _Traits >& output, _Int value )
{
	static_assert( std::is_integral_v< _Int >, "Functions by the name `binary` requires an integral type!" );
	
	constexpr char bits_per_byte = 8;
	
	for ( int bit = sizeof( _Int ) * bits_per_byte; bit > 0; )
		output << ( ( value >> --bit ) & 1 );
}

void u8_binary( std::ostream& output, uint8_t value )		{ basic_binary( output, value ); }
void i8_binary( std::ostream& output, int8_t value )		{ basic_binary( output, value ); }
void u16_binary( std::ostream& output, uint16_t value )		{ basic_binary( output, value ); }
void i16_binary( std::ostream& output, int16_t value )		{ basic_binary( output, value ); }
void u32_binary( std::ostream& output, uint32_t value )		{ basic_binary( output, value ); }
void i32_binary( std::ostream& output, int32_t value )		{ basic_binary( output, value ); }
void u64_binary( std::ostream& output, uint64_t value )		{ basic_binary( output, value ); }
void i64_binary( std::ostream& output, int64_t value )		{ basic_binary( output, value ); }

void binary( std::ostream& output, int64_t value )			{ basic_binary( output, value ); }
void ubinary( std::ostream& output, uint64_t value )		{ basic_binary( output, value ); }

void u8_binary( std::wostream& output, uint8_t value )		{ basic_binary( output, value ); }
void i8_binary( std::wostream& output, int8_t value )		{ basic_binary( output, value ); }
void u16_binary( std::wostream& output, uint16_t value )	{ basic_binary( output, value ); }
void i16_binary( std::wostream& output, int16_t value )		{ basic_binary( output, value ); }
void u32_binary( std::wostream& output, uint32_t value )	{ basic_binary( output, value ); }
void i32_binary( std::wostream& output, int32_t value )		{ basic_binary( output, value ); }
void u64_binary( std::wostream& output, uint64_t value )	{ basic_binary( output, value ); }
void i64_binary( std::wostream& output, int64_t value )		{ basic_binary( output, value ); }

void binary( std::wostream& output, int64_t value )			{ basic_binary( output, value ); }
void ubinary( std::wostream& output, uint64_t value )		{ basic_binary( output, value ); }
