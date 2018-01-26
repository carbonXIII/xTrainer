/*
 * trainer.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: Jordan Richards
 */
#include "trainer.h"
#include "../tools/tools.h"

#include <windows.h>
#include <winbase.h>
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

		unsigned long strSize = 100;
		if(h && QueryFullProcessImageNameA(h, 0, strbuf, &strSize) && string(strbuf).find(string(keyword)) != string::npos){
			ret.push_back(make_pair(buffer[i], string(strbuf)));
		}CloseHandle(h);
	}

	return ret;
}

PageInfo::PageInfo(void* startAddr, void* endAddr): startAddr(startAddr), endAddr(endAddr) {}
PageInfo::PageInfo(void* startAddr, size_t size): startAddr(startAddr), endAddr(startAddr + size) {}

PageInfo::PageInfo(string range) {
	int b;
	for(int i = 0; i < range.size(); i++)if(range[i] == '-'){
		range[i] = 0;
		b = i + 1;
		break;
	}

	startAddr = (void*)strtoull(range.c_str() + 0,0,16);
	endAddr   = (void*)strtoull(range.c_str() + b,0,16);
}

Process::Process(unsigned long pid, string filename): pid(pid){
#ifndef TRAINER_LINUX
	proc = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
#endif
	resolveBaseAddress(filename);
}

Process::~Process(){
#ifndef TRAINER_LINUX
	CloseHandle(proc);
#endif
}

void Process::resolveBaseAddress(string programName){
    HMODULE hMods[1024];
    DWORD size;

    if (EnumProcessModules(proc, hMods, 1024*sizeof(HMODULE), &size)){
        LOG << "Module count: " <<  size / sizeof(HMODULE) << '\n';
        for (unsigned i = 0; i < (size / sizeof(HMODULE)); i++){
            char buf[MAX_PATH];
            if (GetModuleFileNameEx(proc, hMods[i], buf, MAX_PATH)){
                if (string(buf).find(string(programName)) != string::npos){
                    baseAddress = hMods[i];
                    return;
                }
            }
        }
    }
	baseAddress = nullptr;
}

size_t Process::readBytes(void* addr, void* buffer, size_t size){
	size_t r = 0;

#ifndef TRAINER_LINUX
	if(!ReadProcessMemory(proc, addr, buffer, size, &r)){
		LOG << "Error while reading process memory: " << GetLastError() << '\n';
		LOG << "Memory address: " << addr << '\n';
		LOG << "Bytes read: " << r << "; Expected: " << size << '\n';
	}
#else
	iovec local, remote;
	local.base = buffer;
	local.size = size;

	remote.base = addr;
	remote.size = size;


	read = process_vm_readv(pid, &local, 1, &remote, 1, 0);
#endif

	return r;
}

size_t Process::writeBytes(void* addr, void* buffer, size_t size){
	size_t written = 0;

#ifndef TRAINER_LINUX
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

#ifndef TRAINER_LINUX
	MEMORY_BASIC_INFORMATION info;
	for(void* p = 0; VirtualQueryEx(proc, p, &info, sizeof(info)) == sizeof(info); p += info.RegionSize){
		ret.emplace_back(info.BaseAddress, info.RegionSize);
	}
#else
	char buf[64];
	sprintf(buf, "/proc/%ld/maps", pid);
	ifstream fin(buf);

	string waste;
	string range; fin >> range;
	while(fin >> range){
		ret.emplace_back(range);
		getline(fin, waste);//horrible way to seek to the next line
	}

	fin.close();
#endif

	return ret;
}

std::vector<PageInfo> Process::queryPages(const PageQuery& query){
	vector<PageInfo> ret;

	vector<PageInfo> pageInfo = getPageList();
	string buffer(query.title.size(), 0);

	for(int i = 0; i < pageInfo.size(); i++){
		if(query.size > 0 && pageInfo[i].size() != query.size)continue;

		size_t j;
		for(j = 0; j < query.containedAddresses.size(); j++){
			size_t addr = query.containedAddresses[j].value;
			if(query.containedAddresses[j].relative)addr += (size_t)baseAddress;
			if((size_t)pageInfo[i].startAddr > addr || (size_t)pageInfo[i].endAddr <= addr)break;
		}if(j != query.containedAddresses.size())continue;

		if(!query.title.empty()){
			size_t read = readBytes(pageInfo[i].startAddr, (void*)buffer.data(), query.title.size());
			if(read < query.title.size() || query.title != buffer)continue;
		}

		ret.push_back(pageInfo[i]);
	}

	return ret;
}

PageInfo Process::queryFirstPage(const PageQuery& query){
	vector<PageInfo> pageInfo = getPageList();
	string buffer(query.title.size(), 0);

	for(int i = 0; i < pageInfo.size(); i++){
		if(query.size > 0 && pageInfo[i].size() != query.size)continue;

		size_t j;
		for(j = 0; j < query.containedAddresses.size(); j++){
			size_t addr = query.containedAddresses[j].value;
			if(query.containedAddresses[j].relative)addr += (size_t)baseAddress;
			if((size_t)pageInfo[i].startAddr > addr || (size_t)pageInfo[i].endAddr <= addr)break;
		}if(j != query.containedAddresses.size())continue;

		if(!query.title.empty()){
			size_t read = readBytes(pageInfo[i].startAddr, (void*)buffer.data(), query.title.size());
			if(read < query.title.size() || query.title != buffer)continue;
		}

		return pageInfo[i];
	}

	throw runtime_error("Could not find requested memory page.");
}
