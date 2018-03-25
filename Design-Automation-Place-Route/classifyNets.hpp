//
//  classifyNets.hpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef classifyNets_hpp
#define classifyNets_hpp

#include <stdio.h>
#include <vector>
#include "structures.h"

void findBoundaries(std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<bool> & boundaryLoc);

void makeBoundaryVec (std::vector<bool> boundaryLoc, std::vector<int> & boundaries);

void makeChannelVec (std::vector<bool> boundaryLoc, std::vector<int> & channels);

void isGlobal(std::vector<std::pair<int,int> > channels, net & netPair, std::vector<cell> cellData);

int terminalOffset(int terminal, int rotation);

void classifyNets(std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<net> & netsGlobal, std::vector<net> & netsChannel, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

#endif /* classifyNets_hpp */
