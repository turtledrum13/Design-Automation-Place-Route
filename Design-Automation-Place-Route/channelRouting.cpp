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
#include "lin.h"


void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<net> & netlistPairs, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries)
{
    //Construct vector for each boundary in the channel
    chan dummy;
    dummy.top.resize(layout[0].size(), 0);
    dummy.bottom.resize(layout[0].size(), 0);

    std::vector<chan> channelVec (channels.size(), dummy);

    std::cout << channelVec.size() << std::endl;
    std::vector<int> netID (channels.size(),0);
    
    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        int index = netlistPairs[i].channel;
        int boundTop = channels[index].first;
        int boundBottom = channels[index].second;
        
        netID[index]++;

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
    for(size_t N=5; N<6; N++) //channelVec.size()
    {
        //make room for the first track
        if (netID[N] > 0)
        {
            addTrack(2, channels[N].second, cellData, layout);
        }

        //Loop through boundary vectors to create HCG (undirected graph)
        std::vector<numberList> HCG;
        HCG = makeHCG(netID[N], channelVec[N].top, channelVec[N].bottom);
        
        for(size_t i=0; i<HCG.size(); i++)
        {
            HCG[i].display();
            std::cout << "\n\n";
        }
        
        //Loop through boundary vectors to create VCG (directed graph)
        makeVCG(netID[N], channelVec[N].top, channelVec[N].bottom);

        //Perform left-edge routing (with dogleg later)
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

std::vector<numberList> makeHCG(int numNets, std::vector<int> top, std::vector<int> bottom)
{
    //initialize HCG
    std::vector<numberList> graph(numNets);
    
    //make head nodes in the order in which they occur
    for(size_t i=0; i<bottom.size(); i++)
    {
        if(bottom[i] > 0)
        {
            if(!graph[bottom[i]-1].notEmpty())
            {
                graph[bottom[i]-1].appendNode(bottom[i]);
            }
        }
        
        if(top[i] > 0)
        {
            if(!graph[top[i]-1].notEmpty())
            {
                graph[top[i]-1].appendNode(top[i]);
            }
        }
    }

//check for horizontal constraints and appending to each other if detected
//if any other IDs are at position equal to or within the focused one, add it to each other's list

    for(size_t i=0; i<numNets; i++)
    {
        //net of focus is the head of the list in this graph position
        int currentNet = i+1;

        //scan through the whole boundary
        for(size_t j=0; j<bottom.size(); j++)
        {
            size_t first = 0, last = 0; //flag variables initilized to zero
            
            //so long as the first terminal of the net has not been found...
            if(first == 0)
            {
                //if either the top or bottom boundary block is part of currentNet...
                if(top[j] == currentNet || bottom[j] == currentNet)
                {
                    if(top[j] == bottom[j]) //if both top and bottom are found at once, we're done
                    {
                        first = j+1;
                        last = j+1;
                    }
                    else                    //if only one terminal of the net found, set first = index
                    {
                        first = j+1;
                    }
                }
            }

            //if the first terminal but not the last has been found
            if(first != 0 && last == 0)
            {
                if(top[j] > 0 || bottom[j] > 0)
                {
                    //add to graph of i and add i to its graph
                    if(top[j] > 0 && top[j] != currentNet)
                    {
                        graph[i].appendNode(top[j]);
                        //graph[top[j]-1].appendNode(i+1);
                    }
                    
                    if(bottom[j] > 0 && bottom[j] != currentNet)
                    {
                        graph[i].appendNode(bottom[j]);
                        //graph[bottom[j]-1].appendNode(i+1);
                    }
                    
                    //close down the search if the last terminal is found
                    if(top[j] == currentNet || bottom[j] == currentNet)
                    {
                        last = j+1;
                    }
                }
            }
        }
    }
    
    //reorder the HCG by left first
    std::vector<numberList> orderedGraph;
    std::vector<bool> placed (numNets, false);
    
    for(size_t k=0; k<bottom.size(); k++)
    {
        if(bottom[k] > 0)
        {
            if(not placed[bottom[k]-1])
            {
                orderedGraph.push_back(graph[bottom[k]-1]);
                placed[bottom[k]-1] = true;
            }
        }
        
        if(top[k] > 0)
        {
            if(not placed[top[k]-1])
            {
                orderedGraph.push_back(graph[top[k]-1]);
                placed[top[k]-1] = true;
            }
        }
    }
    
    return orderedGraph;
}

std::vector<numberList> makeVCG(int numNets, std::vector<int> top, std::vector<int> bottom)
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

    return graph;
}
