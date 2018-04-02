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
#include "constraintList.hpp"
#include <stdio.h>
#include <vector>

void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<net> & netlistPairs, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries);

void updateBelow(int numRows, int atRow, std::vector<cell> & cellData, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

void addTrack(int numRows, int atRow, std::vector<cell> & cellData, std::vector<net> & netlistPairs, std::vector<std::vector<int> > & layout, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

std::vector<constraintList> makeHCG(chan C, std::vector<net> & netlistPairs);

std::vector<constraintList> makeVCG(chan C);

void removeChild(int netNum, std::vector<constraintList>& HCG, std::vector<constraintList>& VCG);

int detectCycle(std::vector<constraintList> &VCG, std::vector<int>& cycleList);

void dogleg(int parent, int child, std::vector<net> & netlistPairs, std::vector<cell> & cellData, std::vector<constraintList>& VCG, std::vector<constraintList>& HCG, chan& channel, std::vector<int>& cycle_list, std::vector<std::vector<int> > &layout);

//std::vector<int> cycleList(std::vector<constraintList> & VCG, int parent);


#endif /* channelRouting_hpp */
