//
//  channelRouting.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "channelRouting.hpp"
#include "layoutOperations.hpp"
#include <stdio.h>

void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries)
{
    //For each channel
    for(size_t i=0; i<channels.size(); i++)
    {
        //Form a vector for each boundary in the channel
        //addTrack(10, 48, cellData, layout);

    }

    printf("\n\nDid channel routing\n\n");

}

void updateCellsY(int numRows, int atRow, std::vector<cell> & cellData)
{
    for (size_t i=0; i<cellData.size(); i++)
    {
        if (cellData[i].y >= atRow)
        {
            cellData[i].y += numRows;
        }
    }
}

void addTrack(int numRows, int atRow, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout)
{
    addRows(numRows, atRow, layout);
    updateCellsY(numRows, atRow, cellData);
}
