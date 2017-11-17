/*
 * tools.h
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include <crtdefs.h>
#include <tuple>

const size_t RAM_SIZE      = 0x2000000;
const size_t RAM_BASE_ADDR = 0x80000000;

//Memory functions (includes Big-Endian support)
extern float getFloat(char* p, bool bigEndian);
extern double getDouble(char* p, bool bigEndian);
extern unsigned getUInt16(char* p, bool bigEndian);
extern unsigned long getUInt32(char* p, bool bigEndian);
extern unsigned long long getUInt64(char* p, bool bigEndian);

extern bool contains(const char* str, const char* key);//basic string search

#endif /* TOOLS_H_ */
