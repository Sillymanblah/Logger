#pragma once
#ifndef LOG_BASE_HPP
#define LOG_BASE_HPP

// Direct
#include "log_settings.hpp"
#include "log_types.hpp"

// If the user has not defined `LOGGING_DEBUG` to false and
#if !defined( LOGGING_DEBUG ) && !defined( NDEBUG )
// User definable macro to enable debugging logs for within the 
#define LOGGING_DEBUG true
#endif
/// @brief Base class of any logstream, which stores a logger to track log data about the logs themselves when running with `LOGGING_DEBUG` set to true.
class log_base
{
public:
	log_base() = default;
	log_base( const log_base& ) = default;
	log_base( log_base&& ) = default;
	log_base& operator = ( const log_base& ) = default;
	log_base& operator = ( log_base&& ) = default;

	/// @brief Updates the @ref log_settings for this logger.
	///
	/// @param new_settings The new `log_settings`.
	/// @return The previous `log_settings` that was replaced.
	log_settings update_settings( log_settings new_settings );

	/// @brief Enables the @ref log_settings for this logger.
	///
	/// @param new_settings The new `log_settings`.
	/// @return The previous `log_settings` that was replaced.
	log_settings enable_settings( log_settings new_settings );

	/// @brief Toggles the @ref log_settings for this logger.
	///
	/// @param new_settings The new `log_settings`.
	/// @return The previous `log_settings` that was replaced.
	log_settings toggle_settings( log_settings new_settings );

	/// @brief Toggles the @ref log_settings for this logger.
	///
	/// @param new_settings The new `log_settings`.
	/// @return The previous `log_settings` that was replaced.
	log_settings disable_settings( log_settings new_settings );

protected:
	bool is_logged( logging_level level );
	std::tm* log_start();

public:
	log_settings settings();
	std::time_t last_log();

private:
	/// @brief The settings associated with this logstream
	log_settings settings;

protected:
	/// @brief The start time of the last log, used to store the time temporarily for `start_log` and `std::format`.
	std::time_t last_time;

	/// @brief The time struct for the last log's start time, also used for the storage duration to push data to the stream.
	/// The primary difference is this member is not offered by a getter, one must create their own `std::tm` object if they need one.
	std::tm last_time_data;

/// @section Static
/// @{
public:
	/// @brief The log that all logs use for debugging their lifetime.
	static logstream my_log;

/// @}
};

#endif // LOG_BASE_HPP