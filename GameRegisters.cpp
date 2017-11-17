/*
 * GameRegisters.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#include "tools.h"
#include "GameRegisters.h"

unsigned long GameRegisters::getPC() const{
	return getUInt32(data + OFFSET_PC, false);
}

unsigned long GameRegisters::getGPR(int idx) const{
	return getUInt32(data + OFFSET_GPR + idx * 4, false);
}

unsigned long GameRegisters::getFPR(int idx) const{
	return getDouble(data + OFFSET_FPR + idx * 16, false);
}
