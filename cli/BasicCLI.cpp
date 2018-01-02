/*
 * BasicCLI.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: shado
 */

#include "BasicCLI.h"
#include <conio.h>
#include <fstream>
#include <sstream>

using namespace std;

BasicCLI::BasicCLI(Dolphin* dolphin): CLI(dolphin) {
	commands["display"]  = [this](istringstream& ss){ return display_cmd(ss); };
	commands["set"]      = [this](istringstream& ss){ return set_cmd(ss); };
	commands["snapshot"] = [this](istringstream& ss){ return snapshot_cmd(ss); };
	commands["exit"]     = [this](istringstream& ss){ return exit(); };
}
const char* digits = "0123456789ABCDEF";

void printHex(const char* str, int n){
	for(int i = 0; i < n; i++){
		cout << digits[str[i] & 0xF] << digits[str[i] & 0xF0];
		cout << " ";
	}
}

int BasicCLI::display_cmd(std::istringstream& ss){
	default_logger.useStdErr(false);
	string type; ss >> type;
	if(type == "player"){
		int i;
		if(!(ss >> i)){
			cout << "USAGE: display player [player index]" << endl;
			goto end;
		}if(i < 1 || i > 4){
			cout << "Invalid player index." << endl;
			goto end;
		}
		while(true){
			m_dolphin->ram.update();

			if(kbhit()){
				char c = getch();
				if(c == 'q')break;
			}

			cout << "Player " << i << ":" << m_dolphin->ram.getPlayer(i - 1) << "                            \r";//actually a terrible way to rewrite lines
		}
		cout << endl;
	}else if(type == "register"){
		string regName; ss >> regName;

		if(regName == "PC"){
			while(true){
				m_dolphin->reg.update();

				if(kbhit()){
					char c = getch();
					if(c == 'q')break;
				}

				cout << std::hex << m_dolphin->reg.getPC() << std::dec << '\r';
			}cout << endl;
		}else if(regName == "GPR" || regName == "GP"){
			int idx;
			if(!(ss >> idx)){
				cout << "USAGE: display register " << regName << " [register number]" << endl;
				goto end;
			}if(idx < 0 || idx > 31){
				cout << "Invalid register index." << endl;
				goto end;
			}
			while(true){
				m_dolphin->reg.update();

				if(kbhit()){
					char c = getch();
					if(c == 'q')break;
				}

				cout << std::hex << m_dolphin->reg.getGPR(idx) << std::dec << '\r';
			}cout << endl;
		}else if(regName == "FPR" || regName == "FP"){
			int idx;
			if(!(ss >> idx)){
				cout << "USAGE: display register " << regName << " [register number]" << endl;
				goto end;
			}if(idx < 0 || idx > 31){
				cout << "Invalid register index." << endl;
				goto end;
			}
			while(true){
				m_dolphin->reg.update();

				if(kbhit()){
					char c = getch();
					if(c == 'q')break;
				}

				cout << m_dolphin->reg.getFPR(idx) << '\r';
			}cout << endl;
		}else{
			cout << "Unknown register." << endl;
		}
	}else if(type == "string"){
		size_t addr, n;
		if(!(ss >> addr) || !(ss >> n)){
			cout << "USAGE: display string [ADDRESS] [LENGTH]" << endl;
			goto end;
		}

		bool hex;
		string s; ss >> s;
		if(s == "HEX")

		if(addr < RAM_BASE_ADDR || addr + n >= RAM_BASE_ADDR + m_dolphin->ram.getSize()){
			cout << "Invalid address range." << endl;
			goto end;
		}

		char str[n];
		while(true){
			m_dolphin->ram.update();

			if(kbhit()){
				char c = getch();
				if(c == 'q')break;
			}

			m_dolphin->ram.readString(addr - RAM_BASE_ADDR,str,n);
			for(int i = 0; i < n; i++)cout << str[i];
			cout << "\r";
		}
	}else if(type == "hexstring"){
		size_t addr, n;
		if(!(ss >> addr) || !(ss >> n)){
			cout << "USAGE: display hexstring [ADDRESS] [LENGTH]" << endl;
			goto end;
		}

		bool hex;
		string s; ss >> s;
		if(s == "HEX")

		if(addr < RAM_BASE_ADDR || addr + n >= RAM_BASE_ADDR + m_dolphin->ram.getSize()){
			cout << "Invalid address range." << endl;
			goto end;
		}

		char str[n];
		while(true){
			m_dolphin->ram.update();

			if(kbhit()){
				char c = getch();
				if(c == 'q')break;
			}

			m_dolphin->ram.readString(addr - RAM_BASE_ADDR,str,n);
			printHex(str,n);
			cout << "\r";
		}
	}else if(type == "int"){
		size_t addr;
		if(!(ss >> addr)){
			cout << "USAGE: display int [ADDRESS]" << endl;
			goto end;
		}

		if(addr < RAM_BASE_ADDR || addr + sizeof(int16_t) >= RAM_BASE_ADDR + m_dolphin->ram.getSize()){
			cout << "Invalid address range." << endl;
			goto end;
		}

		while(true){
			m_dolphin->ram.update();

			if(kbhit()){
				char c = getch();
				if(c == 'q')break;
			}

			cout << get<int16_t>(&m_dolphin->ram[addr - RAM_BASE_ADDR],true) << "                 \r";
		}
	}else if(type == "float"){
		size_t addr;
		if(!(ss >> addr)){
			cout << "USAGE: display int [ADDRESS]" << endl;
			goto end;
		}

		if(addr < RAM_BASE_ADDR || addr + sizeof(float) >= RAM_BASE_ADDR + m_dolphin->ram.getSize()){
			cout << "Invalid address range." << endl;
			goto end;
		}

		while(true){
			m_dolphin->ram.update();

			if(kbhit()){
				char c = getch();
				if(c == 'q')break;
			}

			cout << get<float>(&m_dolphin->ram[addr - RAM_BASE_ADDR],true) << "                 \r";
		}
	}
	else{
		cout << "Unknown entity." << endl;
	}
end:
	default_logger.useStdErr(true);
	return 0;
}

int BasicCLI::set_cmd(std::istringstream& ss){
	string type; ss >> type;

	if(type == "player"){
		int i; ss >> i;
		if(i > 4 || i < 1){
			cout << "Invalid player number." << endl;
			return 0;
		}
		Player& p = m_dolphin->ram.getPlayer(i - 1);

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
	}

	return 0;
}

int BasicCLI::snapshot_cmd(std::istringstream& ss){
	string filename; ss >> filename;
	if(filename.empty()){
		cout << "USAGE: snapshot filename" << endl;
		return 0;
	}

	size_t start = 0, size = 0; ss >> start >> size;
	if(size == 0)size = m_dolphin->ram.getSize() - start;

	char* buffer = new char[size];
	m_dolphin->ram.snapshot(buffer, start, size);

	ofstream fout(filename);
	fout.write(buffer, m_dolphin->ram.getSize());
	fout.close();

	return 0;
}

void BasicCLI::processCommand(string s){
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
