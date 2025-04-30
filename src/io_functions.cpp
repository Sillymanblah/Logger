#include "io_functions.hpp"

size_t get_binary_cstring( char* buffer, size_t size, const uint64_t& value )
{
	bool ignore_zero = true; // Ignore leading zeros.

	size_t index = 0; // Index to write to the buffer.

	for ( size_t bit_shift = 0; bit_shift < size; ++bit_shift )
	{
		bool bit = ( ( value >> ( size - 1 - bit_shift ) ) & 1ULL ); // Get the bit at the current index.
		if ( bit )
		{
			buffer[ index++ ] = '1';
			ignore_zero = false;
		}
		else if ( !ignore_zero )
		{
			buffer[ index++ ] = '0';
		}
	}

	return index; // Return the number of bits written to the buffer.
}