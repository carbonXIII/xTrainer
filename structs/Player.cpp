/*
 * Player.cpp
 *
 *  Created on: Nov 7, 2017
 *      Author: shado
 */
#include "Player.h"

#include "RAMPage.h"

Player::Player(RAMPage* parent, size_t staticPlayer): parent(parent), m_playerStatic(staticPlayer){ }

void Player::locateAddreses(){
	m_playerEntity = get<uint32_t>(&(*parent)[m_playerStatic + 0xB0], true) - RAM_BASE_ADDR;
	m_playerData = get<uint32_t>(&(*parent)[m_playerEntity + 0x2C], true) - RAM_BASE_ADDR;
}

void Player::update(){
	stock = (*parent)[m_playerStatic + OFFSET_STOCK];
	inGame = get<int32_t>(&(*parent)[m_playerStatic + OFFSET_INGAME],true) > 0;
	if(!inGame)return;

	locateAddreses();

	x   = get<float>(&(*parent)[m_playerData + OFFSET_X], true);
	y   = get<float>(&(*parent)[m_playerData + OFFSET_Y], true);
	vx  = get<float>(&(*parent)[m_playerData + OFFSET_VX], true);
	vy  = get<float>(&(*parent)[m_playerData + OFFSET_VY], true);
	dmg = get<float>(&(*parent)[m_playerData + OFFSET_DMG], true);
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
bool Player::isInGame() const { return inGame; }

void Player::setX(float val){
	parent->write(m_playerData + OFFSET_X, val);
}

void Player::setY(float val){
	parent->write(m_playerData + OFFSET_Y, val);
}

void Player::setVx(float val){
	parent->write(m_playerData + OFFSET_VX, val);
}

void Player::setVy(float val){
	parent->write(m_playerData + OFFSET_VY, val);
}

void Player::setDmg(float val){
	parent->write(m_playerData + OFFSET_DMG, val);

	uint16_t n = val;
	parent->write(m_playerStatic + OFFSET_DMG_VIS, n);//change the displayed damage as well
}

void Player::setStock(uint8_t val){
	parent->write(m_playerStatic + OFFSET_STOCK, val);
}

std::ostream& operator << (std::ostream& out, const Player& p){
	out << "\tx: "   << p.getX();
	out << "\ty: "   << p.getY();
	out << "\tvx: "  << p.getVx();
	out << "\tvy: "  << p.getVy();
	out << "\tdmg: " << p.getDmg();
	out << "\tstock: " << (int)p.getStock();
	return out;
}
