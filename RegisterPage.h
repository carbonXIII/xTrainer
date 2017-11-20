/*
 * GameRegisters.h
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#ifndef REGISTERPAGE_H_
#define REGISTERPAGE_H_

#include "Memory.h"

const size_t PC_ADDRESS = 0x7FF622DAC520;

class RegisterPage: public Memory{
public:
	RegisterPage(): Memory() {}
	RegisterPage(Process* process);

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

#endif /* REGISTERPAGE_H_ */
