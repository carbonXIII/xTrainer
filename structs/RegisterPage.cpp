/*
 * GameRegisters.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#include "../structs/RegisterPage.h"

#include "../tools/tools.h"
#include <exception>
#include <iostream>

using namespace std;

RegisterPage::RegisterPage(Process* proc): Memory(proc->queryFirstPage(PageQuery("",0,{PC_ADDRESS})),proc){ }

unsigned long RegisterPage::getPC() const{
	return get<uint32_t>(data + OFFSET_PC, false);
}

unsigned long RegisterPage::getGPR(int idx) const{
	return get<uint32_t>(data + OFFSET_GPR + idx * 4, false);
}

unsigned long RegisterPage::getFPR(int idx) const{
	return get<double>(data + OFFSET_FPR + idx * 16, false);
}
