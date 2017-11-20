/*
 * Memory.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */
#include "Memory.h"

Memory::Memory(){}

Memory::Memory(PageInfo info, Process* proc): n(info.size()), base_addr(info.startAddr), m_proc(proc){
	data = new char[n];
	update();
}

int Memory::getSize() const{
	return n;
}

Process* Memory::getProcess() const{
	return m_proc;
}

char& Memory::operator [] (int idx){
	return data[idx];
}

void Memory::update(){
	m_proc->readBytes(base_addr, (void*) data, n);
}

void Memory::snapshot(char* buffer, size_t start, size_t size){
	if(size == 0)size = n - start;
	memcpy(buffer, data + start, size);
}

void Memory::writeString(size_t offset, char* str, size_t n){
	m_proc->writeBytes(base_addr + offset, (void*)str, n);
}
