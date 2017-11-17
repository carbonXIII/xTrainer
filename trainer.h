/*
 * trainer.h
 *
 *  Created on: Nov 17, 2017
 *      Author: shado
 */

#ifndef TRAINER_H_
#define TRAINER_H_

#include <vector>
#include <string>
#include <windows.h>

extern std::vector<std::pair<size_t, void*>> getPages(HANDLE proc, size_t keysize = 0, std::string keyword = "", std::vector<size_t> keyaddr = std::vector<size_t>());

extern std::vector<std::pair<unsigned long, std::string>> findProcess();

#endif /* TRAINER_H_ */
