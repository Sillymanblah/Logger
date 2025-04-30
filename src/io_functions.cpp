#include "io_functions.hpp"

void get_binary_cstring( char* buffer, size_t size, const uint64_t& value )
{
	for ( size_t index = 0; index < size; ++index )
		buffer[ index ] = ( ( value >> index ) & 1ULL ) ? '1' : '0';
}