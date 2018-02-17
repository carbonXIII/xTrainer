/*
 * MemoryManager.cpp
 *
 *  Created on: Jan 24, 2018
 *  Author: Jordan Richards
 */

#include "MemoryManager.h"
#include <algorithm>
#include <vector>
#include <exception>
#include <functional>

namespace xtrainer{

Page::Page(PageInfo&& p, int expiration): expirationTime(expiration){
	size = p.size();
	addr = (size_t)p.startAddr;
	data = new char[size];
}

Page::~Page(){
	delete [] data;
}

size_t Page::read(size_t addr, void* buffer, size_t n){
	char* str = (char*)buffer;
	size_t k = 0;
	for(int i = addr - this->addr; i < addr - this->addr + n && i < size; i++){
		str[i] = data[i];
		k++;
	}return k;
}

void Page::update(Process* proc){
	proc->readBytes((void*)addr, data, size);
	lastUpdated = time(0);
}

MemoryManager::MemoryManager(Process* p): parent(p) {}

void MemoryManager::addPage(PageInfo&& pageInfo, int expiration){
	pages.insert(Page(std::move(pageInfo),expiration));
}

const Page* MemoryManager::findPage(size_t addr){
	if(!pages.size())return nullptr;
	auto page = std::lower_bound(pages.begin(), pages.end(), addr);
	if(page == pages.end() || (page != pages.begin() && page->getStartAddress() > addr))page--;


	if(page->getStartAddress() + page->getSize() < addr)return &*page;
	return nullptr;
}

size_t MemoryManager::read(size_t addr, void* buffer, size_t n, bool forceUpdate){
	Page* p = const_cast<Page*>(findPage(addr));

	if(p == nullptr){
		return parent->readBytes((void*)addr, buffer, n);
	}

	if(allExpired || forceUpdate || p->isExpired())
		p->update(parent);

	allExpired = false;
	return p->read(addr, buffer, n);
}

size_t MemoryManager::write(size_t addr, void* str, size_t n){
	allExpired = true;
	return parent->writeBytes((void*)addr, str, n);
}

void dfs(int i, int* visited, std::vector<Property*> in, std::function<void(Property*)> callback){
	if(visited[i] == 2)return;
	if(visited[i] == 1)throw std::runtime_error("Circular dependency while processing property.");
	visited[i] = 1;
	if(in[i]->dependency() != nullptr){
		auto it = std::find(in.begin(), in.end(), in[i]->dependency());
		if(it == in.end())throw std::runtime_error("Required property not found.");
		dfs(it - in.begin(), visited, in, callback);
	}
	visited[i] = 2;
	callback(in[i]);
}

void MemoryManager::updateProperties(std::set<Property*> in){
	std::vector<Property*> v(in.begin(), in.end());

	int* visited = new int[v.size()];
	for(int i = 0; i < v.size(); i++){
		dfs(i, visited, v, [](Property* p){
			p->update();
		});
	}
	delete [] visited;
}

void MemoryManager::updateModules(){
	std::set<Property*> props;
	for(auto it = mods.begin(); it != mods.end(); it++){
		it->second->getProperties(std::inserter(props,props.begin()));
	}

	updateProperties(props);
}

}//namespace xtrainer
