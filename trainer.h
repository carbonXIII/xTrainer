/*
 * trainer.h
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#ifndef TRAINER_H_
#define TRAINER_H_

#include <vector>
#include <string>

#if (defined (_WIN32) || defined (_WIN64))
	#include <windows.h>
#else
	#include <sys/uio.h>
#endif

extern std::vector<std::pair<unsigned long, std::string>> enumerateProcesses(const char* keyword);

struct PageQuery{
	size_t size = -1;
	std::string title;
	std::vector<size_t> containedAddresses;
};

struct PageInfo{
	PageInfo(void* startAddr, void* endAddr);
	PageInfo(void* startAddr, size_t size);
	PageInfo(std::string range);

	size_t size() const { return (size_t)endAddr - (size_t)startAddr; }
	void* startAddr = 0;
	void* endAddr = 0;
};

struct Process{
	Process(unsigned long pid);

	size_t readBytes(void* addr, void* buffer, size_t size);
	size_t writeBytes(void* addr, void* buffer, size_t size);

	std::vector<PageInfo> queryPages(PageQuery* description);

	unsigned long getPID() const { return pid; };

	~Process();

private:
	unsigned long pid;

	void* proc;//only used on windows

	std::vector<PageInfo> getPageList();
};

#endif /* TRAINER_H_ */
