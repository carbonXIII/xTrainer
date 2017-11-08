/*
 * Player.h
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "tools.h"

class GameMem;

class Player{
public:
	Player();
	Player(GameMem& mem, size_t staticPlayer);

	void locateAddreses(GameMem& mem);
	void update(GameMem& mem);

	size_t getStaticLocation() const;
	size_t getEntityLocation() const;
	size_t getDataLocation()   const;

	float getX() const;
	float getY() const;
	float getVx() const;
	float getVy() const;
	float getDmg() const;
	uint8_t getStock() const;

	void setX(GameMem& mem, float val);
	void setY(GameMem& mem, float val);
	void setVx(GameMem& mem, float val);
	void setVy(GameMem& mem, float val);
	void setDmg(GameMem& mem, float val);
	void setStock(GameMem& mem, uint8_t val);

	enum : size_t {
		OFFSET_X = 0xB0,
		OFFSET_Y = 0xB4,
		OFFSET_VX = 0x80,
		OFFSET_VY = 0x84,
		OFFSET_DMG = 0x1830,
		OFFSET_DMG_VIS = 0x60,
		OFFSET_STOCK = 0x8E
	};

private:
	size_t m_playerStatic = 0;
	size_t m_playerEntity = 0;
	size_t m_playerData = 0;

	float x = 0;
	float y = 0;
	float vx = 0;
	float vy = 0;
	float dmg = 0;
	uint8_t stock = 0;
};

#endif /* PLAYER_H_ */
