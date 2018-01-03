/*
 * Stage.h
 *
 *  Created on: Jan 1, 2018
 *      Author: shado
 */

#ifndef STRUCTS_STAGE_H_
#define STRUCTS_STAGE_H_

#include "../tools/tools.h"

#include <iostream>
#include <map>

class RAMPage;

class Stage {
friend class RAMPage;
public:
	void addStageName(int32_t stageID, std::string name);

	size_t getStaticLocation() const;

	int32_t getCurrentStageID() const;
	std::string getCurrentStageName() const;
	int32_t getCurrentMusicID() const;

	enum : size_t {
		OFFSET_STAGE_ID = 0x88,
		OFFSET_MUSIC_ID = 0x98
	};

protected:
	Stage() {}
	Stage(RAMPage* parent, size_t stageStatic);

	void update();
private:
	RAMPage* parent = nullptr;
	size_t m_stageStatic = 0;

	std::map<int32_t,std::string> stageList;

	int32_t stageID = -1;
	int32_t musicID = -1;
};

std::ostream& operator << (std::ostream& os, const Stage& stage);

#endif /* STRUCTS_STAGE_H_ */
