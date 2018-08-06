/*
 * trainer.h
 *
 *  Created on: Nov 17, 2017
 *      Author: Jordan Richards
 */

#ifndef TRAINER_H_
#define TRAINER_H_

#include <vector>
#include <string>
#include <initializer_list>

//TODO: fix Linux support (it has probably never worked)

#ifdef TRAINER_LINUX
	#include <sys/uio.h>
#else
	#include <crtdefs.h>
	#include <windows.h>
	#include <winbase.h>
	#include <psapi.h>
  #include <TlHelp32.h>
#endif

namespace xtrainer{

extern std::vector<std::pair<unsigned long, std::string> > enumerateProcesses(const char* keyword);

struct Address{
	bool relative = false;
	size_t value;

	Address(bool relative, size_t value): relative(relative), value(value) {}
};

//Allows us to search for a page matching a certain criteria
struct PageQuery{
	PageQuery(): containedAddresses() {};
	PageQuery(std::string title, size_t size = 0, std::initializer_list<Address> addresses = {}): size(size), title(title), containedAddresses(addresses) {}

	size_t size = 0;//the desired size of the page (0 if we don't care)
	std::string title;//a string containing a byte array that we expect to find at the beginning of the desired page
	std::vector<Address> containedAddresses;//desired foreign addresses that should be in the page
};

//TODO: add support for other types of PageQuery, such as string searches (sort of like Cheat Engine)

//Stores the information for a page to allow us to allocate Pages (see MemoryManager.h)
struct PageInfo{
	PageInfo(void* startAddr, void* endAddr);
	PageInfo(void* startAddr, size_t size);
	PageInfo(std::string range);

	size_t size() const { return (size_t)endAddr - (size_t)startAddr; }
	void* startAddr = 0;
	void* endAddr = 0;
};

enum MemAccess {RO,/*Read Only*/
                RW,/*Read Write*/
                RE,/*Read and Exceute*/
                RWE};/*Read, Write, Execute*/

//Structuring holding Process information (on windows it also stores a HANDLE to the process)
struct Process{
	Process(unsigned long pid, std::string filename = "");

	size_t readBytes(void* addr, void* buffer, size_t size);
	size_t writeBytes(void* addr, const void* buffer, size_t size);
  void* allocateMemory(size_t length, MemAccess access);
  bool freeMemory(void* addr);

	//Query functions that take PageQuery objects to find appropriate Pages
	std::vector<PageInfo> queryPages(const PageQuery& description);
	PageInfo queryFirstPage(const PageQuery& description);//throws a runtime_exception if there are no pages

	unsigned long getPID() const { return pid; }
	void* getBaseAddress() const { return baseAddress; }
  std::vector<long> getThreads();

  void startDebugging();
  void stopDebugging();

  void* getInternalHandle() const { return proc; }//only useful on windows

	~Process();

	//returns a full list of PageInfo
	std::vector<PageInfo> getPageList();

private:
	void resolveBaseAddress(std::string filename);//called during constructor (requires filename to find the base page, or part of the filename)

	unsigned long pid;
	void* baseAddress;
	void* proc;//only used on windows
};

}//namespace xtrainer

#endif /* TRAINER_H_ */
