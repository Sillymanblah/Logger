#pragma once
#ifndef LOG_DATA_HPP
#define LOG_DATA_HPP

// Our Files
#include "logger_core.hpp"

// STL Files
#include <string>

// Enumeration to capture all possible log levels allowed.
// This is used to determine the level of logging that is desired.
enum class log_level : BYTE
{
	DEBUG	= 1 << 0, 	// Marks debug logs.
	TRACE	= 1 << 1, 	// Marks debug logs.
	INFO	= 1 << 2,	// Marks information logs.
	WARN	= 1 << 3,	// Marks warning logs.
	ERROR	= 1 << 4,	// Marks error logs.
	FATAL	= 1 << 5,	// Marks fatal error logs.

	// The below values could be removed from the enum, but for now this will do.
	OFF		= 0,								// Sets no log levels as active, this is not useful unless you need to disable logging conditionally or temporarily.
	SYSTEM	= DEBUG | TRACE,					// Sets system debug and tracing logs as active, opposite of `log_level::DEFAULT`.
	DEFAULT	= INFO | WARN | ERROR | FATAL,	// Sets standard log levels active, opposite of `log_level::SYSTEM`.
	ISSUE	= WARN | ERROR | FATAL,			// Sets issue log levels as active (warnings, errors, and fatal errors).
	DANGER	= ERROR | FATAL,					// Sets only critical logs to active.
	ALL		= SYSTEM | DEFAULT,					// Sets all log levels to active.
};

// Structure for use with the operator << for logging, if desired.
template < class _Elem >
class log_data
{
public:
	log_data( log_level&&, const _Elem* msg ) : level( std::move( level ) ), msg( msg ) {}

	template < class _Traits, class _Alloc >
	log_data( log_level&&, const std::basic_string< _Elem, _Traits, _Alloc >& msg ) : level( std::move( level ) ), msg( msg.c_str() ) {}

	template < class _Traits, class _Alloc >
	log_data( log_level&&, const std::basic_string_view< _Elem, _Traits, _Alloc >& msg ) : level( std::move( level ) ), msg( msg.c_str() ) {}

	template < class _Traits = std::char_traits< _Elem > >
	friend class basic_logstream< _Elem, _Traits >;

private:
	log_level level;
	const _Elem* msg;
};

#endif // LOG_DATA_HPP