//
//  layoutOperations.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "layoutOperations.hpp"
#include <vector>

void makeCell(cell C, std::vector<std::vector<int> > & layout)
{
    for (int row=C.x; row>C.x-6; row--)
    {
        for (int col=C.y; col<C.y+6; col++)
        {
            layout[row][col] = C.cell;
        }
    }
    
}
