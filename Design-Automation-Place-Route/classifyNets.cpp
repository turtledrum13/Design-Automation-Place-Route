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


void makeChannelVec (std::vector<bool> locations, std::vector<std::pair<int,int> > & vec)
{
    std::pair<int,int> firstChannel (locations.size()-1, locations.size()-1);
    vec.push_back(firstChannel);
    
    int bottomBoundary = 0, topBoundary = 0;
    
    for (int i=locations.size()-2; i>0; i--)  //from the second boundary location to the second last boundary location
    {
        if (locations[i])
        {
            if (topBoundary == 0)
            {
                if (bottomBoundary == 0)
                {
                    bottomBoundary = i;
                }
                else
                {
                    topBoundary = i;
                }
            }
        }
        
        if (bottomBoundary!= 0 && topBoundary != 0)
        {
            std::pair<int,int> nextChannel (bottomBoundary, topBoundary);
            vec.push_back(nextChannel);
            bottomBoundary = 0; topBoundary = 0;
        }
    }
    
    std::pair<int,int> lastChannel (0, 0);
    vec.push_back(lastChannel);
}


int terminalOffset(int terminal, int rotation)
{
    int offset = 0;
    switch(rotation)
    {
        case 1 :
            switch(terminal)
            {
                case 1 : offset = -5;
                case 2 : offset = -5;
                case 3 : offset = 0;
                case 4 : offset = 0;
            }
        case 2 :
            switch(terminal)
            {
                case 1 : offset = -5;
                case 2 : offset = 0;
                case 3 : offset = -5;
                case 4 : offset = 0;
            }
        case 3 :
            switch(terminal)
            {
                case 1 : offset = 0;
                case 2 : offset = 0;
                case 3 : offset = -5;
                case 4 : offset = -5;
            }
        case 4 :
            switch(terminal)
            {
                case 1 : offset = 0;
                case 2 : offset = -5;
                case 3 : offset = 0;
                case 4 : offset = -5;
            }
        default : offset = 0;
    }
    
    return offset;
}


void isGlobal(std::vector<std::pair<int,int> > channels, std::vector<net> & netlist, std::vector<cell> cells)
{
    for (int i=0; i<netlist.size(); i++)
    {
        
        cell cellA = cells[netlist[i].c1.first];
        int termA = netlist[i].c1.second;
        cell cellB = cells[netlist[i].c2.first];
        int termB = netlist[i].c2.second;
        
        //find boundary lines each terminal falls on
        int boundaryA = cellA.y + terminalOffset(termA, cellA.r);
        int boundaryB = cellB.y + terminalOffset(termB, cellB.r);
        
        // see if both boundaries are in same channel
        if (abs(boundaryA-boundaryB) < 3)
        {
            netlist[i].global = false;
            
            for (int j=0; j<channels.size(); j++)
            {
                if (boundaryA == channels[j].first || boundaryA == channels[j].second)
                {
                    if (boundaryB == channels[j].first || boundaryB == channels[j].second)
                        netlist[i].channel = j; printf("net %i in channel %i\n", i+1, j); break;
                }
            }
            
        }
        else
        {
            netlist[i].global = true; printf("net %i is global\n", i+1);
                                             
        }
    }
}


void classifyNets(std::vector<cell> CELLS, std::vector<std::vector<int> > LAYOUT, std::vector<net> & GLOBAL, std::vector<net> & CHANNEL, std::vector<net> & NETLIST)
{
    std::vector<int> boundaryVec;
    std::vector<std::pair<int,int> > channelVec;
    std::vector<bool> boundaryLoc, channelLoc;
    
    findBoundaries(CELLS, LAYOUT, boundaryLoc);
    makeBoundaryVec(boundaryLoc, boundaryVec);
    makeChannelVec(boundaryLoc, channelVec);
    
    isGlobal(channelVec, NETLIST, CELLS);
    
    for (int i=0; i<NETLIST.size(); i++)
    {
        if (NETLIST[i].global) GLOBAL.push_back(NETLIST[i]);
        else CHANNEL.push_back(NETLIST[i]);
    }
    
  
//    printf("\n\nboundary bool:\n");
//    for (int i=0; i<boundaryLoc.size(); i++)
//    {
//        printf(boundaryLoc[i] ? "1 " : "0 ");
//    }
//
//    printf("\n\nboundary locations:\n");
//    for (int i=0; i<boundaryVec.size(); i++)
//    {
//        printf("%i ",boundaryVec[i]);
//    }
//    printf("\n\nchannel locations:\n");
//    for (int i=0; i<channelVec.size(); i++)
//    {
//        printf("(%i,%i) ",channelVec[i].first, channelVec[i].second);
//    }
}


