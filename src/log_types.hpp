#pragma once
#ifndef LOG_TYPES
#define LOG_TYPES

#include <type_traits>
#include <mutex>

// Forwarding declaration of logstream.
template < class my_char, class my_traits = std::char_traits< my_char >, class my_mutex = std::mutex, size_t my_size = 1024 >
class basic_logstream;

using logstream = basic_logstream< char >;
using wlogstream = basic_logstream< wchar_t >;

#endif // LOG_TYPES