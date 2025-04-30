// Our Files
#include "../io_functions.hpp"

// STL Files
#include <iostream>
#include <iomanip>

template < class _Elem, class _Traits >
void test_binary_read( std::basic_istream< _Elem, _Traits >& input );

int main()
{
	unsigned long long ull_out = 10561366137;
	unsigned long long ull_in = 15363523;
	const int ci_out = 15346;
	const int ci_in = 396344;
	const short cs_out = 1364;
	const short cs_in = 2345;
	char char_out = 53;
	char char_in = 31;

	std::cout << binary( 100 ) << '\n';
	std::cout << binary( ull_out ) << '\n';
	std::cin >> binary( ull_in );
	std::cout << ull_in << '\n';
	std::cout << binary( ci_out ) << '\n';
	// std::cin >> binary( ci_in ); // This line would throw errors.

	binary( std::cout, cs_out ); std::cout << '\n';
	// binary( std::cin, cs_in ); // This line would also throw errors.
	binary( std::cout, 15 ); std::cout << '\n';
	binary( std::cin, char_in );
	std::cout << char_in;
	binary( std::cout, char_out ); std::cout << '\n';

	std::cout << std::showbase << binary( 100 ) << '\n';
	std::cout << std::uppercase << binary( 100 ) << '\n';
	std::cout << std::setw( 20 ) << binary( 100 ) << '\n';
	std::cout << std::setw( 20 ) << std::left << binary( 100 ) << '\n';
	std::cout << std::setw( 20 ) << std::internal << binary( 100 ) << '\n';
}