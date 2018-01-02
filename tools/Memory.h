/*
 * Memory.h
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "../tools/trainer.h"
#include "../tools/log.h"

class Memory{
public:
	Memory();
	Memory(PageInfo info, Process* proc);

	size_t getSize() const;
	Process* getProcess() const;

	char& operator [] (int idx);

	void update();
	void snapshot(char* buffer, size_t start = 0, size_t size = 0);

	void readString(size_t offset, char* str, size_t n);
	void writeString(size_t offset, char* str, size_t n);
	template <typename T>
	void write(int offset, T t){
		T buffer = reverse(t);
		writeString(offset, (char*)&buffer, sizeof(char));
	}

	~Memory() { delete [] data; }

protected:
	char* data = nullptr;
	size_t n = 0;

	void* base_addr = nullptr;
	Process* m_proc = nullptr;
};

#endif /* MEMORY_H_ */
