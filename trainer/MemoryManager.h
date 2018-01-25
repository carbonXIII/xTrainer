/*
 * MemoryManager.h
 *
 *  Created on: Jan 24, 2018
 *      Author: shado
 */

#ifndef TRAINER_MEMORYMANAGER_H_
#define TRAINER_MEMORYMANAGER_H_

#include "trainer.h"
#include "Module.h"

#include <ctime>
#include <set>
#include <map>

struct Page{
	Page(PageInfo&& p, int expiration);
	~Page();

	size_t read(size_t addr, void* buffer, size_t n);
	void update(Process* proc);

	bool operator < (const Page& o) const{
		return addr < o.addr;
	}

	bool operator < (size_t addr) const{
		return this->addr < addr;
	}

	char* data;
	size_t size;
	size_t addr;

	time_t lastUpdated = -1;
	int expirationTime;
};

class MemoryManager {
public:
	MemoryManager(Process* p);

	void addPage(PageInfo&& p, int expiration);//pages should not overlap
	size_t read(size_t addr, void* buffer, size_t n, bool forceUpdate = false);
	size_t write(size_t addr, void* str, size_t n);

	template <typename T, typename ...Args>
	Module& addModule(std::string name, Args... args){
		T* t = new T(this,args...);
		t->init();
		mods[name] = t;
		return *mods[name];
	}

	Module& operator[] (std::string name){
		return *mods[name];
	}

	void updateModules();

protected:
	std::set<Page> pages;
	std::map<std::string,Module*> mods;

	const Page* findPage(size_t addr);

private:
	void updateProperties(std::set<Property*> in);

	Process* parent;

	bool allExpired = false;
};

#endif /* TRAINER_MEMORYMANAGER_H_ */
