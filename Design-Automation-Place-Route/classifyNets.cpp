//
//  classifyNets.cpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.
//

#include "classifyNets.hpp"
#include <vector>
#include <stdlib.h>


void classifyNets(std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
{
    makeBoundaryVec(layout.size()-1, boundaries);
    makeChannelVec(boundaries, channels);

    for (int i=0; i<netlistPairs.size(); i++)
    {
        isGlobal(channels, netlistPairs[i], cellData);
    }
}


void makeBoundaryVec (int width, std::vector<int> & boundaries)
{
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
        case 0:
        {
            offset = 0; break;
        }
        case 1 :
            switch(terminal)
            {
                case 0: offset = 0; break;
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
                case 0: offset = 0; break;
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
                case 0: offset = 0; break;
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
                case 0: offset = 0; break;
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
                case 0: offset = 0; break;
                case 1 : offset = -5; break;
                case 2 : offset = 0; break;
                default : break;
            }
            break;

        case 6 :
            switch(terminal)
            {
                case 0: offset = 0; break;
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

