/*
 * GameMem.cpp
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */
#include "GameMem.h"

GameMem::GameMem(){}

GameMem::GameMem(std::pair<size_t,void*> info, HANDLE proc): n(info.first), addr(info.second), m_proc(proc){
	data = new char[n];
	update();
}

GameMem::GameMem(size_t size, void* addr, HANDLE proc): n(size), addr(addr), m_proc(proc){
	data = new char[n];
	update();
}

int GameMem::getSize() const{
	return n;
}

HANDLE GameMem::getProcess() const{
	return m_proc;
}

void GameMem::resetPage(std::pair<size_t,void*> page){
	if(n != page.first){
		delete [] data;
		n = page.first;
		data = new char[n];
	}

	addr = page.second;
	update();
}

char& GameMem::operator [] (int idx){
	return data[idx];
}

void GameMem::update(){
	size_t read;
	ReadProcessMemory(m_proc, addr, data, n, &read);
}

void GameMem::writeString(int offset, char* str, size_t n){
	size_t written;
	WriteProcessMemory(m_proc, addr + offset, str, n, &written);
}

void GameMem::writeChar(int offset, char c){
	writeString(offset, &c, sizeof(char));
}

void GameMem::writeFloat(int offset, float f){
	float buffer = getFloat((char*)&f, true);
	writeString(offset, (char*)&buffer, sizeof(float));
}

void GameMem::writeUInt16(int offset, unsigned i){
	unsigned buffer = getUInt16((char*)&i, true);
	writeString(offset, (char*)&buffer, sizeof(unsigned));
}

void GameMem::snapshot(char* buffer, size_t start, size_t size){
	if(size == 0)size = n - start;
	memcpy(buffer, data + start, size);
}
