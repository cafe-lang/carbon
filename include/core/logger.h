//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h> // for va_list
#include "console.h"

namespace carbon {

class Logger {
public:
	enum LogLevel 
	{
		JUST_LOG = 0,

		VERBOSE = 1,
		INFO    = 2,
		SUCCESS = 3,
		WARNING = 4,
		ERROR   = 5,
	};

	static void set_level(LogLevel p_level);
	static void reset_level();
	static LogLevel get_level();
	static bool is_level(LogLevel p_level);
	
	static void log(const char* p_msg, Console::Color p_fg = Console::Color::L_WHITE, Console::Color p_bg = Console::Color::BLACK);
	static void log(const char* p_msg, LogLevel p_level, Console::Color p_fg = Console::Color::L_WHITE, Console::Color p_bg = Console::Color::BLACK);

	static void log_verbose(const char* p_msg);
	static void log_info(const char* p_msg);
	static void log_success(const char* p_msg);
	static void log_warning(const char* p_msg);
	static void log_error(const char* p_msg);

	static void logf_verbose(const char* p_fmt, ...);
	static void logf_info(const char* p_fmt, ...);
	static void logf_success(const char* p_fmt, ...);
	static void logf_warning(const char* p_fmt, ...);
	static void logf_error(const char* p_fmt, ...);

	static void initialize();
	static void cleanup();

protected:
	virtual void log_impl(const char* p_msg, Console::Color p_fg, Console::Color p_bg) const = 0;

	virtual void log_verbose_impl(const char* p_msg) const = 0;
	virtual void log_info_impl(const char* p_msg) const = 0;
	virtual void log_success_impl(const char* p_msg) const = 0;
	virtual void log_warning_impl(const char* p_msg) const = 0;
	virtual void log_error_impl(const char* p_msg) const = 0;

	virtual void logf_verbose_impl(const char* p_fmt, va_list p_list) const = 0;
	virtual void logf_info_impl(const char* p_fmt, va_list p_list) const = 0;
	virtual void logf_success_impl(const char* p_fmt, va_list p_list) const = 0;
	virtual void logf_warning_impl(const char* p_fmt, va_list p_list) const = 0;
	virtual void logf_error_impl(const char* p_fmt, va_list p_list) const = 0;

private:
	static Logger* singleton;
	static LogLevel level;
	static LogLevel last_level;
};

}

#endif // LOGGER_H