/*
 * RAMPage.cpp
 *
 *  Created on: Nov 18, 2017
 *      Author: shado
 */

#include "RAMPage.h"

#include <exception>

const char* GAME_ID = "GALE01";

using namespace std;

RAMPage::RAMPage(Process* proc): Memory(proc->queryFirstPage(PageQuery(GAME_ID, RAM_SIZE)),proc){
	for(int i = 0; i < 4; i++)
		players.push_back(Player(this, PLAYER_LOC[i]));
}

void RAMPage::update(){
	Memory::update();
	for(int i = 0; i < players.size(); i++){
		players[i].update();
	}
}

Player& RAMPage::getPlayer(int i){
	return players[i];
}
