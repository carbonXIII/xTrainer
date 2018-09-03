/*
 * MemoryManager.h
 *
 *  Created on: Jan 24, 2018
 *      Author: Jordan Richards
 */

#ifndef TRAINER_MEMORYMANAGER_H_
#define TRAINER_MEMORYMANAGER_H_

#include "trainer.h"
#include "Module.h"

#include <ctime>
#include <set>
#include <map>

namespace xtrainer{

//Cache for foreign process memory
struct Page{
	Page(PageInfo&& p, int expiration);
	~Page();

	//read and update the contents of the page
	size_t read(address_t addr, void* buffer, size_t n);
	void update(Process* proc);

	size_t getSize() const { return size; }
	address_t getStartAddress() const { return addr; }

	//sorting/searching functions
	bool operator < (const Page& o) const{
		return addr < o.addr;
	}

	bool operator < (address_t addr) const{
		return this->addr < addr;
	}

	bool isExpired() const{
		return lastUpdated + expirationTime < time(0);
	}

private:
	char* data;
	size_t size;
	address_t addr;

	time_t lastUpdated = -1;
	int expirationTime;
};

//Manages memory for a foreign process
class MemoryManager {
public:
	MemoryManager(Process* p);

	void addPage(PageInfo&& p, int expiration);//pages should not overlap

	//read and write from foreign process memory
	size_t read(address_t addr, void* buffer, size_t n, bool forceUpdate = false);//reads from cache if available (updating if necessary)
	size_t write(address_t addr, void* str, size_t n);

	//Add a new module to the MemoryManager
	template <typename T, typename ...Args>
	Module& addModule(std::string name, Args... args){
		T* t = new T(this,args...);
		t->init();
		mods[name] = t;
		return *mods[name];
	}


	//retrieves module
	Module& operator[] (std::string name){
		return *mods[name];
	}

	//updates all the modules (not pages)
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

}//namespace xtrainer

#endif /* TRAINER_MEMORYMANAGER_H_ */
