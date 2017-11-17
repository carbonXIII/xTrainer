/*
 * GameRegisters.h
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#ifndef GAMEREGISTERS_H_
#define GAMEREGISTERS_H_

#include <windows.h>
#include "GameMem.h"

class GameRegisters : public GameMem{
public:
	GameRegisters() : GameMem() {};
	GameRegisters(std::pair<size_t,void*> info, HANDLE proc) : GameMem(info,proc) {};
	GameRegisters(size_t size, void* addr, HANDLE proc) : GameMem(size,addr,proc) {};

	enum : size_t {
		OFFSET_PC = 0x6520,
		OFFSET_GPR = 0x64A0,
		OFFSET_FPR = 0x6580,
		OFFSET_FPR_copy = 0x6588
	};

	unsigned long getPC() const;
	unsigned long getGPR(int idx) const;
	unsigned long getFPR(int idx) const;
};

#endif /* GAMEREGISTERS_H_ */
