//
//  classifyNets.hpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.
//

#ifndef classifyNets_hpp
#define classifyNets_hpp

#include <stdio.h>
#include <vector>
#include "structures.h"

void classifyNets(std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

void makeBoundaryVec (int width, std::vector<int> & boundaries);

void makeChannelVec (std::vector<int>& boundaries, std::vector<std::pair<int,int> > & channels);

void isGlobal(std::vector<std::pair<int,int> > channels, net & netPair, std::vector<cell> cellData);

int terminalOffset(int terminal, int rotation);

#endif /* classifyNets_hpp */
