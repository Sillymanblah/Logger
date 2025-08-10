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
	/// @brief Default constructor, initializes all class data to zero.
	log_base() = default;

	/// @brief Creates a new `log_base` with the passed-in settings active.
	///
	/// @param settings The @ref `log_settings` to set active for this logger.
	log_base( log_settings settings );

	/// @brief Copy constructor, deleted since the only thing that would make sense to copy would be the time, which is trivial to create a new log from the getter anyways.
	///
	/// @param other A constant reference to another `log_base` to copy.
	log_base( const log_base& other ) = delete;

	/// @brief Move constructor, moves all associated data with the object before its destruction.
	///
	/// @param other An r-value of the `log_base` to move.
	log_base( log_base&& other ) = default;

	/// @brief Copy assigner, deleted since the only thing that would make sense to copy would be the time, which is trivial to create a new log from the getter anyways.
	///
	/// @param other A constant reference to another `log_base` to copy.
	log_base& operator = ( const log_base& other ) = delete;

	/// @brief Move assigner, moves all associated data into our current object replacing our data before its destruction.
	///
	/// @param other An r-value of the `log_base` to move.
	log_base& operator = ( log_base&& other ) = default;

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
	/// @brief Tells whether the given logging level is logged.
	///
	/// @param level The @ref `logging_level` to check against the logger's settings, if this number is an invalid type for the enumeration, returns false.
	/// @return A boolean indicating whether the value is logged.
	bool is_logged( logging_level level );

	/// @brief Marks the start of a log by initializing our @ref `log_base::last_time` and @ref `log_base::last_time_data` with data for the current time.
	///
	/// @return A pointer to this object's `last_time_data`.
	std::tm* log_start();

public:
	/// @brief Get the settings associated with this log.
	///
	/// @return The settings for this log.
	log_settings get_settings();

	/// @brief Get the start time of the last log.
	///
	/// @return The stored `std::time_t` of the most recent log start time.
	std::time_t last_log();

private:
	/// @brief The settings associated with this logstream
	log_settings settings{};

	/// @brief The start time of the last log, used to store the time temporarily for `start_log` and `std::format`.
	std::time_t last_time{};

	/// @brief The time struct for the last log's start time, also used for the storage duration to push data to the stream.
	/// The primary difference is this member is not offered by a getter, one must create their own `std::tm` object if they need one.
	std::tm last_time_data{};

/// @section Static
/// @{
protected:
	/// @brief The log that all logs use for debugging their lifetime.
	static logstream my_log;

/// @}
};

#endif // LOG_BASE_HPP