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

using namespace std;

const size_t RAM_SIZE      = 0x2000000;
const size_t RAM_BASE_ADDR = 0x80000000;
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

char reverse(char c){
	char ret = 0;
	for(int i = 0; i < 8; i++){
		ret <<= 1;
		ret |= (c & 1u);
		c >>= 1;
	}return ret;
}

float reverseFloat(char* p){
	float ret;

	char* retBytes = (char*) &ret;

	retBytes[0] = p[3];
	retBytes[1] = p[2];
	retBytes[2] = p[1];
	retBytes[3] = p[0];

	return ret;
}

unsigned reverseUInt16(char* p){
	unsigned ret = 0;
	char* retBytes = (char*)&ret;

	retBytes[0] = p[1];
	retBytes[1] = p[0];

	return ret;
}

unsigned long reverseUInt32(char* p){
	unsigned long ret = 0;
	char* retBytes = (char*)&ret;

	retBytes[0] = (p[3]);
	retBytes[1] = (p[2]);
	retBytes[2] = (p[1]);
	retBytes[3] = (p[0]);

	return ret;
}

unsigned long long reverseUInt64(char* p){
	unsigned long long ret = 0;
	char* retBytes = (char*)&ret;

	retBytes[0] = (p[7]);
	retBytes[1] = (p[6]);
	retBytes[2] = (p[5]);
	retBytes[3] = (p[4]);
	retBytes[4] = (p[3]);
	retBytes[5] = (p[2]);
	retBytes[6] = (p[1]);
	retBytes[7] = (p[0]);

	return ret;
}

struct GameMem{
	GameMem(){}

	GameMem(pair<size_t,void*> info, HANDLE proc): n(info.first), addr(info.second), m_proc(proc){
		data = new char[n];
		update();
	}

	GameMem(size_t size, void* addr, HANDLE proc): n(size), addr(addr), m_proc(proc){
		data = new char[n];
		update();
	}

	HANDLE getProcess() const{
		return m_proc;
	}

	void resetPage(pair<size_t,void*> page){
		if(n != page.first){
			delete [] data;
			n = page.first;
			data = new char[n];
		}

		addr = page.second;
		update();
	}

	char& operator [] (int idx){
		return data[idx];
	}

	void update(){
		size_t read;
		ReadProcessMemory(m_proc, addr, data, n, &read);
	}

	void writeString(int offset, char* str, size_t n){
		size_t written;
		WriteProcessMemory(m_proc, addr + offset, str, n, &written);
	}

	void writeFloat(int offset, float f){
		float buffer = reverseFloat((char*)&f);
		writeString(offset, (char*)&buffer, sizeof(float));
	}

	void writeUInt16(int offset, unsigned i){
		unsigned buffer = reverseUInt16((char*)&i);
		writeString(offset, (char*)&buffer, sizeof(unsigned));
	}

	void snapshot(char* buffer){
		memcpy(buffer, data, n);
	}

private:
	char* data;
	int n;

	void* addr;

	HANDLE m_proc;
} mem;

struct Player{
	Player() {}

	Player(GameMem& mem, size_t staticPlayer): m_playerStatic(staticPlayer){
		update(mem);
	}

	void locateMemory(GameMem& mem){
		m_playerEntity = reverseUInt32(&mem[m_playerStatic + 0xB0]) - RAM_BASE_ADDR;
		m_playerData = reverseUInt32(&mem[m_playerEntity + 0x2C]) - RAM_BASE_ADDR;
	}

	void update(GameMem& mem){
		locateMemory(mem);

		x   = reverseFloat(&mem[m_playerData + OFFSET_X]);
		y   = reverseFloat(&mem[m_playerData + OFFSET_Y]);
		vx  = reverseFloat(&mem[m_playerData + OFFSET_VX]);
		vy  = reverseFloat(&mem[m_playerData + OFFSET_VY]);
		dmg = reverseFloat(&mem[m_playerData + OFFSET_DMG]);
	}

	size_t getStaticLocation() const { return m_playerStatic; }
	size_t getEntityLocation() const { return m_playerEntity; }
	size_t getDataLocation()   const { return m_playerData; }

	float getX() const { return x; }
	float getY() const { return y; }
	float getVx() const { return vx; }
	float getVy() const { return vy; }
	float getDmg() const { return dmg; }

	void setX(GameMem& mem, float val){
		mem.writeFloat(m_playerData + OFFSET_X, val);
	}

	void setY(GameMem& mem, float val){
		mem.writeFloat(m_playerData + OFFSET_Y, val);
	}

	void setVx(GameMem& mem, float val){
		mem.writeFloat(m_playerData + OFFSET_VX, val);
	}

	void setVy(GameMem& mem, float val){
		mem.writeFloat(m_playerData + OFFSET_VY, val);
	}

	void setDmg(GameMem& mem, float val){
		mem.writeFloat(m_playerData + OFFSET_DMG, val);

		unsigned n = val;
		mem.writeUInt16(m_playerStatic + OFFSET_DMG_VIS, n);//change the displayed damage as well
	}

	enum : size_t {
		OFFSET_X = 0xB0,
		OFFSET_Y = 0xB4,
		OFFSET_VX = 0x80,
		OFFSET_VY = 0x84,
		OFFSET_DMG = 0x1830,
		OFFSET_DMG_VIS = 0x60
	};

private:
	size_t m_playerStatic = 0;
	size_t m_playerEntity = 0;
	size_t m_playerData = 0;

	float x = 0;
	float y = 0;
	float vx = 0;
	float vy = 0;
	float dmg = 0;
};

/*
 * 2000000
 * 4a119a
 * 80c7d32
 */

Player player[4];

enum State { RUN, STOP } state;

map<string, function<int(istringstream&)>> commands;

const size_t PLAYER_LOC[4] = {
	0x453080, 0x453F10, 0x454DA0, 0x455C30
};

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
