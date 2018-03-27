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
    size_t width = layout[0].size();
    std::vector<chan> channelVec (channels.size(), chan(width, 0));

    std::cout << channelVec.size() << std::endl;
    std::vector<int> netID (channels.size(),0);

    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        int index = netlistPairs[i].channel;
        int boundTop = channels[index].first;
        int boundBottom = channels[index].second;

        netID[index]++;

        coord srcTerm = terminalCoords(netlistPairs[i].c1, cellData);
        coord destTerm = terminalCoords(netlistPairs[i].c2, cellData);

        //enter the source terminal into the appropriate boundary vector
        if(srcTerm.y == boundTop)
        {
            channelVec[index].top[srcTerm.x] = netID[index];
        }
        else if(srcTerm.y == boundBottom)
        {
            channelVec[index].bottom[srcTerm.x] = netID[index];
        }

        //enter the destination terminal into the appropriate boundary vector
        if(destTerm.y == boundTop)
        {
            channelVec[index].top[destTerm.x] = netID[index];
        }
        else if(destTerm.y == boundBottom)
        {
            channelVec[index].bottom[destTerm.x] = netID[index];
        }
    }


    printf("\nnumber of nets by channel:\n");
    for(size_t i=0; i<netID.size(); i++)
    {
        channelVec[i].numNets = netID[i];
        printf("%i ",netID[i]);
    }
    printf("\n\n\n\n");


    //printing out channel boundaries for debugging
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


    //For each of the channels in layout
    for(size_t N=1; N<2; N++) //channelVec.size()
    {
        //make room for the first track
        if (netID[N] > 0)
        {
            addTrack(2, channels[N].second, cellData, layout, boundaries, channels);
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
        std::vector<numberList> VCG;
        VCG = makeVCG(channelVec[N]);

        //Perform left-edge routing (with dogleg later)
        //Loop through HCG --> VCG
        //maintain current track # (initialized to the row between the channel indeces)
        //update net.placed as you go and delete nodes from the graphs
        //addTrack(2, currentTrack, cellData, layout); (only as necessary)
        //when HCG and VCG are through, exit loop

        //increment channel

    }

    printf("\nDid channel routing\n\n");

}

void updateBelow(int numRows, int atRow, std::vector<cell> & cellData, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
{
    for (size_t i=0; i<cellData.size(); i++)
    {
        if (cellData[i].y >= atRow)
        {
            cellData[i].y += numRows;
        }
    }
    
    for (size_t j=0; j<boundaries.size(); j++)
    {
        if (boundaries[j] >= atRow)
        {
            boundaries[j] += numRows;
        }
    }
    
    for (size_t k=0; k<channels.size(); k++)
    {
        if (channels[k].second >= atRow)
        {
            channels[k].second += numRows;
        }
        if (channels[k].first >= atRow)
        {
            channels[k].first += numRows;
        }
    }
}


void addTrack(int numRows, int atRow, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
{
    addRows(numRows, atRow, layout);
    updateBelow(numRows, atRow, cellData, boundaries, channels);
}


std::vector<numberList> makeHCG(chan C)
{
    std::vector<std::vector<int> > netGraph(C.bottom.size());
    std::vector<numberList> graph(C.numNets);
    int index1, index2;

    for (int i=0; i<C.numNets; i++)
    {
        graph[i].appendNode(i+1);
    }

    for(int i=0; i<C.numNets; i++)
    {
        index1 = -1;
        index2 = -1;

        for(size_t j = 0; j<C.bottom.size(); j++)
        {
            if (C.bottom[j]==i+1)
            {
                if(index1 > -1)
                {
                    index2 = j;
                    break;
                }
                else
                {
                    index1 = j;
                }
            }

            if (C.top[j]==i+1)
            {
                if(index1 > -1)
                {
                    index2 = j;
                    break;
                }
                else
                {
                    index1 = j;
                }
            }
        }

        for (int j=index1; j<index2+1; j++)
        {
            netGraph[j].push_back(i+1);
        }
    }

    for(int i=0; i<C.bottom.size(); i++)
    {
        for (int j=0; j<netGraph[i].size(); j++)
        {
            for(int k=0; k<netGraph[i].size(); k++)
            {
                if(k!=j && graph[netGraph[i][j]-1].lookUp(netGraph[i][k])==0)
                {
                    graph[netGraph[i][j]-1].appendNode(netGraph[i][k]);
                }
            }
        }
    }

    /*//initialize HCG
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
    }*/

    //reorder the HCG by left first
    std::vector<numberList> orderedGraph;
    std::vector<bool> placed (C.numNets, false);

    for(size_t k=0; k<C.bottom.size(); k++)
    {
        if(C.bottom[k] > 0)
        {
            if(not placed[C.bottom[k]-1])
            {
                orderedGraph.push_back(graph[C.bottom[k]-1]);
                placed[C.bottom[k]-1] = true;
            }
        }

        if(C.top[k] > 0)
        {
            if(not placed[C.top[k]-1])
            {
                orderedGraph.push_back(graph[C.top[k]-1]);
                placed[C.top[k]-1] = true;
            }
        }
    }

    return orderedGraph;
}


std::vector<numberList> makeVCG(chan C)
{
    std::vector<numberList> graph(C.numNets);

    for(size_t i=0; i<C.width; i++)
    {
        if(C.top[i] > 0)
        {
            if(0 < i < C.width-1)
            {
                if(C.bottom[i-1] > 0) graph[C.top[i]].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0) graph[C.top[i]].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0) graph[C.top[i]].appendNode(C.bottom[i+1]);
            }
            else if (i == 0)
            {
                if(C.bottom[i] > 0) graph[C.top[i]].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0) graph[C.top[i]].appendNode(C.bottom[i+1]);
            }
            else
            {
                if(C.bottom[i-1] > 0) graph[C.top[i]].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0) graph[C.top[i]].appendNode(C.bottom[i]);
            }
        }
    }

    return graph;
}
