/*
 * csv.cpp
 *
 *  Created on: Jan 2, 2018
 *      Author: Jordan Richards
 */
#include "csv.h"
#include "tools.h"

#include <fstream>

using namespace std;

namespace xtrainer{

SimpleCSV::SimpleCSV(const char* filename, bool hasHeader){
	read(filename, hasHeader);
}

void SimpleCSV::read(const char* filename, bool hasHeader){
	ifstream fin(filename);
	char buffer[256];

	if(hasHeader){
		fin.getline(buffer, 256);
		parseLine(buffer, true);
	}

	while(!fin.eof()){
		fin.getline(buffer, 256);
		parseLine(buffer);
	}
}

bool SimpleCSV::parseToken(std::string line, int* i, std::string& out){
	out.clear();
	bool inQuotes = false;
	while(*i < line.size()){
		if(line[*i] == '"')inQuotes = !inQuotes;
		if(!inQuotes && line[*i] == ',')break;
		out.push_back(line[*i]);
		(*i)++;
	}
	if(out.empty())return false;
	if(line[*i] == ',')(*i)++;
	return true;
}

void SimpleCSV::parseLine(std::string line, bool isHeader){
	if(!isHeader)lines.push_back(vector<string>());
	vector<string>& out = isHeader ? header : *lines.rbegin();

	int i = 0;
	string token;
	while(parseToken(line,&i,token)){
		out.push_back(token);
	}
}

size_t SimpleCSV::findHeaderIndex(std::string heading){
	for(int i = 0; i < header.size(); i++) if(header[i] == heading) return i;
	return header.size();
}

void SimpleCSV::setReadHeaders(std::string* headers, int n){
	readHeaders.clear();
	for(int i = 0; i < n; i++){
		readHeaders.push_back(findHeaderIndex(headers[i]));
	}
}

void SimpleCSV::setReadHeaders(size_t* idc, int n){
	readHeaders.clear();
	readHeaders.insert(readHeaders.begin(), idc, idc + n);
}

}//namespace xtrainer
