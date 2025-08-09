#include "log_base.hpp"
#include "logstream.hpp"

logstream log_base::my_log;

log_settings log_base::update_settings( log_settings new_settings )
{ this->settings = new_settings; }

log_settings log_base::enable_settings( log_settings new_settings )
{ this->settings.enable( new_settings ); }

log_settings log_base::toggle_settings( log_settings new_settings )
{ this->settings.toggle( new_settings ); }

log_settings log_base::disable_settings( log_settings new_settings )
{ this->settings.disable( new_settings ); }