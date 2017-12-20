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
#include <sstream>
#include <iostream>

class BasicCLI: public CLI {
public:
	BasicCLI(Dolphin* dolphin);

protected:
	virtual int display_cmd(std::istringstream& is);
	virtual int set_cmd(std::istringstream& is);
	virtual int snapshot_cmd(std::istringstream& is);

	virtual void processCommand(std::string buffer);

private:
	std::map<std::string, std::function<int(std::istringstream&)>> commands;
};

#endif /* BASICCLI_H_ */
