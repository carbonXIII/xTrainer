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

//Big-Endian Support Functions
extern float reverseFloat(char* p);
extern unsigned reverseUInt16(char* p);
extern unsigned long reverseUInt32(char* p);
extern unsigned long long reverseUInt64(char* p);

extern bool contains(const char* str, const char* key);//basic string search

#endif /* TOOLS_H_ */
