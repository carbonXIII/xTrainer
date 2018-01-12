/*
 * Memory.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */
#include "Memory.h"

Memory::Memory(){}

Memory::Memory(PageInfo info, Process* proc): n(info.size()), addr(info.startAddr), m_proc(proc){
	data = new char[n];
	update();
}

size_t Memory::getSize() const{
	return n;
}

Process* Memory::getProcess() const{
	return m_proc;
}

void Memory::setUpdateMode(UpdateMode mode){
	updateMode = mode;
}

UpdateMode Memory::getUpdateMode() const{
	return updateMode;
}

void Memory::setBaseAddress(size_t baseAddress){
	this->baseAddr = baseAddress;
}

size_t Memory::getBaseAddress() const{
	return baseAddr;
}

char* Memory::operator [] (size_t idx){
	if(!inRange(idx - baseAddr))return 0;
	return data + idx - baseAddr;
}

void Memory::update(bool forceAll){
	UpdateMode mode = forceAll ? UpdateMode::ALL : updateMode;

	switch(mode){
	case UpdateMode::SPARSE:
		for(auto it = mods.begin(); it != mods.end(); it++){
			for(auto b = it->second->props.begin(); b != it->second->props.end(); it++){
				m_proc->readBytes(addr + b->second->getAddr() - baseAddr,
					(void*) data + b->second->getAddr() - baseAddr, b->second->getSize());
			}
		}

		break;

	case UpdateMode::ALL:
	default:
		m_proc->readBytes(addr, (void*) data, n);
	}

	for(auto it = mods.begin(); it != mods.end(); it++){
		it->second->update();
	}
}

void Memory::readString(size_t start, char* buffer, size_t size){
	if(size == 0)size = n - start;
	memcpy(buffer, data + start - baseAddr, size);
}

void Memory::writeString(size_t offset, char* str, size_t n){
	m_proc->writeBytes(addr + offset - baseAddr, (void*)str, n);
}

bool Memory::inRange(size_t addr) const{
	return addr < n;
}
