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
#include <fstream>
#include <cstring>

using namespace std;

std::vector<std::pair<unsigned long, std::string>> enumerateProcesses(const char* keyword){
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
		if(h && GetProcessImageFileNameA(h, strbuf, 100) && contains(strbuf, keyword)){
			ret.push_back(make_pair(buffer[i], string(strbuf)));
		}
	}

	return ret;
}

PageInfo::PageInfo(void* startAddr, void* endAddr): startAddr(startAddr), endAddr(endAddr){}
PageInfo::PageInfo(void* startAddr, size_t size): startAddr(startAddr), endAddr(startAddr + size){}

PageInfo::PageInfo(string range){
	int b;
	for(int i = 0; i < range.size(); i++)if(range[i] == '-'){
		range[i] = 0;
		b = i + 1;
		break;
	}

	startAddr = (void*)strtoull(range.c_str() + 0,0,16);
	endAddr   = (void*)strtoull(range.c_str() + b,0,16);
}

Process::Process(unsigned long pid): pid(pid){
#if (defined (_WIN32) || defined (_WIN64))
	proc = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
#endif
}

Process::~Process(){
#if (defined (_WIN32) || defined (_WIN64))
	CloseHandle(proc);
#endif
}

size_t Process::readBytes(void* addr, void* buffer, size_t size){
	size_t read;

#if (defined (_WIN32) || defined (_WIN64))
	ReadProcessMemory(proc, addr, buffer, size, &read);
#else
	iovec local, remote;
	local.base = buffer;
	local.size = size;

	remote.base = addr;
	remote.size = size;


	read = process_vm_readv(pid, &local, 1, &remote, 1, 0);
#endif

	return read;
}

size_t Process::writeBytes(void* addr, void* buffer, size_t size){
	size_t written;

#if (defined (_WIN32) || defined (_WIN64))
	WriteProcessMemory(proc, addr, buffer, size, &written);
#else
	iovec local, remote;
	local.base = buffer;
	local.size = size;

	remote.base = addr;
	remote.size = size;


	read = process_vm_writev(pid, &local, 1, &remote, 1, 0);
#endif

	return written;
}

std::vector<PageInfo> Process::getPageList(){
	vector<PageInfo> ret;

#if (defined (_WIN32) || defined (_WIN64))
	MEMORY_BASIC_INFORMATION info;
	for(void* p = 0; VirtualQueryEx(proc, p, &info, sizeof(info)) == sizeof(info); p += info.RegionSize){
		ret.emplace_back(info.BaseAddress, info.RegionSize);
	}
#else
	char buf[64];
	sprintf("/proc/%ld/maps", pid);
	ifstream fin(buf);

	string range; fin >> range;
	while(fin >> range){
		ret.emplace_back(range);
		string waste; getline(fin, waste);
	}

	fin.close();
#endif

	return ret;
}

std::vector<PageInfo> Process::queryPages(PageQuery* query){
	vector<PageInfo> ret;

	vector<PageInfo> pageInfo = getPageList();

	string buffer(query->title.size(), 0);

	for(int i = 0; i < pageInfo.size(); i++){
		if(query->size > 0 && pageInfo[i].size() != query->size)continue;

		unsigned j;
		for(j = 0; j < query->containedAddresses.size(); j++){
			if((size_t)pageInfo[i].endAddr > query->containedAddresses[j] && (size_t)pageInfo[i].endAddr <= query->containedAddresses[j])break;
		}if(j != query->containedAddresses.size())continue;

		if(!query->title.empty()){
			size_t read = readBytes(pageInfo[i].startAddr, (void*)buffer.data(), query->title.size());
			if(read < query->title.size() || query->title != buffer)continue;
		}

		ret.push_back(pageInfo[i]);
	}

	return ret;
}
