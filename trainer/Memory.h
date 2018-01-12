/*
 * Memory.h
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "trainer.h"
#include "../tools/log.h"

#include "Module.h"

enum class UpdateMode{
	ALL, SPARSE
};

class Memory{
public:
	Memory();
	Memory(PageInfo info, Process* proc);

	size_t getSize() const;
	Process* getProcess() const;

	void setUpdateMode(UpdateMode mode);
	UpdateMode getUpdateMode() const;

	void setBaseAddress(size_t baseAddress);
	size_t getBaseAddress() const;

	char* operator [] (size_t idx);

	void update(bool forceUpdateAll = false);

	void readString(size_t offset, char* str, size_t n);//TODO: change arg order
	void writeString(size_t offset, char* str, size_t n);

	template <typename T> void write(int offset, T t){
		T buffer = reverse(t);
		writeString(offset, (char*)&buffer, sizeof(char));
	}

	bool inRange(size_t addr) const;
	bool inRange(size_t addr, size_t n) const;

	template <typename T, typename... Args>
	Module& addModule(std::string name, Args... args){
		T* t = new T(this,args...);
		t->init();
		t->update();
		mods[name] = t;
		return *mods[name];
	}

	Module& getModule(std::string name){
		return *mods[name];
	}

	~Memory() {
		delete [] data;
		for(auto it = mods.begin(); it != mods.end(); it++)delete it->second;
	}

protected:
	char* data = nullptr;

	size_t n = 0;
	void* addr = nullptr;
	Process* m_proc = nullptr;
	size_t baseAddr = 0;

	UpdateMode updateMode = UpdateMode::ALL;

	std::map<std::string,Module*> mods;
};

#endif /* MEMORY_H_ */
