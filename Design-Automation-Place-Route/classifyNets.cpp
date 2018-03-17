//
//  classifyNets.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "classifyNets.hpp"
#include <vector>


void findBoundaries(std::vector<cell> cells, std::vector<std::vector<int> > layout, std::vector<bool> & boundaryLoc)
{
    int length = layout.size(); //parameter equal to number of rows in layout
    
    boundaryLoc.resize (length, false);
    
    for (int i=0; i<cells.size(); i++)
    {
        if (cells[i].r > 0)
        {
            int boundaryRow1 = cells[i].y;
            int boundaryRow2 = boundaryRow1-5;
            boundaryLoc[boundaryRow1] = true;
            boundaryLoc[boundaryRow2] = true;
        }
    }
}

void makeBoundaryVec (std::vector<bool> locations, std::vector<int> & vec)
{
    for (int i=locations.size()-1; i>=0; i--)
    {
        if (locations[i]) vec.push_back(i);
    }
}

void makeChannelVec (std::vector<bool> locations, std::vector<int> & vec)
{
    for (int i=locations.size()-1; i>=0; i--)
    {
        if (locations[i]) vec.push_back(i);
    }
}


void classifyNets(std::vector<cell> CELLS, std::vector<std::vector<int> > LAYOUT, std::vector<int> & GLOBAL, std::vector<int> & CHANNEL)
{
    std::vector<int> boundaryVec, channelVec;
    std::vector<bool> boundaryLoc, channelLoc;
    
    findBoundaries(CELLS, LAYOUT, boundaryLoc);
    
    makeBoundaryVec(boundaryLoc, boundaryVec);
    makeChannelVec(boundaryLoc, channelVec);
    
    printf("\n\nboundary bool:\n");
    for (int i=0; i<boundaryLoc.size(); i++)
    {
        printf(boundaryLoc[i] ? "1 " : "0 ");
    }
    
    printf("\n\nboundary locations:\n");
    for (int i=0; i<boundaryVec.size(); i++)
    {
        printf("%i ",boundaryVec[i]);
    }
    printf("\n\nchannel locations:\n");
    for (int i=0; i<channelVec.size(); i++)
    {
        printf("%i ",channelVec[i]);
    }
}


