/*
 * Stage.cpp
 *
 *  Created on: Jan 1, 2018
 *      Author: shado
 */

#include "Stage.h"
#include "RAMPage.h"

#include <fstream>
#include <cstring>

Stage::Stage(RAMPage* parent, size_t stageStatic): m_stageStatic(stageStatic), parent(parent) {}

void Stage::update(){
	if(parent == nullptr)return;
	stageID = get<int32_t>(&(*parent)[m_stageStatic + OFFSET_STAGE_ID],true);
	musicID = get<int32_t>(&(*parent)[m_stageStatic + OFFSET_MUSIC_ID],true);
}

size_t Stage::getStaticLocation() const{
	return m_stageStatic;
}

int32_t Stage::getCurrentStageID() const{
	return stageID;
}

std::string Stage::getCurrentStageName() const{
	auto it = stageList.find(stageID);
	if(it == stageList.end())return std::string("Unknown");
	return it->second;
}

int32_t Stage::getCurrentMusicID() const{
	return musicID;
}

void Stage::addStageName(int32_t stageID, std::string name){
	stageList[stageID] = name;
}

std::ostream& operator << (std::ostream& os, const Stage& stage){
	os << "Stage ID: " << stage.getCurrentStageID() << "\t" << "Music ID: " << stage.getCurrentMusicID() << "\t" << "Stage Name: " << stage.getCurrentStageName();
	return os;
}
