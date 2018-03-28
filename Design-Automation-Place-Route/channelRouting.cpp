
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
    //Construct a vector for each boundary in the channel
    size_t width = layout[0].size();
    std::vector<chan> channelVec (channels.size(), chan(width, 0));

    std::vector<int> netID (channels.size(),0);

    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        int chanIndex = netlistPairs[i].channel;
        int boundTop = channels[chanIndex].first;
        int boundBottom = channels[chanIndex].second;
        bool cycle = false;

        netID[chanIndex]++;

        coord srcTerm = terminalCoords(netlistPairs[i].src, cellData);
        coord destTerm = terminalCoords(netlistPairs[i].dest, cellData);

        netlistPairs[i].setSpan(srcTerm.x, destTerm.x);
        channelVec[chanIndex].netPointer.push_back(i);

        //enter the source terminal into the appropriate boundary vector
        if(srcTerm.y == boundTop)  channelVec[chanIndex].top[srcTerm.x] = netID[chanIndex];
        else if(srcTerm.y == boundBottom)  channelVec[chanIndex].bottom[srcTerm.x] = netID[chanIndex];

        //enter the destination terminal into the appropriate boundary vector
        if(destTerm.y == boundTop)  channelVec[chanIndex].top[destTerm.x] = netID[chanIndex];
        else if(destTerm.y == boundBottom)  channelVec[chanIndex].bottom[destTerm.x] = netID[chanIndex];
    }
    
    for(size_t i=0; i<netID.size(); i++)
    {
        channelVec[i].numNets = netID[i];
    }

    ///////////////////////////////////////////////
    //For each of the channels in layout....///////
    ///////////////////////////////////////////////

    for(size_t N=0; N<channelVec.size(); N++)
    {
        printf("\n\n\n");
        printf("\n\nCHANNEL %i\n\n",N+1);
        
    
        int atRow = channels[N].first;
        int netsRemaining = channelVec[N].numNets;
        int previousPlacement;
        bool cycle = false;
        
        //if the bottom channel has nets in it, create space to insert tracks via normal method
        if(N==0)
        {
            if(channelVec[N].numNets>0)
            {
                appendRows(2, layout);
                updateBelow(2, atRow, cellData, netlistPairs, boundaries, channels);
                atRow += 2;
            }
        }
        
        //Loop through boundary vectors to create HCG (undirected graph)
        std::vector<numberList> HCG;
        std::vector<netPos> indexPairs(channelVec[N].numNets);
        HCG = makeHCG(channelVec[N], netlistPairs, indexPairs);
        
        //Loop through boundary vectors to create VCG (directed graph)
        std::vector<numberList> VCG;
        VCG = makeVCG(channelVec[N]);

        
        
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
                bool free = noConstraints(VCG, indexPairs[i].net, indexPairs);
                
                if (indexPairs[i].x > watermark && !indexPairs[i].placed && free)
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
        
        //        for (int i=0;i<indexPairs.size();i++)
        //        {
        //            std::cout<<indexPairs[i].track<<"\n";
        //        }
        
        ////////////////BLAKES NEW CODE///////////////////
        
        
        ////////////////AARONS NEW CODE///////////////////
        while(netsRemaining > 0)
        {
            if(!cycle)
            {
                addTrack(2, atRow, cellData, netlistPairs, layout, boundaries, channels);
                previousPlacement = 0;
            }
            else
            {
                for(size_t i=0; i<HCG.size(); i++)
                {
                    net& currentNet = netlistPairs[channelVec[N].netPointer[i]];
                    if(!currentNet.placed)
                    {
                        currentNet.placed = true; //split the first unrouted net in HCG
                        netsRemaining --;
                    }
                }
            }
            
            cycle = true; //assume cycle is true before scanning HCG (which could prove this assumption false)

            for(size_t i=0; i<HCG.size(); i++)
            {
                net& currentNet = netlistPairs[channelVec[N].netPointer[i]];
                int ID = HCG[i].findHead();
                
                if(!currentNet.placed && !HCG[i].findVal(previousPlacement) && VCG[ID-1].isEmpty(ID))
                {
                    if(!VCG[ID-1].isEmpty(ID)){printf("\n\t\tVCG[%i] not empty\n", ID);}
                    
                    makeTrunk(currentNet, atRow, layout);
                    removeChild(i, HCG, VCG);
                    previousPlacement = ID;
                    netsRemaining--;
                    cycle = false;
                }
            }
        }
        ////////////////AARONS NEW CODE///////////////////

        
        //////PRINTING//////////
        
        //printing HCG
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
        
        //printing VCG
        std::cout << "\n\nVertical Constraint Graph:\n\n";

        for(size_t i=0; i<VCG.size(); i++)
        {
            std::cout << i+1 <<":\t";
            VCG[i].display();
            std::cout << "\n\n";
        }

        std::cout << "\n\n\n\n";
    }

    printf("\nDid channel routing\n\n");

}




void updateBelow(int numRows, int atRow, std::vector<cell> & cellData, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
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
    
    for (size_t l=0; l<netlistPairs.size(); l++)
    {
        if (netlistPairs[l].y >= atRow)
        {
            netlistPairs[l].y += numRows;
        }
    }
}


void addTrack(int numRows, int atRow, std::vector<cell> & cellData, std::vector<net> & netlistPairs, std::vector<std::vector<int> > & layout, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
{
    addRows(numRows, atRow, layout);
    updateBelow(numRows, atRow, cellData, netlistPairs, boundaries, channels);
}


std::vector<numberList> makeHCG(chan C, std::vector<net> & netlistPairs, std::vector<netPos> & indexPairs)
{
    std::vector<std::vector<int> > netGraph(C.width);
    std::vector<numberList> graph(C.numNets);
    int x1, x2;

    for(int i=0; i<C.numNets; i++)
    {
        graph[i].appendNode(i+1); //create a vector of list heads in number order

        x1 = netlistPairs[C.netPointer[i]].x1;
        x2 = netlistPairs[C.netPointer[i]].x2;
        
        if(x1>0) x1-=1;
        if(x2<C.width) x2+=1;
        
        ////////////////BLAKES NEW CODE///////////////////
        indexPairs[i].x = x1;
        indexPairs[i].y = x2;
        indexPairs[i].net = i+1;
        ////////////////BLAKES NEW CODE///////////////////


        for(size_t j = x1; j<x2+1; j++)
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
//    for(int i=0; i<C.numNets; i++)
//    {
//        graph[i].appendNode(i+1);
//    }

    for(size_t i=0; i<C.width; i++)
    {
        if(C.top[i] > 0)
        {
            if(0 < i && i < C.width-1)
            {
                if(C.bottom[i-1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0) graph[C.top[i]-1].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i+1]);
                
                /////////////////////////////////////////////
                //Find VCG cycles for dogleg/////////////////
                if(C.bottom[i-1] > 0) if(graph[C.bottom[i-1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i\t\t\t\n\n", C.top[i]);
                if(C.bottom[i] > 0) if(graph[C.bottom[i]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i\t\t\t\n\n", C.top[i]);
                if(C.bottom[i+1] > 0) if(graph[C.bottom[i+1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i\t\t\t\n\n", C.top[i]);
                /////////////////////////////////////////////
            }
            else if (i == 0)
            {
                if(C.bottom[i] > 0) graph[C.top[i]-1].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i+1]);
                
                /////////////////////////////////////////////
                //Find VCG cycles for dogleg/////////////////
                if(C.bottom[i] > 0) if(graph[C.bottom[i]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i\t\t\t\n\n", C.top[i]);
                if(C.bottom[i+1] > 0) if(graph[C.bottom[i+1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i\t\t\t\n\n", C.top[i]);
                /////////////////////////////////////////////
            }
            else
            {
                if(C.bottom[i-1] > 0) graph[C.top[i]-1].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0) graph[C.top[i]-1].appendNode(C.bottom[i]);
                
                /////////////////////////////////////////////
                //Find VCG cycles for dogleg/////////////////
                if(C.bottom[i-1] > 0) if(graph[C.bottom[i-1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i\t\t\t\n\n", C.top[i]);
                if(C.bottom[i] > 0) if(graph[C.bottom[i]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i\t\t\t\n\n", C.top[i]);
                /////////////////////////////////////////////
            }
        }
    }

    return graph;
}

void removeChild(int netNum, std::vector<numberList>& HCG, std::vector<numberList>& VCG)
{
    std::vector<int> connections = HCG[netNum].returnList();
    
    for(size_t i=0; i<connections.size(); i++)
    {
        printf("%i->%i   ",connections[i], HCG[netNum].findHead());
        VCG[connections[i]-1].removeAll(HCG[netNum].findHead());
        printf("\n");
        VCG[connections[i]-1].display();
    }
    
    printf("\n");
}

bool netCompare(netPos a, netPos b)
{
    return a.x < b.x;
}

bool noConstraints(std::vector<numberList> &VCG, int num, std::vector<netPos> & indexPairs)
{
    bool free = true;
    for (int i=0;i<VCG.size();i++)
    {
        if(!indexPairs[i].placed && num != i+1)
        {
            free = VCG[i].isFree(num);

            if(!free)
            {
                return false;
            }
        }
    }
    return true;
}














