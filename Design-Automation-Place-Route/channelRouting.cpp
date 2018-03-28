
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
#include <algorithm>
#include "lin.h"


void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<net> & netlistPairs, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries)
{
    //Construct vector for each boundary in the channel
    size_t width = layout[0].size();
    std::vector<chan> channelVec (channels.size(), chan(width, 0));
    
    std::vector<int> netID (channels.size(),0);
    
    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        int chanIndex = netlistPairs[i].channel;
        int boundTop = channels[chanIndex].first;
        int boundBottom = channels[chanIndex].second;
        
        netID[chanIndex]++;
        
        coord srcTerm = terminalCoords(netlistPairs[i].c1, cellData);
        coord destTerm = terminalCoords(netlistPairs[i].c2, cellData);
        
        netlistPairs[i].setSpan(srcTerm.x, destTerm.x);
        channelVec[chanIndex].netPointer.push_back(i);
        
        //enter the source terminal into the appropriate boundary vector
        if(srcTerm.y == boundTop)
        {
            channelVec[chanIndex].top[srcTerm.x] = netID[chanIndex];
        }
        else if(srcTerm.y == boundBottom)
        {
            channelVec[chanIndex].bottom[srcTerm.x] = netID[chanIndex];
        }
        
        //enter the destination terminal into the appropriate boundary vector
        if(destTerm.y == boundTop)
        {
            channelVec[chanIndex].top[destTerm.x] = netID[chanIndex];
        }
        else if(destTerm.y == boundBottom)
        {
            channelVec[chanIndex].bottom[destTerm.x] = netID[chanIndex];
        }
    }
    
    
    printf("\nnumber of nets by channel:\n");
    for(size_t i=0; i<netID.size(); i++)
    {
        channelVec[i].numNets = netID[i];
        printf("%i ",netID[i]);
        
    }
    printf("\n\n\n\n");
    
    
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////
    
    //For each of the channels in layout
    for(size_t N=0; N<channelVec.size(); N++)
    {
        //make room for the first track
        if (netID[N] > 0)
        {
            addTrack(2, channels[N].second, cellData, layout, boundaries, channels);
        }
        
        printf("\n\nCHANNEL %i\n\n",N+1);
        
        //Loop through boundary vectors to create HCG (undirected graph)
        std::vector<numberList> HCG;
        std::vector<netPos> indexPairs(channelVec[N].numNets);
        
        HCG = makeHCG(channelVec[N], netlistPairs, indexPairs);
        
        
        
        std::cout << "\nHorizontal Constraint Graph:\n\n";
        
        for(size_t i=0; i<HCG.size(); i++)
        {
            HCG[i].display();
            std::cout << "\n\n";
        }
        
        
        //printing out channel boundaries for debugging
        printf("\n\n");
        for(size_t j=0; j<channelVec[N].width; j++)
        {
            printf("%i ",channelVec[N].top[j]);
        }
        printf("\n\n");
        for(size_t j=0; j<channelVec[N].width; j++)
        {
            printf("%i ",channelVec[N].bottom[j]);
        }
        printf("\n\n\n");
        
        
        //Loop through boundary vectors to create VCG (directed graph)
        std::vector<numberList> VCG;
        VCG = makeVCG(channelVec[N]);
        
        std::cout << "\n\nVertical Constraint Graph:\n\n";
        
        for(size_t i=0; i<VCG.size(); i++)
        {
            std::cout << i+1 <<":\t";
            VCG[i].display();
            std::cout << "\n\n";
        }
        
        std::cout << "\n\n\n\n";
        
        
        ////////////////BLAKES NEW CODE///////////////////
        for (int i=0; i< indexPairs.size(); i++)
        {
            indexPairs[i].placed = false;
        }
        
        int watermark = 0, track = 0;
        std::sort(indexPairs.begin(), indexPairs.end(), netCompare);
        
        while (true)
        {
            track += 1;
            watermark = 0;
            
            for (int i=0; i<indexPairs.size();i++)
            {
                if (indexPairs[i].x > watermark && indexPairs[i].placed == false)
                {
                    indexPairs[i].track = track;
                    indexPairs[i].placed = true;
                    watermark = indexPairs[i].y;
                }
            }
            
            if (watermark==0)
            {
                break;
            }
        }
        
        for (int i=0;i<indexPairs.size();i++)
        {
            std::cout<<indexPairs[i].track<<"\n";
        }
        
        ////////////////BLAKES NEW CODE///////////////////

        
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


std::vector<numberList> makeHCG(chan C, std::vector<net> & netlistPairs, std::vector<netPos> & indexPairs)
{
    std::vector<std::vector<int> > netGraph(C.width);
    std::vector<numberList> graph(C.numNets);
    int index1, index2;

    for(int i=0; i<C.numNets; i++)
    {
        graph[i].appendNode(i+1); //create a vector of list heads in number order
        
        index1 = netlistPairs[C.netPointer[i]].span.first;
        index2 = netlistPairs[C.netPointer[i]].span.second;
        
        
        ////////////////BLAKES NEW CODE///////////////////
        indexPairs[i].x = index1;
        indexPairs[i].y = index2;
        indexPairs[i].net = i+1;
        ////////////////BLAKES NEW CODE///////////////////

        
        for(size_t j = index1; j<index2+1; j++)
        {
            netGraph[j].push_back(i+1);
        }
    }

    for(int i=0; i<C.width; i++)
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
            if(0 < i && i < C.width-1)
            {
                if(C.bottom[i-1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0) graph[C.top[i]-1].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i+1]);
            }
            else if (i == 0)
            {
                if(C.bottom[i] > 0) graph[C.top[i]-1].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i+1]);
            }
            else
            {
                if(C.bottom[i-1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0) graph[C.top[i]-1].appendNode(C.bottom[i]);
            }
        }
    }
    
    return graph;
}

bool netCompare(netPos a, netPos b)
{
    return a.x < b.x;
}
