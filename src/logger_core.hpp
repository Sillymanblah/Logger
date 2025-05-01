#pragma once
#ifndef LOGGER_CORE_HPP
#define LOGGER_CORE_HPP

// Macros for DLL APIs
#if defined( _WIN32 ) && defined( LOGGER_USE_DLL )
	#define LOGGER_DLL_CALL __cdecl
	#if defined( LOGGER_BUILD )
		#define LOGGER_DLL_API __declspec( dllexport )
	#else // ^^^ defined( LOGGER_BUILD ) / !defined( LOGGER_BUILD ) vvv
		#define LOGGER_DLL_API __declspec( dllimport )
	#endif
#else // ^^^ defined( _WIN32 ) && defined( LOGGER_USE_DLL ) / !defined( _WIN32 ) && defined( LOGGER_USE_DLL ) vvv
	#define LOGGER_DLL_CALL
	#define LOGGER_DLL_API
#endif

// Macro to get the current line from the file, might not be safe to use with all build systems and compilers.
#define FILE_LINE "\'" __FILE__ "\' line " str(__LINE__)

// Type aliases that are typical for the program.
using BYTE = unsigned char;

#endif // LOGGER_CORE_HPP