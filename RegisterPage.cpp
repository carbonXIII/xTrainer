/*
 * GameRegisters.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#include "RegisterPage.h"
#include "tools.h"
#include <exception>

using namespace std;

RegisterPage::RegisterPage(Process* process){
	PageQuery q;
	q.containedAddresses.push_back(PC_ADDRESS);
	auto pages = process->queryPages(&q);

	if(!pages.size())
		throw runtime_error("Could not find register page.");

	Memory(pages[0], process);
}

unsigned long RegisterPage::getPC() const{
	return get<uint32_t>(data + OFFSET_PC, false);
}

unsigned long RegisterPage::getGPR(int idx) const{
	return get<uint32_t>(data + OFFSET_GPR + idx * 4, false);
}

unsigned long RegisterPage::getFPR(int idx) const{
	return get<double>(data + OFFSET_FPR + idx * 16, false);
}
