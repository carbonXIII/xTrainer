/*
 * Player.cpp
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */
#include "Player.h"
#include "GameMem.h"

Player::Player() {}

Player::Player(GameMem& mem, size_t staticPlayer): m_playerStatic(staticPlayer){
	update(mem);
}

void Player::locateAddreses(GameMem& mem){
	m_playerEntity = getUInt32(&mem[m_playerStatic + 0xB0], true) - RAM_BASE_ADDR;
	m_playerData = getUInt32(&mem[m_playerEntity + 0x2C], true) - RAM_BASE_ADDR;
}

void Player::update(GameMem& mem){
	locateAddreses(mem);

	x   = getFloat(&mem[m_playerData + OFFSET_X], true);
	y   = getFloat(&mem[m_playerData + OFFSET_Y], true);
	vx  = getFloat(&mem[m_playerData + OFFSET_VX], true);
	vy  = getFloat(&mem[m_playerData + OFFSET_VY], true);
	dmg = getFloat(&mem[m_playerData + OFFSET_DMG], true);
	stock = mem[m_playerStatic + OFFSET_STOCK];
}

size_t Player::getStaticLocation() const { return m_playerStatic; }
size_t Player::getEntityLocation() const { return m_playerEntity; }
size_t Player::getDataLocation()   const { return m_playerData; }

float Player::getX() const { return x; }
float Player::getY() const { return y; }
float Player::getVx() const { return vx; }
float Player::getVy() const { return vy; }
float Player::getDmg() const { return dmg; }
uint8_t Player::getStock() const { return stock; }

void Player::setX(GameMem& mem, float val){
	mem.writeFloat(m_playerData + OFFSET_X, val);
}

void Player::setY(GameMem& mem, float val){
	mem.writeFloat(m_playerData + OFFSET_Y, val);
}

void Player::setVx(GameMem& mem, float val){
	mem.writeFloat(m_playerData + OFFSET_VX, val);
}

void Player::setVy(GameMem& mem, float val){
	mem.writeFloat(m_playerData + OFFSET_VY, val);
}

void Player::setDmg(GameMem& mem, float val){
	mem.writeFloat(m_playerData + OFFSET_DMG, val);

	unsigned n = val;
	mem.writeUInt16(m_playerStatic + OFFSET_DMG_VIS, n);//change the displayed damage as well
}

void Player::setStock(GameMem& mem, uint8_t val){
	mem.writeChar(m_playerStatic + OFFSET_STOCK, val);
}
