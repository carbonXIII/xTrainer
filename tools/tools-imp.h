/*
 * tools.cpp
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */
#ifndef TOOLS_IMP_H_
#define TOOLS_IMP_H_

#include <cstring>
#include <sstream>

template <typename T>
T reverse(T t){
	char* inBytes = (char*)&t;

	T ret;
	char* retBytes = (char*)&ret;

	for(int i = 0; i < sizeof(T); i++){
		retBytes[i] = inBytes[sizeof(T) - i - 1];
	}

	return ret;
}

template <typename T>
T get(char* p, bool bigEndian){
	T ret = *(reinterpret_cast<T*>(p));
	return bigEndian ? reverse(ret) : ret;
}

template <typename T>
struct Converter{
	void operator()(std::string a, T& b){
		std::istringstream ss(a);
		if(a.size() > 2 && a[0] == '0' && (a[1] == 'x' || a[1] == 'X'))ss >> std::hex >> b;
		else ss >> b;
	}
};

template <>
struct Converter<std::string>{
	void operator()(std::string a, std::string& b){
		b = a;
	}
};

template <typename T>
void convert(std::string s, T& out){
	Converter<T> c;
	c(s,out);
}

#endif
