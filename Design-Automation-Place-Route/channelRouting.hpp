//
//  channelRouting.hpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.
//

#ifndef channelRouting_hpp
#define channelRouting_hpp

#include "structures.h"
#include "classifyNets.hpp"
#include "constraintList.hpp"
#include <stdio.h>
#include <vector>

void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<net> & netlistPairs, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries);

std::vector<constraintList> makeHCG(chan C, std::vector<net> & netlistPairs);

std::vector<constraintList> makeVCG(chan C);

void updateBelow(int numRows, int atRow, std::vector<cell> & cellData, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels);

void removeChild(int netNum, std::vector<constraintList>& HCG, std::vector<constraintList>& VCG);

int detectCycle(std::vector<constraintList> &VCG, std::vector<int>& cycleList);

void dogleg(int parent, int child, std::vector<net>& netlistPairs, std::vector<cell>& cellData, std::vector<constraintList>& VCG, std::vector<constraintList>& HCG, chan& channel, std::vector<std::vector<int> > &layout);

#endif /* channelRouting_hpp */
