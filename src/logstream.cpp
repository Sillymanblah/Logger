#include "logstream.hpp"

// Verify that the log level settings are valid, can be any combination of the log levels.
const logger_base::level& LOGGER_DLL_CALL logger_base::verify_level_settings( const logger_base::level& level_settings )
{
	if ( level_settings > level::ALL_LEVELS ) throw std::invalid_argument( "These level settings are invalid!\n" );
	return level_settings;
}

// Verifies that the log settings are valid, can be any combination of the log settings.
const logger_base::setting& LOGGER_DLL_CALL logger_base::verify_logger_settings( const logger_base::setting& logger_settings )
{
	if ( logger_settings > setting::ALL_SET ) throw std::invalid_argument( "These logger settings are invalid!\n" );
	return logger_settings;
}

// Verifies that the log level used for a log is valid (logs must use one of the basic types).
const logger_base::level& LOGGER_DLL_CALL logger_base::verify_logging_level( const logger_base::level& logging_level )
{
	switch ( logging_level )
	{
	case level::DEBUG:
	case level::TRACE:
	case level::INFO:
	case level::WARN:
	case level::ERROR:
	case level::FATAL:
		return logging_level;
	
	default:
		throw std::invalid_argument( "Invalid logging level passed as argument to the logger!\n" );
	}
}

constexpr bool LOGGER_DLL_API LOGGER_DLL_CALL logger_base::is_contained( const BYTE& checked, const BYTE& base )
{ return ( checked & base ) == checked; }

bool LOGGER_DLL_API LOGGER_DLL_CALL logger_base::is_logging_level( const logger_base::level& logging_level ) const
{ return is_contained( logging_level, config.level ); }

bool LOGGER_DLL_API LOGGER_DLL_CALL logger_base::is_loggable( const logger_base::level& logging_level ) const
{ return verify_logging_level( logging_level ) && this->is_logging_level( logging_level ); }

bool LOGGER_DLL_API LOGGER_DLL_CALL logger_base::check_setting( const logger_base::setting& checked_settings ) const
{ return is_contained( checked_settings, config.settings ); }