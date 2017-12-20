/*
 * RAMPage.h
 *
 *  Created on: Nov 18, 2017
 *      Author: shado
 */

#ifndef RAMPAGE_H_
#define RAMPAGE_H_

#include <vector>
#include "../tools/Memory.h"
#include "Player.h"

extern const char* GAME_ID;
const size_t RAM_SIZE      = 0x2000000;
const size_t RAM_BASE_ADDR = 0x80000000;
const size_t PLAYER_LOC[4] = { 0x453080, 0x453F10, 0x454DA0, 0x455C30 };

class RAMPage: public Memory {
public:
	RAMPage(): Memory() {};
	RAMPage(Process* proc);

	void update();
	Player& getPlayer(int idx);

private:
	std::vector<Player> players;
	PageQuery _q;
};

#endif /* RAMPAGE_H_ */
