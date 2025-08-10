/// DIRECT
#include "log_settings.hpp"

constexpr log_settings::log_settings() : data( 0 ) {}

constexpr log_settings::log_settings( developer_settings dev, system_settings sys ) : data( { dev, sys } ) {}

constexpr log_settings::log_settings( system_settings sys, developer_settings dev ) : data( { dev, sys } ) {}

constexpr log_settings::log_settings( system_settings sys ) : data( { developer_settings::none, sys } ) {}

constexpr log_settings::log_settings( developer_settings dev ) : data( { dev, system_settings::none } ) {}

bool log_settings::check( uint8_t raw_level ) { return this->data.raw & raw_level; }

bool log_settings::is_set( logging_level level ) { return check( 1 << static_cast< uint8_t >( level ) ); }

bool log_settings::operator & ( logging_level level ) { return is_set( level ); }

log_settings log_settings::operator | ( log_settings settings ) { return log_settings( this->data.raw | settings.data.raw ); }

log_settings log_settings::operator ^ ( log_settings settings ) { return log_settings( this->data.raw ^ settings.data.raw ); }

log_settings log_settings::operator & ( log_settings settings ) { return log_settings( this->data.raw & settings.data.raw ); }

log_settings log_settings::operator ~ () { return log_settings( ~this->data.raw ); }

log_settings& log_settings::operator |= ( log_settings settings )
{
	// Call the built in operator for integral types.
	this->data.raw |= settings.data.raw;

	// Return a reference to ourself.
	return *this;
}

log_settings& log_settings::operator ^= ( log_settings settings )
{
	// Call the built in operator for integral types.
	this->data.raw ^= settings.data.raw;

	// Return a reference to ourself.
	return *this;
}

log_settings& log_settings::operator &= ( log_settings settings )
{
	// Call the built in operator for integral types.
	this->data.raw &= settings.data.raw;

	// Return a reference to ourself.
	return *this;
}

log_settings& log_settings::enable( log_settings settings ) { return *this |= settings; }

log_settings& log_settings::toggle( log_settings settings ) { return *this ^= settings; }

log_settings& log_settings::disable( log_settings settings )
{
	/// Invert the settings to turn off the settings we want to enable and then call the and operator.
	this->data.raw &= ~settings.data.raw;
	
	/// Return a reference to ourself.
	return *this;
}

log_settings& log_settings::toggle() { return ~*this; }

log_settings operator + ( developer_settings dev, system_settings sys )
{ return log_settings( dev, sys ); }

log_settings operator + ( system_settings sys, developer_settings dev )
{ return log_settings( sys, dev ); }