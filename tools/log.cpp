/*
 * log.cpp
 *
 *  Created on: Dec 18, 2017
 *      Author: Jordan Richards
 */
#include "log.h"

Logger default_logger;

Logger& _log(Logger& logger, const char* func_name, const char* file_name, long line_number){
	return (logger << "[ " << func_name << ", " << file_name << ", line " << line_number << " ] ");
}
