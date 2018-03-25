//
//  globalRouting.hpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef globalRouting_hpp
#define globalRouting_hpp

#include "structures.h"
#include "layoutOperations.hpp"
#include <stdio.h>
#include <vector>
#include <fstream>


void printOut(std::ofstream & file, std::vector<std::vector<int> > & layout);

void global(std::vector<net> & globalNets, std::vector<net> & channelNets, std::vector<net> & netlistPairs, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<int> boundaries,  std::vector<std::pair<int,int> > & channels, std::ofstream & file);

coord terminalCoords(std::pair<int,int> cell_term, std::vector<cell> cell_data);

coord findVertical(coord src, coord dest, std::vector<std::vector<int> > layout, std::vector<int> bound, bool & topTerm);

void updateCells(std::vector<cell> &cellData, coord XY);

void updateLayout(coord XY, std::vector<std::vector<int> > &layout);

#endif /* globalRouting_hpp */
