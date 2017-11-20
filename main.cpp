

/*
 * main.cpp
 *
 *  Created on: Oct 27, 2017
 *      Author: shado
 */
#define _WIN32_WINNT 0x0501

#include <conio.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <map>
#include <exception>
#include <functional>

#include "tools.h"
#include "trainer.h"

#include "RegisterPage.h"
#include "RAMPage.h"

using namespace std;

Process* process;

RAMPage ram;RegisterPage reg;

enum State { RUN, STOP } state;

map<string, function<int(istringstream&)>> commands;

void processCommand(string s){
	if(s.empty())return;

	istringstream ss(s);

	string cmd; ss >> cmd;
	auto it = commands.find(cmd);

	if(it == commands.end()){
		cout << "Unknown command." << endl;
		return;
	}

	it->second(ss);
}

void init(){
	commands["display"] = [](istringstream& ss){
		string type; ss >> type;
		if(type == "player"){
			int i;
			if(!(ss >> i)){
				cout << "USAGE: display player [player index]" << endl;
				return 0;
			}if(i < 1 || i > 4){
				cout << "Invalid player index." << endl;
				return 0;
			}
			while(true){
				ram.update();

				if(kbhit()){
					char c = getch();
					if(c == 'q')break;
				}

				cout << "\rPlayer " << i << ":" << ram.getPlayer(i - 1) << "                            ";
			}cout << endl;
		}else if(type == "register"){
			string regName; ss >> regName;

			if(regName == "PC"){
				while(true){
					reg.update();

					if(kbhit()){
						char c = getch();
						if(c == 'q')break;
					}

					cout << "\r" << std::hex << reg.getPC() << std::dec;
				}cout << endl;
			}else if(regName == "GPR" || regName == "GP"){
				int idx;
				if(!(ss >> idx)){
					cout << "USAGE: display register " << regName << " [register number]" << endl;
					return 0;
				}if(idx < 0 || idx > 31){
					cout << "Invalid register index." << endl;
					return 0;
				}
				while(true){
					reg.update();

					if(kbhit()){
						char c = getch();
						if(c == 'q')break;
					}

					cout << "\r" << std::hex << reg.getGPR(idx) << std::dec;
				}cout << endl;
			}else if(regName == "FPR" || regName == "FP"){
				int idx;
				if(!(ss >> idx)){
					cout << "USAGE: display register " << regName << " [register number]" << endl;
					return 0;
				}if(idx < 0 || idx > 31){
					cout << "Invalid register index." << endl;
					return 0;
				}
				while(true){
					reg.update();

					if(kbhit()){
						char c = getch();
						if(c == 'q')break;
					}

					cout << "\r" << reg.getFPR(idx);
				}cout << endl;
			}else{
				cout << "Unknown register." << endl;
			}
		}
		else{
			cout << "Unknown entity." << endl;
		}return 0;
	};

	commands["exit"] = [](istringstream&){
		state = State::STOP;
		return 0;
	};

	commands["set"] = [](istringstream& ss){
		string type; ss >> type;

		if(type == "player"){
			int i; ss >> i;
			if(i > 4 || i < 1){
				cout << "Invalid player number." << endl;
				return 0;
			}
			Player& p = ram.getPlayer(i - 1);

			string var; ss >> var;
			float val; ss >> val;

			if(var == "x"){
				p.setX(val);
			}else if(var == "y"){
				p.setY(val);
			}else if(var == "vx"){
				p.setVx(val);
			}else if(var == "vy"){
				p.setVy(val);
			}else if(var == "dmg" || var == "damage"){
				p.setDmg(val);
			}else if(var == "stock"){
				p.setStock(val);
			}else{
				cout << "Unknown variable." << endl;
			}
		}else{
			cout << "Unknown entity." << endl;
		}return 0;
	};

	commands["snapshot"] = [](istringstream& ss){
		string filename; ss >> filename;
		if(filename.empty()){
			cout << "USAGE: snapshot filename" << endl;
			return 0;
		}

		size_t start = 0, size = 0; ss >> start >> size;
		if(size == 0)size = ram.getSize() - start;

		char* buffer = new char[size];
		ram.snapshot(buffer, start, size);

		ofstream fout(filename);
		fout.write(buffer, ram.getSize());
		fout.close();

		return 0;
	};
}

//Register page, base address: 0x7ff622da6000

int main(){
	init();

	auto procs = enumerateProcesses("dolphin");

	int option;
	unsigned long pid;
	if(procs.size() != 0){
		cout << "Found 1 or more potential Dolphin instances.\n Select one, or select \'Not listed\' to input the PID manually." << endl;
		cout << "\tExecutable Name                                              | PID" << endl;
		int i = 0;
		for(; i < procs.size(); i++){
			cout << (i+1) << ".\t";
			if(procs[i].second.size() > 60){
				cout << "...";
				cout << procs[i].second.substr(procs[i].second.size() - 57);
			}else{
				cout << procs[i].second;
				for(int j = 0; j < 60 - procs[i].second.size(); j++)cout << " ";
			}cout << "  ";
			cout << procs[i].first << endl;
		}cout << (i+1) << ". Not listed." << endl;

		cin >> option;
	}if(procs.size() == 0 || option > procs.size()){
		cout << "Input the PID of the target process: ";
		cin >> pid;
	}else pid = procs[option - 1].first;

	process = new Process(pid);

	try{
		ram = RAMPage(process);
		reg = RegisterPage(process);
	}catch(runtime_error& e){
		cerr << e.what() << endl;
		cerr << "Make sure the game is running." << endl;
		return -1;
	}


	char* buffer = new char[100];
	while(state == State::RUN){
		cout << ">$ "; fflush(stdin);
		cin.getline(buffer, 100);
		processCommand(string(buffer));
	}

	delete [] buffer;
	delete process;

	return 0;
}
