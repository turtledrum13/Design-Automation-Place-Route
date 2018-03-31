//
//  classifyNets.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "classifyNets.hpp"
#include <vector>
#include <stdlib.h>


void classifyNets(std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<net> & netsGlobal, std::vector<net> & netsChannel, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
{
    std::vector<bool> boundaryLoc;

    findBoundaries(cellData, layout, boundaryLoc);
    makeBoundaryVec(boundaryLoc, boundaries);
    makeChannelVec(boundaries, channels);

    for (int i=0; i<netlistPairs.size(); i++)
    {
        isGlobal(channels, netlistPairs[i], cellData);
    }

    for (int i=0; i<netlistPairs.size(); i++)
    {
        if (netlistPairs[i].global) netsGlobal.push_back(netlistPairs[i]);
        else netsChannel.push_back(netlistPairs[i]);
    }

    //    printf("\n\nboundary bool:\n");
    //    for (int i=0; i<boundaryLoc.size(); i++)
    //    {
    //        printf(boundaryLoc[i] ? "1 " : "0 ");
    //    }
    //
        printf("\n\nboundary locations:\n");
        for (int i=0; i<boundaries.size(); i++)
        {
            printf("%i ",boundaries[i]);
        }
        printf("\n\nchannel locations:\n");
        for (int i=0; i<channels.size(); i++)
        {
            printf("(%i,%i) ",channels[i].first, channels[i].second);
        }
        printf("\n\n");
}


void findBoundaries(std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<bool> & boundaryLoc)
{
    size_t length = layout.size(); //parameter equal to number of rows in layout

    boundaryLoc.resize (length, false);

    for (int i=0; i<cellData.size(); i++)           //for all the cells
    {
        if (cellData[i].r > 0)                      //if the cell is not a dummy
        {
            int boundaryRow1 = cellData[i].y;       //its lower edge is a boundary
            boundaryLoc[boundaryRow1] = true;

            int boundaryRow2 = boundaryRow1-5;      //its upper edge is a boundary
            boundaryLoc[boundaryRow2] = true;
        }
    }
}


void makeBoundaryVec (std::vector<bool> boundaryLoc, std::vector<int> & boundaries)
{
    int width = boundaryLoc.size()-1;

    //set first boundary as bottom row
    boundaries.push_back(width);

    //for all the boundaries
    for (int i=width; i>-1; i--)
    {
        if (i%7 == 0 || (i-5)%7 == 0)
        {
            boundaries.push_back(i);
        }
    }

    //set last boundary as top row
    boundaries.push_back(0);
}


void makeChannelVec (std::vector<int>& boundaries, std::vector<std::pair<int,int> > & channels)
{
    std::pair<int,int> newChannel;

    for(size_t i=0; i<boundaries.size()/2; i++)
    {
        newChannel.first = boundaries[2*i];
        newChannel.second = boundaries[2*i+1];

        channels.push_back(newChannel);
    }
}


int terminalOffset(int terminal, int rotation)
{
    int offset = 0;
    switch(rotation)
    {
        case 1 :
            switch(terminal)
            {
                case 1 : offset = -5; break;
                case 2 : offset = -5; break;
                case 3 : offset = 0; break;
                case 4 : offset = 0; break;
                default : break;
            }
            break;

        case 2 :
            switch(terminal)
            {
                case 1 : offset = -5; break;
                case 2 : offset = 0; break;
                case 3 : offset = -5; break;
                case 4 : offset = 0; break;
                default : break;
            }
            break;

        case 3 :
            switch(terminal)
            {
                case 1 : offset = 0; break;
                case 2 : offset = 0; break;
                case 3 : offset = -5; break;
                case 4 : offset = -5; break;
                default : break;
            }
            break;

        case 4 :
            switch(terminal)
            {
                case 1 : offset = 0; break;
                case 2 : offset = -5; break;
                case 3 : offset = 0; break;
                case 4 : offset = -5; break;
                default : break;
            }
            break;

        case 5 :
            switch(terminal)
            {
                case 1 : offset = -5; break;
                case 2 : offset = 0; break;
                default : break;
            }
            break;

        case 6 :
            switch(terminal)
            {
                case 1 : offset = 0; break;
                case 2 : offset = -5; break;
                default : break;
            }
            break;

        default : break;
    }

    return offset;
}


void isGlobal(std::vector<std::pair<int,int> > channels, net & netPair, std::vector<cell> cellData)
{
    cell cellA = cellData[netPair.src.first];
    int termA = netPair.src.second;

    cell cellB = cellData[netPair.dest.first];
    int termB = netPair.dest.second;

    //find boundary lines each terminal falls on
    int boundaryA = cellA.y + terminalOffset(termA, cellA.r);
    int boundaryB = cellB.y + terminalOffset(termB, cellB.r);

    // see if both boundaries are in same channel
    if (abs(boundaryA-boundaryB) < 3)
    {
        netPair.global = false;

        for (int j=0; j<channels.size(); j++)
        {
            if (boundaryA == channels[j].first || boundaryA == channels[j].second)
            {
                if (boundaryB == channels[j].first || boundaryB == channels[j].second)
                    netPair.channel = j; break;
            }
        }

    }
    else
    {
        netPair.global = true;

    }
}

