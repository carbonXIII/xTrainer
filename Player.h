/*
 * Player.h
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "tools.h"
#include <iostream>

class RAMPage;

class Player{
friend class RAMPage;
public:
	size_t getStaticLocation() const;
	size_t getEntityLocation() const;
	size_t getDataLocation()   const;

	float getX() const;
	float getY() const;
	float getVx() const;
	float getVy() const;
	float getDmg() const;
	uint8_t getStock() const;
	bool isInGame() const;

	void setX(float val);
	void setY(float val);
	void setVx(float val);
	void setVy(float val);
	void setDmg(float val);
	void setStock(uint8_t val);

	enum : size_t {
		OFFSET_X = 0xB0,
		OFFSET_Y = 0xB4,
		OFFSET_VX = 0x80,
		OFFSET_VY = 0x84,
		OFFSET_DMG = 0x1830,
		OFFSET_DMG_VIS = 0x60,
		OFFSET_STOCK = 0x8E,
		OFFSET_INGAME = 0
	};

protected:
	Player(RAMPage* mem, size_t staticPlayer);

	void update();

private:
	void locateAddreses();

	size_t m_playerStatic = 0;
	size_t m_playerEntity = 0;
	size_t m_playerData = 0;

	float x = 0;
	float y = 0;
	float vx = 0;
	float vy = 0;
	float dmg = 0;
	uint8_t stock = 0;
	bool inGame = false;

	RAMPage* parent;
};

extern std::ostream& operator << (std::ostream&, const Player& p);

#endif /* PLAYER_H_ */
