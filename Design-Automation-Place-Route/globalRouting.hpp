//
//  globalRouting.hpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.

//

#ifndef globalRouting_hpp
#define globalRouting_hpp

#include "structures.h"
#include "layoutOperations.hpp"
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>


void global(std::vector<net> & netlistPairs, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<int> boundaries,  std::vector<std::pair<int,int> > & channels);

coord terminalCoords(std::pair<int,int> cell_term, std::vector<cell> cell_data);

coord findVertical(coord src, coord dest, std::vector<std::vector<int> > layout, std::vector<int> bound, bool & topTerm);

void updateRight(std::vector<cell> &cellData, coord XY);

void updateLayout(coord XY, std::vector<std::vector<int> > &layout, std::string updateSetting);

#endif /* globalRouting_hpp */
