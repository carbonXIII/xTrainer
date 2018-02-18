/*
 * tools.h
 *
 *  Created on: Nov 7, 2017
 *      Author: Jordan Richards
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include <tuple>
#include <cstring>
#include <iostream>
#include <vector>

#include "log.h"//Basic logging

namespace xtrainer{

//Big-endian to little endian conversion function
template <typename T> T reverse(T t);

//Memory helper function
template <typename T> T get(char* p, bool bigEndian);

extern bool fileExists(const char* filename);

template <typename T> void convert(std::string s, T& out);

} //namespace xtrainer

#include "tools-imp.h"

#endif /* TOOLS_H_ */
