//
//  classifyNets.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "classifyNets.hpp"
#include <vector>

std::vector<bool> findBoundaries(std::vector<cell> cells, std::vector<std::vector<int> > & layout)
{
    int length = layout.size(); //parameter equal to number of rows in layout
    
    std::vector<bool> boundaryLoc (length, false);
    
    for (int i=0; i<cells.size(); i++)
    {
        int boundaryRow1 = cells[i].y;
        int boundaryRow2 = boundaryRow1+5;
        
        boundaryLoc[boundaryRow1] = true;
        boundaryLoc[boundaryRow2] = true;
    }
    
    return boundaryLoc;
}
