/*
 * RAMPage.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: shado
 */

#include "RAMPage.h"
#include <exception>

extern const char* GAME_ID = "GALE01";

using namespace std;

RAMPage::RAMPage(Process* proc){
	PageQuery q;
	q.title = GAME_ID;
	q.size = RAM_SIZE;
	auto pages = proc->queryPages(&q);

	if(!pages.size())
		throw runtime_error("Could not find game memory.");

	Memory(pages[0], proc);

	players.reserve(4);
	for(int i = 0; i < 4; i++)
		players.push_back(Player(this, PLAYER_LOC[i]));
}

void RAMPage::update(){
	Memory::update();

	for(int i = 0; i < players.size(); i++)players[i].update();
}

Player& RAMPage::getPlayer(int i){
	return players[i];
}
