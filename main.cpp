/*
 * main.cpp
 *
 *  Created on: Oct 27, 2017
 *      Author: shado
 */
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <psapi.h>
#include <conio.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <map>
#include <functional>

#include "tools.h"
#include "GameMem.h"
#include "GameRegisters.h"
#include "Player.h"

using namespace std;

const char* GAME_ID = "GALE01";

vector<pair<size_t, void*>> getPages(HANDLE proc, size_t keysize = 0, string keyword = "", vector<size_t> keyaddr = vector<size_t>()){
	vector<pair<size_t, void*>> ret;

	vector<char> buffer;

	MEMORY_BASIC_INFORMATION info;
	for(void* p = 0; VirtualQueryEx(proc, p, &info, sizeof(info)) == sizeof(info); p += info.RegionSize){
		if(keysize && info.RegionSize != keysize)continue;

		unsigned i;
		for(i = 0; i < keyaddr.size(); i++){
			if((size_t)info.BaseAddress > keyaddr[i] || (size_t)info.BaseAddress + info.RegionSize <= keyaddr[i])break;
		}if(i != keyaddr.size())continue;

		if(!keyword.empty()){
			buffer.resize(info.RegionSize);

			size_t read;
			ReadProcessMemory(proc, info.BaseAddress, buffer.data(), info.RegionSize, &read);

			unsigned i;
			for(i = 0; i < keyword.size(); i++) if(keyword[i] != buffer[i])break;
			if(i != keyword.size())continue;
		}


		ret.push_back(make_pair(info.RegionSize, info.BaseAddress));
	}

	return ret;
}

vector<pair<unsigned long, string>> findProcess(){
	vector<pair<unsigned long, string>> ret;

	vector<unsigned long> buffer(100);
	char strbuf[100];

	unsigned long size;
	do{
		buffer.resize(buffer.size() + 100);
		EnumProcesses(buffer.data(), buffer.size()*sizeof(unsigned long), &size);
	}while(size/sizeof(unsigned long) == buffer.size());
	buffer.resize(size/sizeof(unsigned long));

	for(int i = 0; i < buffer.size(); i++){
		HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, buffer[i]);

		unsigned long strSize;
		if(h && GetProcessImageFileNameA(h, strbuf, 100) && contains(strbuf, "dolphin")){
			ret.push_back(make_pair(buffer[i], string(strbuf)));
		}
	}

	return ret;
}

GameMem mem;
GameRegisters reg;
Player player[4];

enum State { RUN, STOP } state;

map<string, function<int(istringstream&)>> commands;

const size_t PLAYER_LOC[4] = { 0x453080, 0x453F10, 0x454DA0, 0x455C30 };

void updatePlayers(){
	for(int i = 0; i < 4; i++){
		if(getUInt32(&mem[PLAYER_LOC[i]], true) != 0){
			cout << "Player " << (i + 1) << " in game." << endl;
			player[i] = Player(mem, PLAYER_LOC[i]);
		}
	}
}

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
			int i; ss >> i;
			while(true){
				mem.update();
				player[i - 1].update(mem);

				if(kbhit()){
					char c = getch();
					if(c == 'q')break;
				}

				cout << "\rPlayer " << i << ":";
				cout << "  x:\t"   << player[i - 1].getX();
				cout << "  y:\t"   << player[i - 1].getY();
				cout << "  vx:\t"  << player[i - 1].getVx();
				cout << "  vy:\t"  << player[i - 1].getVy();
				cout << "  dmg:\t" << player[i - 1].getDmg();
				cout << "  stock:\t" << (int)player[i - 1].getStock();
				cout << "                            ";
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

	commands["updatePlayers"] = [](istringstream&){
		updatePlayers();
		return 0;
	};

	commands["syncPage"] = [](istringstream&){
		auto pages = getPages(mem.getProcess(), RAM_SIZE, GAME_ID);
		if(pages.size() == 0){
			cout << "Could not find game memory. Is the game running?" << endl;
			system("PAUSE");
			return -1;
		}

		mem.resetPage(pages[0]);

		return 0;
	};

	commands["set"] = [](istringstream& ss){
		string type; ss >> type;

		if(type == "player"){
			int i; ss >> i;
			if(i > 4 || i < 0){
				cout << "Invalid player number." << endl;
				return 0;
			}
			Player& p = player[i - 1];

			string var; ss >> var;
			float val; ss >> val;

			if(var == "x"){
				p.setX(mem, val);
			}else if(var == "y"){
				p.setY(mem, val);
			}else if(var == "vx"){
				p.setVx(mem, val);
			}else if(var == "vy"){
				p.setVy(mem, val);
			}else if(var == "dmg" || var == "damage"){
				p.setDmg(mem, val);
			}else if(var == "stock"){
				p.setStock(mem, val);
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
		if(size == 0)size = mem.getSize() - start;

		char* buffer = new char[size];
		mem.snapshot(buffer, start, size);

		ofstream fout(filename);
		fout.write(buffer, mem.getSize());
		fout.close();

		return 0;
	};
}

//Register page, base address: 0x7ff622da6000

int main(){
	init();

	vector<pair<unsigned long, string>> procs = findProcess();

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

	HANDLE target = OpenProcess(PROCESS_ALL_ACCESS, true, pid);

	auto pages = getPages(target, RAM_SIZE, GAME_ID);
	if(pages.size() == 0){
		cout << "Could not find game memory. Is the game running?" << endl;
		system("PAUSE");
		return -1;
	}

	mem = GameMem(pages[0], target);

	vector<size_t> keyaddr(1, 0x7FF622DAC520);//PC address
	auto regPages = getPages(target, 0, "", keyaddr);

	cout << regPages[0].second << endl;

	reg = GameRegisters(regPages[0], target);

	updatePlayers();

	char* buffer = new char[100];
	while(state == State::RUN){
		cout << ">$ "; fflush(stdin);
		cin.getline(buffer, 100);
		processCommand(string(buffer));
	}

	delete [] buffer;

	return 0;
}
