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
#include <vector>
#include <cstring>
#include <sstream>
#include <map>
#include <functional>

#include "tools.h"
#include "GameMem.h"
#include "Player.h"

using namespace std;

const char* GAME_ID = "GALE01";

vector<pair<size_t, void*>> getPages(HANDLE proc, string keyword, size_t keysize = 0){
	vector<pair<size_t, void*>> ret;

	vector<char> buffer;

	MEMORY_BASIC_INFORMATION info;
	for(void* p = 0; VirtualQueryEx(proc, p, &info, sizeof(info)) == sizeof(info); p += info.RegionSize){
		if(keysize && info.RegionSize != keysize)continue;

		buffer.resize(info.RegionSize);

		size_t read;
		ReadProcessMemory(proc, info.BaseAddress, buffer.data(), info.RegionSize, &read);

		unsigned i;
		for(i = 0; i < keyword.size(); i++) if(keyword[i] != buffer[i])break;
		if(i == keyword.size())
			ret.push_back(make_pair(info.RegionSize, info.BaseAddress));
	}

	return ret;
}

GameMem mem;
Player player[4];

enum State { RUN, STOP } state;

map<string, function<int(istringstream&)>> commands;

const size_t PLAYER_LOC[4] = { 0x453080, 0x453F10, 0x454DA0, 0x455C30 };

void updatePlayers(){
	for(int i = 0; i < 4; i++){
		if(reverseUInt32(&mem[PLAYER_LOC[i]]) != 0){
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
			while(true){
				int i; ss >> i;
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
		}else{
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
		auto pages = getPages(mem.getProcess(), GAME_ID, RAM_SIZE);
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
}

int main(){
	init();

	int pid;
	cout << "Input target process's PID: ";
	cin >> pid;

	HANDLE target = OpenProcess(PROCESS_ALL_ACCESS, true, pid);

	auto pages = getPages(target, GAME_ID, RAM_SIZE);
	if(pages.size() == 0){
		cout << "Could not find game memory. Is the game running?" << endl;
		system("PAUSE");
		return -1;
	}

	mem = GameMem(pages[0], target);

	cout << string(&mem[0],&mem[6]) << endl;

	updatePlayers();

	cout << "Player 1 entity address: " << std::hex << player[0].getEntityLocation() << std::dec << endl;
	cout << "Player 1 data address:   " << std::hex << player[0].getDataLocation() << std::dec << endl;

	char* buffer = new char[100];
	while(state == State::RUN){
		cout << ">$ "; fflush(stdin);
		cin.getline(buffer, 100);
		processCommand(string(buffer));
	}

	system("pause");

	delete [] buffer;

	return 0;
}
