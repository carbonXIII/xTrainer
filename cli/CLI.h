/*
 * CLI.h
 *
 *  Created on: Nov 20, 2017
 *      Author: shado
 */

#ifndef CLI_H_
#define CLI_H_

#include <iostream>
#include <string>

#include "../structs/RAMPage.h"
#include "../structs/RegisterPage.h"

struct Dolphin{
	Dolphin(Process* p): ram(p), reg(p){}

	RAMPage ram;
	RegisterPage reg;
};

enum State { RUN, STOP };

class CLI {
public:
	CLI(Dolphin* dolphin): m_dolphin(dolphin) {
		state = RUN;
	};

	void update(){
		char buffer[100];
		std::cout << ">$ "; fflush(stdin);
		std::cin.getline(buffer, 100);
		processCommand(std::string(buffer));
	}

	virtual ~CLI() {};

	State getState() const { return state; }
	int exit() { state = STOP; return 0; }

protected:
	virtual void processCommand(std::string command) = 0;

	Dolphin* m_dolphin;
	State state;
};

#endif /* CLI_H_ */
