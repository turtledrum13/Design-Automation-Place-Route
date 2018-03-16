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
    bool isEmpty = true;
    
    int size = 6;           //default cell size is 6x6
    if (C.r > 4) size = 3;  //if the rotation code is 5 or 6 then cell is a pass through cell
    
    for (int row=C.y; row>C.y-6; row--)
    {
        for (int col=C.x; col<C.x+size; col++)
        {
            if (layout[row][col] > 0) isEmpty = false;
        }
    }
    
    if (isEmpty)
    {
        for (int row=C.y; row>C.y-6; row--)
        {
            for (int col=C.x; col<C.x+size; col++)
            {
                layout[row][col] = C.nets;
            }
        }
    }
    else
    {
        printf("failed to add cell here\n");
    }
}

void addRows(int numRows, std::vector<std::vector<int> > & layout)
{
    for (int i=0; i<numRows; i++)
    {
        int size = (layout[layout.size()-1].size());
        std::vector<int> insertVec (size, 0);
        
        layout.push_back(insertVec);
    }
}

void addCols(int numCols, std::vector<std::vector<int> > & layout){
    for (int i=0; i<numCols; i++)
    {
        for (int i=0; i<layout.size(); i++)
        {
            layout[i].push_back(0);
        }
    }
}
