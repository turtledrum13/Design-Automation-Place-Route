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
    chan dummy;
    dummy.top.resize(layout[0].size(), 0);
    dummy.bottom.resize(layout[0].size(), 0);

    std::vector<chan> channelVec (channels.size(), dummy);

    std::cout << channelVec.size() << std::endl;
    std::vector<int> netID (channels.size(),1);
    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        int index = netlistPairs[i].channel;
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

        //enter the destination terminal into the appropriate boundary vector
        if(destBound.y == boundTop)
        {
            channelVec[index].top[destBound.x] = netID[index];
        }
        else if(destBound.y == boundBottom)
        {
            channelVec[index].bottom[destBound.x] = netID[index];
        }

        netID[index]++;
    }


    printf("\nnetID:\n");
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
        for(size_t j=0; j<channelVec[i].bottom.size(); j++)
        {
            printf("%i ",channelVec[i].bottom[j]);
        }
        printf("\n\n\n\n");
    }


    //For each channel
    for(size_t N=0; N<channelVec.size(); N++)
    {

        //make room for the first track
        //if (addTrack(2, channel.N.second, cellData, layout);

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

std::vector<numberList> HCG(int numNets, std::vector<int> top, std::vector<int> bottom)
    {
        std::vector<numberList> graph;
        
        //make head nodes
        for(size_t i=0; i<bottom.size(); i++)
        {
            if(bottom[i] > 0)
            {
                numberList dummy;
                graph.push_back(dummy);
                graph[graph.size()-1].appendNode(bottom[i]);
            }
        }

    //check for horizontal constraints and appending to each other if detected
    //scan through top and bottom simultaneously. If on some index number there is a top and a bottom both greater than 0, then append the bottom to the top's list
    //if any other IDs are at position equal to or within the focused one, add it to each other's list
    for(size_t i=0; i<numNets; i++)
    {
        int currentNet = graph[i].findHead(); //graph[i].getHead() //net of focus is the head of the

        for(size_t j=0; j<bottom.size(); j++)
        {

            size_t first = 0, last = 0;
            if(first == 0)
            {
                if(top[j] == currentNet || bottom[j] == currentNet)
                {
                    if(top[j] == bottom[j])
                    {
                        first = j+1;
                        last = j+1;
                    }
                    else
                    {
                        first = j+1;
                    }
                }
            }

            if(first != 0 && last == 0)
            {
                if(top[j] > 0 || bottom[j] > 0)
                {
                    if(top[j] == currentNet || bottom[j] == currentNet)
                    {
                        last = j+1;
                    }
                    //add to graph of i and add i to its graph
                    
                    
                    
                }
            }



        }
    }

    return graph;
}

std::vector<numberList> VCG(int numNets, std::vector<int> top, std::vector<int> bottom)
{
    std::vector<numberList> graph(numNets);

    //make head nodes
    for(size_t i=0; i<top.size(); i++)
    {
        if(top[i] > 0 && bottom[i] > 0)
        {
            graph[top[i]].appendNode(bottom[i]);
        }
    }
    //check for horizontal constraints and appending to each other if detected
    //scan through top and bottom simultaneously. If on some index number there is a top and a bottom both greater than 0, then append the bottom to the top's list


    return graph;
}
