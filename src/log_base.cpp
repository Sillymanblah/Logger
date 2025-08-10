#include "log_base.hpp"
#include "log_stream.hpp"
#include "time_fix.hpp"

logstream log_base::my_log = /* Add construction here */;

log_base::log_base( log_settings settings ) : settings( settings ) {}

log_settings log_base::update_settings( log_settings new_settings )
{
	// Get the previous settings.
	log_settings previous = this->settings;

	// Update the settings.
	this->settings = new_settings;
	
	// Return the old settings.
	return previous;
}

log_settings log_base::enable_settings( log_settings new_settings )
{
	// Get the previous settings.
	log_settings previous = this->settings;

	// Update the settings.
	this->settings.enable( new_settings );
	
	// Return the old settings.
	return previous;
}

log_settings log_base::toggle_settings( log_settings new_settings )
{
	// Get the previous settings.
	log_settings previous = this->settings;

	// Update the settings.
	this->settings.toggle( new_settings );
	
	// Return the old settings.
	return previous;
}

log_settings log_base::disable_settings( log_settings new_settings )
{
	// Get the previous settings.
	log_settings previous = this->settings;

	// Update the settings.
	this->settings.disable( new_settings );
	
	// Return the old settings.
	return previous;
}

bool log_base::is_logged( logging_level level )
{ return this->settings & level; }

std::tm* log_base::log_start()
{
	// Get the current time.
	this->last_time = std::time( NULL );
	
	// Convert it into the proper time format and return the pointer to the time data structure.
	return standardized::localtime_s( &this->last_time, &this->last_time_data );
}

log_settings log_base::get_settings()
{ return this->settings; }

std::time_t log_base::last_log()
{ return this->last_time; }