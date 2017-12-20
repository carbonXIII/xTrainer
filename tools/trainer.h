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
#include <initializer_list>

#ifdef TRAINER_LINUX
	#include <sys/uio.h>
#else
	#include <windows.h>
#endif

extern std::vector<std::pair<unsigned long, std::string>> enumerateProcesses(const char* keyword);

struct PageQuery{
	PageQuery(): containedAddresses() {};
	PageQuery(std::string title, size_t size = 0, std::initializer_list<size_t> addresses = {}): size(size), title(title), containedAddresses(addresses) {}

	size_t size = 0;
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

	std::vector<PageInfo> queryPages(const PageQuery& description);
	PageInfo queryFirstPage(const PageQuery& description);

	unsigned long getPID() const { return pid; };

	~Process();

	std::vector<PageInfo> getPageList();

private:
	unsigned long pid;

	void* proc;//only used on windows


};

#endif /* TRAINER_H_ */
