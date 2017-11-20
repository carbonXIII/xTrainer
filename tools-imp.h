/*
 * tools.cpp
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */
#ifndef TOOLS_IMP_H_
#define TOOLS_IMP_H_

#include <cstring>

template <typename T>
T reverse(T t){
	char* inBytes = (char*)&t;

	T ret;
	char* retBytes = (char*)&ret;

	for(int i = 0; i < sizeof(T); i++){
		retBytes[i] = inBytes[sizeof(T) - 1];
	}

	return ret;
}

template <typename T>
T get(char* p, bool bigEndian){
	T ret = *(reinterpret_cast<T*>(p));
	return bigEndian ? reverse(ret) : ret;
}

#endif
