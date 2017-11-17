/*
 * GameMem.h
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */

#ifndef GAMEMEM_H_
#define GAMEMEM_H_

#include <windows.h>

#include "tools.h"

class GameMem{//please rename
public:
	GameMem();
	GameMem(std::pair<size_t,void*> info, HANDLE proc);
	GameMem(size_t size, void* addr, HANDLE proc);

	int getSize() const;
	HANDLE getProcess() const;
	void resetPage(std::pair<size_t,void*> page);

	char& operator [] (int idx);

	void update();
	void snapshot(char* buffer, size_t start = 0, size_t size = 0);

	void writeString(int offset, char* str, size_t n);

	void writeChar(int offset, char c);
	void writeFloat(int offset, float f);
	void writeUInt16(int offset, unsigned i);

protected:
	char* data = nullptr;
	int n = 0;

	void* addr = nullptr;
	HANDLE m_proc = nullptr;
};

#endif /* GAMEMEM_H_ */
