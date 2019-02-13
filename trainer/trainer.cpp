/*
 * trainer.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: Jordan Richards
 */
#include "trainer.h"
#include "../tools/tools.h"

#include <fstream>
#include <cstring>
#include <limits>
#include <sstream>

using namespace std;

namespace xtrainer{

#ifndef TRAINER_LINUX
  std::vector<std::pair<unsigned long, std::string> > enumerateProcesses(const char* keyword){
    vector<pair<unsigned long, string> > ret;

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
#endif

  PageInfo::PageInfo(address_t startAddr, address_t endAddr): startAddr(startAddr), endAddr(endAddr) {}

  PageInfo::PageInfo(string range) {
    int b;
    for(int i = 0; i < range.size(); i++)if(range[i] == '-'){
        range[i] = 0;
        b = i + 1;
        break;
      }

    startAddr = strtoull(range.c_str() + 0,0,16);
    endAddr   = strtoull(range.c_str() + b,0,16);
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
    if(programName.empty()){
      baseAddress = 0;
      return;
    }

#ifndef TRAINER_LINUX
    HMODULE hMods[1024];
    DWORD size;

    if (EnumProcessModules(proc, hMods, 1024*sizeof(HMODULE), &size)){
      LOG << "Module count: " <<  size / sizeof(HMODULE) << '\n';
      for (unsigned i = 0; i < (size / sizeof(HMODULE)); i++){
        char buf[MAX_PATH];
        if (GetModuleFileNameEx(proc, hMods[i], buf, MAX_PATH)){
          if (string(buf).find(string(programName)) != string::npos){
            baseAddress = (address_t)hMods[i];
            return;
          }
        }
      }
    }
    baseAddress = 0;
#else
    char buf[64];
    sprintf(buf, "/proc/%ld/maps", pid);
    ifstream fin(buf);

    string line;
    string addr, path, trash;
    while(!fin.eof()){
      getline(fin,line);

      istringstream ss(line);

      //ignore 4 words
      ss >> addr >> trash >> trash >> trash >> trash >> path;

      cout << addr << " " << path << endl;
      if(path.find(string(programName)) != string::npos){
        baseAddress = PageInfo(addr).startAddr;
        return;
      }
    }
    baseAddress = 0;

    fin.close();
#endif
  }

  size_t Process::readBytes(address_t addr, void* buffer, size_t size){
    size_t read = 0;
    size_t errorNum = 0;

#ifndef TRAINER_LINUX
    if(!ReadProcessMemory(proc, (void*)addr, buffer, size, &read))
      errorNum = GetLastError();
#else
    iovec local, remote;
    local.iov_base = buffer;
    local.iov_len = size;

    remote.iov_base = (void*)addr;
    remote.iov_len = size;


    long long _read = process_vm_readv(pid, &local, 1, &remote, 1, 0);

    if(_read == -1)
      errorNum = errno;
    //else
		read = _read;
#endif
    if(errorNum){
      LOG << "Error while reading process memory: " << errorNum << "\n";
      LOG << "Foreign Address: " << addr << "\n";
      LOG << "Expected byte count: " << size << "\n";
    }

    return read;
  }

  size_t Process::writeBytes(address_t addr, const void* buffer, size_t size){
    size_t written = 0;
    size_t errorNum = 0;

#ifndef TRAINER_LINUX
    if(!WriteProcessMemory(proc, (void*)addr, buffer, size, &written))
      errorNum = GetLastError();
#else
    iovec local, remote;

    //the local buffer shouldn't be edited, but we need it to be non const
    local.iov_base = const_cast<void*>(buffer);
    local.iov_len = size;

    remote.iov_base = (void*)addr;
    remote.iov_len = size;


    written = process_vm_writev(pid, &local, 1, &remote, 1, 0);

    if(written == -1)
      errorNum = errno;
#endif
    if(errorNum){
      LOG << "Error while writing process memory: " << errorNum << "\n";
      LOG << "Foreign Address: " << addr << "\n";
      LOG << "Expected byte count: " << size << "\n";
    }

    return written;
  }

  address_t Process::allocateMemory(size_t length, MemAccess access){
#ifdef TRAINER_LINUX
    return 0;
#else
    long _access;

    if(access == RO){
      _access = PAGE_READONLY;
    }else if(access == RW){
      _access = PAGE_READWRITE;
    }else if(access == RE){
      _access = PAGE_EXECUTE_READ;
    }else if(access == RWE){
      _access = PAGE_EXECUTE_READWRITE;
    }else{
      LOG << "Unknown access type.\n";
      return 0;
    }

    return (address_t)VirtualAllocEx(getInternalHandle(), 0, length, MEM_COMMIT | MEM_RESERVE, _access);
#endif
  }

  bool Process::freeMemory(address_t addr){
#ifdef TRAINER_LINUX
    return false;
#else
    return VirtualFreeEx(getInternalHandle(), (void*)addr, 0, MEM_RELEASE) != 0;
#endif
  }

  address_t Process::uploadString(std::string& s, MemAccess access, bool null_term){
    address_t ret = allocateMemory(s.size() + null_term, access);
    writeBytes(ret, &s[0], s.size());
    if(null_term)writeBytes(ret + s.size(), "\0", 1);
    return ret;
  }

  std::vector<PageInfo> Process::getPageList(){
    vector<PageInfo> ret;

#ifndef TRAINER_LINUX
    MEMORY_BASIC_INFORMATION info;
    for(address_t p = 0; VirtualQueryEx(proc, (void*)p, &info, sizeof(info)) == sizeof(info); p += info.RegionSize){
      ret.emplace_back((address_t)info.BaseAddress, (address_t)info.BaseAddress + info.RegionSize);
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
        address_t addr = query.containedAddresses[j].value;
        if(query.containedAddresses[j].relative)addr += baseAddress;
        if(pageInfo[i].startAddr > addr || pageInfo[i].endAddr <= addr)break;
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
        address_t addr = query.containedAddresses[j].value;
        if(query.containedAddresses[j].relative)addr += baseAddress;
        if(pageInfo[i].startAddr > addr || pageInfo[i].endAddr <= addr)break;
      }if(j != query.containedAddresses.size())continue;

      if(!query.title.empty()){
        size_t read = readBytes(pageInfo[i].startAddr, (void*)buffer.data(), query.title.size());
        if(read < query.title.size() || query.title != buffer)continue;
      }

      return pageInfo[i];
    }

    throw runtime_error("Could not find requested memory page.");
  }

  std::vector<long> Process::getThreads(){
    vector<long> ret;

#ifdef TRAINER_LINUX
    // TODO: get threads on linux
#else
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(!snapshot){
      LOG << "Failed to retreive snapshot, error " << GetLastError() << "\n";
      return ret;
    }

    THREADENTRY32 buf = {sizeof(THREADENTRY32)};
    if(!Thread32First(snapshot, &buf))
      return ret;

    do{
      if(buf.th32OwnerProcessID == getPID())ret.push_back(buf.th32ThreadID);
      buf.dwSize = sizeof(THREADENTRY32);
    }while(Thread32Next(snapshot, &buf));

    CloseHandle(snapshot);
#endif

    return ret;
  }

  void Process::startDebugging(){
#ifdef TRAINER_LINUX
#else
    DebugActiveProcess(getPID());
#endif
  }

  void Process::stopDebugging(){
#ifdef TRAINER_LINUX
#else
    DebugActiveProcessStop(getPID());
#endif
  }

}//namespace xtrainer
