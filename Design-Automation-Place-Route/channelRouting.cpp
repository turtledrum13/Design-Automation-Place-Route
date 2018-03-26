//
//  channelRouting.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "channelRouting.hpp"
#include "layoutOperations.hpp"
#include "globalRouting.hpp"
#include "structures.h"
#include <stdio.h>


void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<net> & netlistPairs, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries)
{
    //Construct vector for each boundary in the channel

    struct chan
    {
        std::vector<int> top, bottom;
    };
    
    chan dummy_chan;
    dummy_chan.top.resize(layout[0].size(), 0);
    dummy_chan.bottom.resize(layout[0].size(), 0);

    std::vector<chan> channelVec (channels.size(), dummy_chan);

    std::cout << channelVec.size() << std::endl;
    std::vector<int> netID (channels.size(),1);
    int maxChannel = 0;
    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        int index = netlistPairs[i].channel;
        if (index > maxChannel) maxChannel = index;
        int boundTop = channels[index].first;
        int boundBottom = channels[index].second;

        coord srcBound = terminalCoords(netlistPairs[i].c1, cellData);
        coord destBound = terminalCoords(netlistPairs[i].c2, cellData);
        
        //enter the source terminal into the appropriate boundary vector
        if(srcBound.y == boundTop)
        {
            channelVec[index].top[srcBound.x] = netID[index];
        }
        else if(srcBound.y == boundBottom)
        {
            channelVec[index].bottom[srcBound.x] = netID[index];
        }
        
//        //enter the destination terminal into the appropriate boundary vector
//        if(destBound.y == boundTop)
//        {
//            channelVec[index].top[destBound.x] = netID[index];
//        }
//        else if(destBound.y == boundBottom)
//        {
//            channelVec[index].bottom[destBound.x] = netID[index];
//        }
        
        netID[index]++;
    }
    
    printf("max channel: %i\n",maxChannel);
    
    printf("netID:\n");
    for(size_t i=0; i<netID.size(); i++)
    {
        printf("%i ",netID[i]);
    }
    printf("\n\n");


    
    for(size_t i=0; i<channelVec.size(); i++)
    {
        for(size_t j=0; j<channelVec[i].top.size(); j++)
        {
            printf("%i ",channelVec[i].top[j]);
        }
        printf("\n\n");
        for(size_t j=0; j<channelVec[i].top.size(); j++)
        {
            printf("%i ",channelVec[i].top[j]);
        }
        printf("\n\n\n\n");
    }
    
    
    //For each channel
    for(size_t i=0; i<channelVec.size(); i++)
    {
        
        
        //make room for the first track
        //if (addTrack(2, (bound1+bound2)/2, cellData, layout);
        
        //Loop through boundary vectors to create HCG (undirected graph)
        
        //Loop through boundary vectors to create VCG (directed graph)
        
        //Perform left-edge routing with dogleg
        //Loop through HCG --> VCG
            //maintain current track # (initialized to the row between the channel indeces)
            //update net.placed as you go and delete nodes from the graphs
            //addTrack(2, currentTrack, cellData, layout); (only as necessary)
        //when HCG and VCG are through, exit loop
        
        //increment channel

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
    //need to update boundaries and channel pairs, too
    //might need to update layout things as well?? - probably not
}
