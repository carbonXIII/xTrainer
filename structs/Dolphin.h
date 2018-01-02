/*
 * dolphin.h
 *
 *  Created on: Dec 23, 2017
 *      Author: shado
 */

#ifndef STRUCTS_DOLPHIN_H_
#define STRUCTS_DOLPHIN_H_

#include "../structs/RAMPage.h"
#include "../structs/RegisterPage.h"

struct Dolphin{
	Dolphin(Process* p): ram(p), reg(p){}

	RAMPage ram;
	RegisterPage reg;
};

#endif /* STRUCTS_DOLPHIN_H_ */
