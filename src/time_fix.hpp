#pragma once
#ifndef TIME_FIX_HPP
#define TIME_FIX_HPP

/// STL
#include <ctime>

namespace standardized
{
	#if _MSC_VER

		#if _MSVC_LANG >= 201103L
		std::tm* localtime_s( std::time_t* source, std::tm* destination );
		#endif

		#if _MSVC_LANG >= 202302L
		std::tm* localtime_r( std::time_t* restrict source, std::tm* restrict destination );
		#endif

	#else // ^^^ _MSC_VER / !_MSC_VER vvv 

		#if __cplusplus >= 201103L
		std::tm* localtime_s( std::time_t* source, std::tm* destination );
		#endif

		#if _cplusplus >= 202302L
		std::tm* localtime_r( std::time_t* restrict source, std::tm* restrict destination );
		#endif

	#endif // !_MSC_VER
}
// namespace standardized

#endif // TIME_FIX_HPP