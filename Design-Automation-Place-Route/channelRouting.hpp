//
//  channelRouting.hpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef channelRouting_hpp
#define channelRouting_hpp

#include "structures.h"
#include "classifyNets.hpp"
#include "lin.h"
#include <stdio.h>
#include <vector>

void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<net> & netlistPairs, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries);

void updateBelow(int numRows, int atRow, std::vector<cell> & cellData, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

void addTrack(int numRows, int atRow, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

std::vector<numberList> makeHCG(chan C, std::vector<net> & netlistPairs);

std::vector<numberList> makeVCG(chan C);

bool netCompare(netPos a, netPos b);

#endif /* channelRouting_hpp */
