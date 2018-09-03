/*
 * Notepad Trainer:
 * Basic Sample program that can read text from a running notepad process.
 * IMPORTANT: Only tested with Windows 10 Notepad; no idea if it works with any other versions without at least minor address changes.
 *
 * author: Jordan Richards
 */
#include <iostream>

#include <xTrainer.h>

using namespace std;
using namespace xtrainer;

//Module that handles resolving pointers (to locate the text address in memory)
struct Text: public Module{
	Text(MemoryManager* parent, address_t addr): Module(parent), addr(addr) {};

	//overload the init function to call post-init on all our
	virtual void init(){
		staticTextPtr.init(this);
		dynamicTextPtr.init(this);
		cursPos.init(this);
	}

	address_t addr;//the base address of the process (referred to as [notepad.exe])
	Property staticTextPtr = Property("staticTextPtr", {addr+0x24548}, Size::_64BIT);//has a fixed address at [notepad.exe]+0x24548
	Property dynamicTextPtr = Property("dynamicTextPtr", {0, &staticTextPtr}, Size::_64BIT);//has a dynamic address at *staticTextPtr

	Property cursPos = Property("cursPos", {addr+0x23A48}, Size::_32BIT);//The position of the cursor on the current line is at [notepad.exe]+0x23A48
};

int main(int argc, char** argv){
	if(argc > 1){
		for(int i = 1; i < argc; i++){
			if(argv[i][0] == '-' && argv[i][1] == 'v'){
				default_logger.useStdErr(true);//Verbose mode
			}
		}
	}

	cout << "Notepad trainer: " << endl;
	cout << "pid: ";
	int pid; cin >> pid;

	//Hook into the notepad process
	Process* p = new Process(pid,"notepad");
	cout << "base Address:   " << p->getBaseAddress() << endl;

	//Create a new memory manager for the process
	MemoryManager mm(p);

	//Add our Text module and update the modules
	mm.addModule<Text>("text", p->getBaseAddress());
	mm.updateModules();

	cout << std::hex << "static text address: " << (size_t)mm["text"]["staticTextPtr"] << std::dec << endl;
	cout << std::hex << "dynamic text address: " << (size_t)mm["text"]["dynamicTextPtr"] << std::dec << endl;

	char buffer;
	size_t read, k;

	//Keep looping until the user inputs 'q' (only reads the first character though)
	while(true){
		char c; cin >> c;
		if(c == 'q')break;

		mm.updateModules();
		cout << "Cursor Position: " << (size_t)mm["text"]["cursPos"] << endl;
		cout << "First Line: " << endl;

		//Read characters from the notepad process until we reach \0
		k = 0;
		do{
			//reads characters with as little efficiency as possible, 1 character at a time
			read = p->readBytes((address_t)mm["text"]["dynamicTextPtr"] + k, &buffer, sizeof(char));
			if(buffer == 0 || buffer == '\n')break;
			cout.put(buffer);
			k += 2;//increment by 2 to deal with unicode
		}while(read != 0);
		cout << endl;
		cout << "Bytes read: " << k << endl;
		cout << endl;
	}

	//cleanup
	delete p;

	return 0;
}
