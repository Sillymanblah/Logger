
#pragma once
#ifndef LOG_SETTINGS_HPP
#define LOG_SETTINGS_HPP

#include <cstdint>

/// @brief Developer settings are individually toggleable.
enum class developer_settings : uint8_t
{
	none = 0,					///< No dev settings enabled, each has to be explicitly enabled.
	trace = 1 << 0,				///< Enable only trace messages.
	debug = 1 << 1,				///< Enable only debug messages.
	all_dev = trace | debug,	///< Enable both types of messages.
};

constexpr size_t developer_settings_bits = 2;

/// @brief Runtime settings are treated as level settings, i.e. everything below the current setting is on automatically.
enum class system_settings : uint8_t
{
	// System settings
	none = 0,							///< Disable all system settings.
	info = 1 << 0,						///< Enable informational logs.
	warn = 1 << 1,						///< Enable warnings.
	error = 1 << 2,						///< Enable non-fatal errors.
	fatal = 1 << 3,						///< Enable fatal errors.
	non_errors = info | warn,			///< Anything that is not an error.
	non_fatal = non_errors | error,		///< Anything that is not a fatal error.
	errors = error | fatal,				///< Errors only (fatal and non-fatal).
	issues = warn | errors,				///< Problems with the program.
	all_sys = non_errors | errors,		///< All system logs enabled.
	suppress_warnings = info | errors,	///< Informational and errors only.
	non_fatal_issues = warn | error,	///< Any issues that are not fatal
};

constexpr size_t system_settings_bits = 4;

/// @brief Logging level is used to pass the current level being logged into the `basic_logstream` object to start a `basic_logstream_log`.
enum class logging_level : uint8_t
{
	trace, // Follow the program path, trace to see which functions are called.
	debug, // State information tracking to try to debug potential errors.
	info, // Logging information to the user for events or state changes.
	warn, // Emitted when there is an issue in the state that does not affect program flow.
	error, // Issues that are dangerous to the program and cause some parts to be unable to execute.
	fatal, // Problems that result in a total program failure (either gracefully or ungracefully).
};

/// @brief Helper function to get the associated logging level string for the logging levels.
///
/// @param level The level to get the string for.
/// @return A pointer to a character array containing the logging level name, or an empty string if an invalid value is passed in.
inline constexpr const char* level_string( logging_level level )
{
	switch ( level )
	{
		case logging_level::trace: return "TRACE";
		case logging_level::debug: return "DEBUG";
		case logging_level::info: return "INFO";
		case logging_level::warn: return "WARN";
		case logging_level::error: return "ERROR";
		case logging_level::fatal: return "FATAL";
		default: return "";
	}
}

/// @brief A class to wrap the concept of logging settings and provide methods to check logging level flags.
///
/// @details This class provides a single constructor to create the logging settings from the two parts @ref developer_settings and @ref system_settings.
/// With that data, we expose several methods
class log_settings
{
private:
	/// @brief Constructs a @ref log_settings object from the raw data, used for efficiency with the bitwise operators.
	/// @param raw The raw settings data to store in this object.
	log_settings( uint8_t raw );

public:
	/// @brief Constructs a @ref log_settings with no settings enabled.
	log_settings();

	/// @brief Constructs a @ref log_settings object from the @ref developer_settings and @ref system_settings.
	///
	/// @param dev The `developer_settings` to enable for this `log_settings`.
	/// @param sys The `system_settings` to enable for this `log_settings`. 
	log_settings( developer_settings dev, system_settings sys );

	/// @brief Constructs a @ref log_settings object from the @ref system_settings and @ref developer_settings.
	///
	/// @param sys The `system_settings` to enable for this `log_settings`. 
	/// @param dev The `developer_settings` to enable for this `log_settings`.
	log_settings( system_settings sys, developer_settings dev );

	/// @brief Constructs a @ref log_settings object with only the @ref system_settings.
	///
	/// @param sys The `system_settings` to enable for this `log_settings`. 
	log_settings( system_settings sys );

	/// @brief Constructs a @ref log_settings object with only the @ref developer_settings.
	///
	/// @param dev The `developer_settings` to enable for this `log_settings`.
	log_settings( developer_settings dev );

	/// @brief Copy constructor
	/// @param other Another `log_settings` to copy the value of.
	log_settings( const log_settings& other ) = default;

	/// @brief Move constructor
	/// @param other Another `log_settings` to move the value from.
	log_settings( log_settings&& other ) = default;

	/// @brief Copy assigner
	/// @param other Another `log_settings` to copy the value of.
	log_settings& operator = ( const log_settings& other ) = default;

	/// @brief Move assigner
	/// @param other Another `log_settings` to move the value from.
	log_settings& operator = ( log_settings&& other ) = default;
	
private:
	/// @brief Checks if the raw logging level data exists in our settings by doing a bitmask comparison. 
	///
	/// @param raw_level A `uint8_t` value with a single bit toggled on representing the `logging_level` we want to check.
	/// @return A boolean value `true` if the same bit is active for this settings, otherwise `false`.
	bool check( uint8_t raw_level );

public:
	/// @brief Checks if the logging level is valid and if it is currently set to active.
	///
	/// @param level The `logging_level` to check against our settings data.
	/// @return A boolean value indicating whether the logging level is active.
	bool is_set( logging_level level );

	/// @brief Behaves similar a bitwise `&` for the logging level with it's respective setting by delegating to @see[is_set]
	///
	/// @param level The `logging_level` to compare to our settings data.
	/// @return A boolean indicating whether the logging level is active.
	bool operator & ( logging_level level );

	/// @brief Performs bitwise or on the settings, this is identical to @ref `enable( log_settings )` except we create a new value.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to enable in `this` `log_settings`.
	/// @return A new `log_settings` with all enabled bits from `this` and `settings`.
	log_settings operator | ( log_settings settings );

	/// @brief Performs bitwise xor on the settings, this is identical to @ref `toggle( log_settings )` except we create a new value.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to toggle in `this` `log_settings`.
	/// @return A new `log_settings` with only the bits enabled in one of the two of `this` and `settings`.
	log_settings operator ^ ( log_settings settings );
	
	/// @brief Performs bitwise or on the settings, this is identical to @ref `disable( ~log_settings )` except we create a new value.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to disable in `this` `log_settings`.
	/// @return A new `log_settings` with all enabled bits from `this` except those enabled in `settings`.
	log_settings operator & ( log_settings settings );

	/// @brief Toggles all current settings for this logger, identical to how `operator ~` works.
	///
	/// @return A reference to `this` `log_settings` with all the settings inverted.
	log_settings& operator ~ ();

	/// @brief Performs bitwise or on the settings, this is identical to @ref `enable( log_settings )`.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to enable in `this` `log_settings`.
	/// @return A reference to `this` `log_settings` after being updated.
	log_settings& operator |= ( log_settings settings );

	/// @brief Performs bitwise xor on the settings, this is identical to @ref `toggle( log_settings )`.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to toggle in `this` `log_settings`.
	/// @return A reference to `this` `log_settings` after being updated.
	log_settings& operator ^= ( log_settings settings );

	/// @brief Performs bitwise or on the settings, this is identical to @ref `disable( ~log_settings )`.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to disable in `this` `log_settings`.
	/// @return A reference to `this` `log_settings` after being updated.
	log_settings& operator &= ( log_settings settings );

	/// @brief Enables all bits not currently enabled in this settings using the parameter as a bitmask.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to enable in `this` `log_settings`.
	/// @return A reference to `this`.
	log_settings& enable( log_settings settings );
	
	/// @brief Toggles all bits turned on in the parameter for `this` object.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to toggle in `this` `log_settings`.
	/// @return A reference to `this`.
	log_settings& toggle( log_settings settings );
	
	/// @brief Disables all bits currently enabled in this settings using the parameter as a bitmask.
	///
	/// @param settings The `log_settings` to use as a bitmask of settings to disable in `this` `log_settings`.
	/// @return A reference to `this`.
	log_settings& disable( log_settings settings );

	/// @brief Toggles all current settings for this logger, identical to how `operator ~` works.
	///
	/// @return A reference to `this` `log_settings` with all the settings inverted.
	log_settings& toggle();
	
private:
	/// @brief The data of the settings with accessors for it's raw data and the parts of the data.
	union
	{
		struct setting_data
		{
			developer_settings dev : developer_settings_bits;
			system_settings sys : system_settings_bits;
		} structured;

		uint8_t raw;
	} data;
};

/// @brief Helper function for combining dev and system settings.
///
/// @param dev The @ref `developer_settings` to enable.
/// @param sys The @ref `system_settings` to enable.
/// @return A @ref `log_settings` with the combination of the two.
log_settings operator + ( developer_settings dev, system_settings sys );

/// @brief Helper function for combining dev and system settings.
///
/// @param sys The @ref `system_settings` to enable.
/// @param dev The @ref `developer_settings` to enable.
/// @return A @ref `log_settings` with the combination of the two.
log_settings operator + ( system_settings sys, developer_settings dev );

#endif // LOG_SETTINGS_HPP