/*
 * Notepad Trainer:
 * Basic Sample program that can read text from a running notepad process.
 *
 * author: Jordan Richards
 */
#include <iostream>

#include <xTrainer.h>
#include <cwchar>

using namespace std;

struct Text: public Module{
	Text(MemoryManager* parent, size_t addr): Module(parent), addr(addr) {};

	virtual void init(){
		pointerA.init(this);
		pointerB.init(this);
		cursPos.init(this);
		pointerC.init(this);
		len.init(this);
	}

	size_t addr;
	Property pointerA = Property("pointerA", {addr+0x24548}, Size::_64BIT);
	Property pointerB = Property("pointerB", {0, &pointerA}, Size::_64BIT);

	Property cursPos = Property("cursPos", {addr+0x23A48}, Size::_32BIT);

	Property pointerC = Property("pointerC", {addr+0x23440}, Size::_64BIT);
	Property len = Property("len", {0, &pointerC}, Size::_32BIT);
};

int main(){
	default_logger.useStdErr(true);
	cout << "pid: ";
	int pid; cin >> pid;

	Process* p = new Process(pid,"notepad");
	cout << "base Address:   " << p->getBaseAddress() << endl;

	MemoryManager mm(p);
	mm.addModule<Text>("text", (size_t)p->getBaseAddress());
	mm.updateModules();

	cout << std::hex << "intermediate pointer: " << (size_t)mm["text"]["pointerA"] << std::dec << endl;
	cout << std::hex << "text address: " << (size_t)mm["text"]["pointerB"] << std::dec << endl;
	cout << std::hex << "length address: " << (size_t)mm["text"]["pointerC"] << std::dec << endl;

	char buffer;
	size_t read, k;
	while(true){
		char c; cin >> c;
		if(c == 'q')break;

		mm.updateModules();
		cout << "Cursor Position: " << (size_t)mm["text"]["cursPos"] << endl;
		cout << "Length: " << (unsigned long)mm["text"]["len"] << endl;
		cout << "First Line: " << endl;
		k = 0;
		do{
			read = p->readBytes((void*)((size_t)mm["text"]["pointerB"] + k), &buffer, sizeof(char));
			if(buffer == 0 || buffer == '\n')break;
			cout.put(buffer);
			k += 2;
		}while(read != 0);
		cout << endl;
		cout << "Bytes read: " << k << endl;
		cout << endl;
	}
	wcout.put(wchar_t('\n'));

	delete p;

	return 0;
}

//7ff72ade4548
//24548
