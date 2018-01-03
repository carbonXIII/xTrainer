/*
 * tools.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: shado
 */

#include "tools.h"
#include <fstream>

bool contains(const char* str, const char* key){
	int n = strlen(str);
	int m = strlen(key);

	for(int i = 0; i < n - m; i++){
		int j = 0;
		for(; j < m; j++){
			if(tolower(str[i+j]) != tolower(key[j]))break;
		}if(j == m)return true;
	}return false;
}

bool fileExists(const char* filename){
	std::ifstream fin(filename);
	return fin.good();
}
