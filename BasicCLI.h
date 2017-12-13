/*
 * BasicCLI.h
 *
 *  Created on: Nov 20, 2017
 *      Author: shado
 */

#ifndef BASICCLI_H_
#define BASICCLI_H_

#include "CLI.h"

#include <string>
#include <map>
#include <functional>
#include <iostream>

class BasicCLI: public CLI {
public:
	BasicCLI(Dolphin* dolphin);

protected:
	virtual void display_cmd(std::istream& is);
	virtual void set_cmd(std::istream& is);
	virtual void snapshot_cmd(std::istream& is);

	virtual void processCommand(std::string buffer);

private:
	std::map<std::string, std::function<int(std::istream&)>> commands;
};

#endif /* BASICCLI_H_ */
