/*
 * Memory.h
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "trainer.h"

class Memory{
public:
	Memory();
	Memory(PageInfo info, Process* proc);

	int getSize() const;
	Process* getProcess() const;

	char& operator [] (int idx);

	void update();
	void snapshot(char* buffer, size_t start = 0, size_t size = 0);

	void writeString(size_t offset, char* str, size_t n);
	template <typename T>
	void write(int offset, T t){
		T buffer = reverse(t);
		writeString(offset, (char*)&buffer, sizeof(char));
	}

protected:
	char* data = nullptr;
	int n = 0;

	void* base_addr = nullptr;
	Process* m_proc = nullptr;
};

#endif /* MEMORY_H_ */
