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
#include <stdio.h>
#include <vector>

void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries);

void updateCellsY(int numRows, int atRow, std::vector<cell> & cellData);

void addTrack(int numRows, int atRow, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout);


#endif /* channelRouting_hpp */
