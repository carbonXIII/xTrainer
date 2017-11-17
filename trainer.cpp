/*
 * trainer.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#include "trainer.h"
#include "tools.h"

#include <windows.h>
#include <psapi.h>
#include <conio.h>

using namespace std;

std::vector<std::pair<size_t, void*>> getPages(HANDLE proc, size_t keysize, std::string keyword, std::vector<size_t> keyaddr){
	vector<pair<size_t, void*>> ret;

	vector<char> buffer;

	MEMORY_BASIC_INFORMATION info;
	for(void* p = 0; VirtualQueryEx(proc, p, &info, sizeof(info)) == sizeof(info); p += info.RegionSize){
		if(keysize && info.RegionSize != keysize)continue;

		unsigned i;
		for(i = 0; i < keyaddr.size(); i++){
			if((size_t)info.BaseAddress > keyaddr[i] || (size_t)info.BaseAddress + info.RegionSize <= keyaddr[i])break;
		}if(i != keyaddr.size())continue;

		if(!keyword.empty()){
			buffer.resize(info.RegionSize);

			size_t read;
			ReadProcessMemory(proc, info.BaseAddress, buffer.data(), info.RegionSize, &read);

			unsigned i;
			for(i = 0; i < keyword.size(); i++) if(keyword[i] != buffer[i])break;
			if(i != keyword.size())continue;
		}


		ret.push_back(make_pair(info.RegionSize, info.BaseAddress));
	}

	return ret;
}

std::vector<std::pair<unsigned long, std::string>> findProcess(){
	vector<pair<unsigned long, string>> ret;

	vector<unsigned long> buffer(100);
	char strbuf[100];

	unsigned long size;
	do{
		buffer.resize(buffer.size() + 100);
		EnumProcesses(buffer.data(), buffer.size()*sizeof(unsigned long), &size);
	}while(size/sizeof(unsigned long) == buffer.size());
	buffer.resize(size/sizeof(unsigned long));

	for(int i = 0; i < buffer.size(); i++){
		HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, buffer[i]);

		unsigned long strSize;
		if(h && GetProcessImageFileNameA(h, strbuf, 100) && contains(strbuf, "dolphin")){
			ret.push_back(make_pair(buffer[i], string(strbuf)));
		}
	}

	return ret;
}
