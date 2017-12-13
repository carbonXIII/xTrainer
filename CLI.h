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

#include "RAMPage.h"
#include "RegisterPage.h"

struct Dolphin{
	Dolphin(Process* p);

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

protected:
	virtual void processCommand(std::string command) = 0;

	Dolphin* m_dolphin;
	enum State { RUN, STOP } state;
};

#endif /* CLI_H_ */
