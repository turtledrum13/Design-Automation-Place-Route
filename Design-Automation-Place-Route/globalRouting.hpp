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
#include <stdio.h>
#include <vector>

coord terminalCoords(std::pair<int,int> cell_term, std::vector<cell> cell_data);

void lees(std::vector<net> & globalNets, std::vector<cell> cellData);

void global();



#endif /* globalRouting_hpp */
