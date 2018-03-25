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

void findBoundaries(std::vector<cell> cells, std::vector<std::vector<int> > layout, std::vector<bool> & boundaryLoc);

void makeBoundaryVec (std::vector<bool> locations, std::vector<int> & vec);

void makeChannelVec (std::vector<bool> locations, std::vector<int> & vec);

void isGlobal(std::vector<std::pair<int,int> > channels, std::vector<net> & netlist, std::vector<cell> cells);

int terminalOffset(int terminal, int rotation);

void classifyNets(std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<net> & netsGlobal, std::vector<net> & netsChannel, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

#endif /* classifyNets_hpp */
