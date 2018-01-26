/*
 * log.h
 *
 *  Created on: Dec 18, 2017
 *      Author: shado
 */

#ifndef TOOLS_LOG_H_
#define TOOLS_LOG_H_

#include <iostream>
#include <fstream>

//A simple logger that uses macros to prepend messages with the function signature
struct Logger{
	Logger() {}

	void useStdErr(bool use){
		_useStdErr = use;
	}

	void openLogFile(const char* filename){
		logFile.open(filename, std::ios_base::app | std::ios_base::out);
	}

	void closeLogFile(){
		logFile.close();
	}

	template <typename T>
	Logger& operator << (const T& a){
		if(_useStdErr)std::cerr << a;
		if(logFile.is_open())logFile << a;
		return *this;
	}

	~Logger(){
		closeLogFile();
	}

private:
	std::ofstream logFile;
	bool _useStdErr = false;
};

extern Logger default_logger;

//define function name macro based on compiler
#ifndef __FUNCTION_NAME__
	#ifdef __GNUG__//G++ (MinGW, Linux)
		#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
	#else
		#ifdef _MSC_VER//Visual Studio
			#define __FUNCTION_NAME__ __FUNCDNAME__
		#else
			#define __FUNCTION_NAME__ ""//Empty String if Unavailable
		#endif
	#endif
#endif

#if (defined (_WIN32) || defined (_WIN64))
#define __FILE_SEP__ "\\"
#else
#define __FILE_SEP__ "/"
#endif

#define __FILENAME__ strrchr(__FILE_SEP__ __FILE__, __FILE_SEP__[0]) + 1

#define LOG _log(default_logger, (__FUNCTION_NAME__),(__FILENAME__),(__LINE__))//Logging macro (captures function name, file name, and line number)
#define CLOG ( logger ) _log((logger), (__FUNCTION_NAME__),(__FILENAME__),(__LINE__))//Custom logger (function-like)

extern Logger& _log(Logger& logger, const char* func_name, const char* file_name, long line_number);//Logging helper function

#endif /* TOOLS_LOG_H_ */
