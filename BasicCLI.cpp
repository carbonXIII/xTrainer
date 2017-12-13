/*
 * BasicCLI.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: shado
 */

#include "BasicCLI.h"
#include <conio.h>

using namespace std;

BasicCLI::BasicCLI(Dolphin* dolphin): CLI(dolphin) {
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
				dolphin->ram.update();

				if(kbhit()){
					char c = getch();
					if(c == 'q')break;
				}

				cout << "\rPlayer " << i << ":" << dolphin->ram.getPlayer(i - 1) << "                            ";
			}cout << endl;
		}else if(type == "register"){
			string regName; ss >> regName;

			if(regName == "PC"){
				while(true){
					dolphin->reg.update();

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
					dolphin->reg.update();

					if(kbhit()){
						char c = getch();
						if(c == 'q')break;
					}

					cout << "\r" << std::hex << dolphin->reg.getGPR(idx) << std::dec;
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
					dolphin->reg.update();

					if(kbhit()){
						char c = getch();
						if(c == 'q')break;
					}

					cout << "\r" << dolphin->reg.getFPR(idx);
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
			Player& p = dolphin->ram.getPlayer(i - 1);

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
		if(size == 0)size = dolphin->ram.getSize() - start;

		char* buffer = new char[size];
		dolphin->ram.snapshot(buffer, start, size);

		ofstream fout(filename);
		fout.write(buffer, dolphin->ram.getSize());
		fout.close();

		return 0;
	};
}

