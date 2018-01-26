/*
 * csv.h
 *
 *  Created on: Jan 2, 2018
 *      Author: shado
 */

#ifndef TOOLS_CSV_H_
#define TOOLS_CSV_H_

#include <vector>
#include <string>
#include <sstream>

#include "tools.h"

//Really simple convenience CSV reader (that probably shouldn't exist in this library)
struct SimpleCSV{
	SimpleCSV(const char* filename, bool hasHeader);

	void setReadHeaders(std::string* headers, int n);
	void setReadHeaders(size_t* indices, int n);

	template <typename T, typename... Args>
	void nextLine(T& a, Args&... args){
		auto it = readHeaders.begin();
		if(*it < lines[cur].size())convert(lines[cur][*it],a);
		nextLine(++it, args...);
	}

	template <typename T>
	void nextLine(T& a){
		auto it = readHeaders.begin();
		if(*it < lines[cur].size())convert(lines[cur][*it],a);
		cur++;
	}

	bool eof() const{
		return cur == lines.size();
	}

	size_t size() const { return lines.size(); }

private:
	template <typename T, typename... Args>
	void nextLine(std::vector<size_t>::iterator it, T& a, Args&... args){
		if(*it < lines[cur].size())convert(lines[cur][*it],a);
		nextLine(++it, args...);
	}

	template <typename T>
	void nextLine(std::vector<size_t>::iterator it, T& a){
		if(*it < lines[cur].size())convert(lines[cur][*it],a);
		cur++;
	}

	void read(const char* filename, bool hasHeader);
	void parseLine(std::string line, bool isHeader = false);
	bool parseToken(std::string line, int* i, std::string& out);

	size_t findHeaderIndex(std::string heading);

	std::vector<std::string> header;
	std::vector<std::vector<std::string>> lines;

	std::vector<size_t> readHeaders;
	int cur = 0;
};

#endif /* TOOLS_CSV_H_ */
