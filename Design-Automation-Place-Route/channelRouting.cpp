
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
#include "constraintList.hpp"
#include "structures.h"
#include <stdio.h>
#include <algorithm>



void channel(std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<net> & netlistPairs, std::vector<std::pair <int,int> > channels, std::vector<int> boundaries)
{
    //Construct a vector for each boundary in the channel
    size_t width = layout[0].size();
    std::vector<chan> channelVec (channels.size(), chan(width, 0)); //vector of chan structures which store information about each channel

    std::vector<int> netID (channels.size(),0);                     //counter for the number of nets in each channel

    for(size_t i=0; i<netlistPairs.size(); i++)                     //for each net in the design...
    {
        int channelVecIndex = netlistPairs[i].channel;
        int boundTop = channels[channelVecIndex].second;
        int boundBottom = channels[channelVecIndex].first;

        netID[channelVecIndex]++;

        coord srcTerm = terminalCoords(netlistPairs[i].src, cellData);
        coord destTerm = terminalCoords(netlistPairs[i].dest, cellData);

        netlistPairs[i].setSpan(srcTerm.x, destTerm.x);
        channelVec[channelVecIndex].nets.push_back(&netlistPairs[i]);

        //enter the source terminal into the appropriate boundary vector
        if(srcTerm.y == boundTop && srcTerm.y != 0)  channelVec[channelVecIndex].top[srcTerm.x] = netID[channelVecIndex];
        else if(srcTerm.y == boundBottom)  channelVec[channelVecIndex].bottom[srcTerm.x] = netID[channelVecIndex];

        //enter the destination terminal into the appropriate boundary vector
        if(destTerm.y == boundTop && srcTerm.y != 0)  channelVec[channelVecIndex].top[destTerm.x] = netID[channelVecIndex];
        else if(destTerm.y == boundBottom)  channelVec[channelVecIndex].bottom[destTerm.x] = netID[channelVecIndex];
    }

    printf("\nNets by Channel:\n");
    for(size_t i=0; i<netID.size(); i++)
    {
        printf("%i  ",netID[i]);
        channelVec[i].numNets = netID[i];
    }
    printf("\n\nTotal number of nets: %zu",netlistPairs.size());


    
    ///////////////////////////////////////////////
    //For each of the channels in layout....///////
    ///////////////////////////////////////////////

    for(size_t N=0; N<channelVec.size(); N++)
    {
        printf("\n\n\n");
        printf("\n\nCHANNEL %zu\n\n",N+1);
        
//        for(int i=0; i<channelVec[N].nets.size(); i++)
//        {
//            printf("%i  ",channelVec[N].nets[i]->num);
//        }
        
    
        int atRow = channels[N].first;
        int netsRemaining = channelVec[N].numNets;
        int previousPlacement;
        int numTracks = 0;
        bool cycle = false;
        
        //if the bottom channel has nets in it, create space to insert tracks via normal method
        if(N==0)
        {
            if(channelVec[N].numNets>0)
            {
                appendRows(2, layout);
                boundaries[0] += 2;
                channels[0].first += 2;
                atRow += 2;
            }
        }

        //Loop through boundary vectors to create HCG (undirected graph)
        std::vector<constraintList> HCG;
        HCG = makeHCG(channelVec[N], netlistPairs);

        //Loop through boundary vectors to create VCG (directed graph)
        std::vector<constraintList> VCG;
        VCG = makeVCG(channelVec[N]);



        while(netsRemaining > 0)
        {

            //////////PRINT UNROUTED NETS///////////
            printf("\n\n%i Unrouted: \n", netsRemaining);
            for(size_t i=0; i<HCG.size(); i++)
            {
                int ind = HCG[i].findHead()-1;
                
                if(!channelVec[N].nets[ind]->routed)
                {
                    //printf("%i  ",HCG[i].findHead());
                    printf("NET#%i    %i-[%i->%i]\t",channelVec[N].nets[ind]->num, HCG[i].findHead(), channelVec[N].nets[ind]->x1, channelVec[N].nets[ind]->x2);
                    printf("src cell: %i @ (%i,%i),\t", channelVec[N].nets[ind]->src.first, cellData[channelVec[N].nets[ind]->src.first].x, cellData[channelVec[N].nets[ind]->src.first].y);
                    printf("dest cell: %i @ (%i,%i)\n", channelVec[N].nets[ind]->dest.first, cellData[channelVec[N].nets[ind]->dest.first].x, cellData[channelVec[N].nets[ind]->dest.first].y);

                }
            }
            printf("\n\n");
            //////////PRINT UNROUTED NETS///////////


            if(!cycle)  //if not caught in a cycle, add a new track
            {
                addTrack(2, atRow, cellData, netlistPairs, layout, boundaries, channels);
                numTracks++;
                printf("Track: %i\n",numTracks);
                previousPlacement = 0;
            }
            else        //caught in a cycle and need to dogleg
            {
                printf("\n\nCYCLE  --  %i nets remaining\n\n\n", netsRemaining);
                for(size_t i=0; i<HCG.size(); i++)
                {
                    net* cycleNet = channelVec[N].nets[i];
                    if(!cycleNet->routed)  //for the first unrouted net
                    {
                        //////////TEMP DOGLEG WORKAROUND///////////
                        cycleNet->routed = true;
                        netsRemaining --;
                        addTrack(2, atRow, cellData, netlistPairs, layout, boundaries, channels);
                        numTracks++;
                        printf("Track: %i\n",numTracks);
                        previousPlacement = 0;
                        //////////TEMP DOGLEG WORKAROUND///////////
                    }
                }
            }


            cycle = true; //assume cycle is true before scanning HCG (which could prove this assumption false)

            for(size_t i=0; i<HCG.size(); i++)
            {
                int ID = HCG[i].findHead();
                net* currentNet = channelVec[N].nets[ID-1];
                
                if(!currentNet->routed && !HCG[i].findVal(previousPlacement) && VCG[ID-1].isEmpty(ID))
                {
                    makeTrunk(currentNet, atRow, layout);                           //draw horizontal component on layout
                    removeChild(i, HCG, VCG);                                       //remove net as a child from any parents in the VCG
                    if(currentNet->span != 0) previousPlacement = ID;               //keep track of the most recently placed non-zero-span net
                    netsRemaining--;                                                //decrement counter for while loop
                    cycle = false;                                                  //reset cycle flag since a net was placed
                }
            }
        }


//        //////PRINTING//////////
//
//        //printing HCG
//        std::cout << "\nHorizontal Constraint Graph:\n\n";
//
//        for(size_t i=0; i<HCG.size(); i++)
//        {
//            HCG[i].display();
//            std::cout << "\n\n";
//        }
//
//
//        //printing out channel boundaries for debugging
//        printf("\n\n");
//        for(size_t j=0; j<channelVec[N].width; j++)
//        {
//            printf("%i ",channelVec[N].top[j]);
//        }
//        printf("\n\n");
//        for(size_t j=0; j<channelVec[N].width; j++)
//        {
//            printf("%i ",channelVec[N].bottom[j]);
//        }
//        printf("\n\n\n");
//
//        //printing VCG
//        std::cout << "\n\nVertical Constraint Graph:\n\n";
//
//        for(size_t i=0; i<VCG.size(); i++)
//        {
//            std::cout << i+1 <<":\t";
//            VCG[i].display();
//            std::cout << "\n\n";
//        }
//
//        std::cout << "\n\n\n\n";
        
    }
}




void updateBelow(int numRows, int atRow, std::vector<cell> & cellData, std::vector<net> & netlistPairs, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
{
    for (size_t i=0; i<cellData.size(); i++)        //update y-coordinates of cells below insertion point
    {
        if (cellData[i].y >= atRow)
            cellData[i].y += numRows;
    }

    for (size_t j=0; j<boundaries.size(); j++)      //update y-coordinates of boundaries below insertion point
    {
        if (boundaries[j] >= atRow)
            boundaries[j] += numRows;
    }

    for (size_t k=0; k<channels.size(); k++)        //update y-coordinates of channels below insertion point
    {
        if (channels[k].second >= atRow)
            channels[k].second += numRows;
        
        if (channels[k].first >= atRow)
            channels[k].first += numRows;
    }

    for (size_t l=0; l<netlistPairs.size(); l++)    //update y-coordinates of nets below insertion point
    {
        if (netlistPairs[l].y >= atRow)
            netlistPairs[l].y += numRows;
    }
}


void addTrack(int numRows, int atRow, std::vector<cell> & cellData, std::vector<net> & netlistPairs, std::vector<std::vector<int> > & layout, std::vector<int> & boundaries, std::vector<std::pair<int,int> > & channels)
{
    insertRows(numRows, atRow, layout);
    updateBelow(numRows, atRow, cellData, netlistPairs, boundaries, channels);
}


std::vector<constraintList> makeHCG(chan C, std::vector<net> & netlistPairs)
{
    std::vector<std::vector<int> > netGraph(C.width);
    std::vector<constraintList> graph(C.numNets);
    int x1=0, x2=0;

    for(int i=0; i<C.numNets; i++)
    {
        graph[i].appendNode(i+1);               //create a vector of list heads in number order

        x1 = C.nets[i]->x1 -1;                  //-1 to account for adjacent nets
        x2 = C.nets[i]->x2 +1;                  //+1 to account for adjacent nets

        if(C.nets[i]->span != 0)                //if net has no span we won't check for horizontal constraints
        {
            for(size_t j = x1; j<x2+1; j++)
            {
                netGraph[j].push_back(i+1);
            }
        }
    }

    for(int col=0; col<C.width; col++) //for each column in the channel
    {
        for (int ld=0; ld<netGraph[col].size(); ld++) //for number of nets crossing that channel (local density)
        {
            for(int k=0; k<netGraph[col].size(); k++) //for number of nets crossing that channel (local density)
            {
                if(!graph[netGraph[col][ld]-1].findVal(netGraph[col][k])) //if not already in the graph
                {
                    graph[netGraph[col][ld]-1].appendNode(netGraph[col][k]); //then add it to the graph
                }
            }
        }
    }

    //reorder the HCG by in left-edge, bottom-first order
    std::vector<constraintList> orderedGraph;
    std::vector<bool> placed (C.numNets, false);

    for(size_t k=0; k<C.width; k++)
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


std::vector<constraintList> makeVCG(chan C)
{
    std::vector<constraintList> graph(C.numNets);

    for(size_t i=0; i<C.width; i++)
    {
        if(C.top[i] > 0)
        {
            if(0 < i && i < C.width-1)
            {
                if(C.bottom[i-1] > 0 && C.bottom[i-1] != C.top[i]) graph[C.top[i]-1].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0 && C.bottom[i] != C.top[i]) graph[C.top[i]-1].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0 && C.bottom[i+1] != C.top[i]) graph[C.top[i]-1].appendNode(C.bottom[i+1]);
                
                //Find VCG cycles for dogleg/////////////////
                if(C.bottom[i-1] > 0) if(graph[C.bottom[i-1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
                if(C.bottom[i] > 0) if(graph[C.bottom[i]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
                if(C.bottom[i+1] > 0) if(graph[C.bottom[i+1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
                /////////////////////////////////////////////
            }
            else if (i == 0)
            {
                if(C.bottom[i] > 0 && C.bottom[i] != C.top[i]) graph[C.top[i]-1].appendNode(C.bottom[i]);
                if(C.bottom[i+1] > 0 && C.bottom[i+1] != C.top[i]) graph[C.top[i]-1].appendNode(C.bottom[i+1]);
                
                //Find VCG cycles for dogleg/////////////////
                if(C.bottom[i] > 0) if(graph[C.bottom[i]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
                if(C.bottom[i+1] > 0) if(graph[C.bottom[i+1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
                /////////////////////////////////////////////
            }
            else
            {
                if(C.bottom[i-1] > 0 && C.bottom[i-1] != C.top[i]) graph[C.top[i]-1].appendNode(C.bottom[i-1]);
                if(C.bottom[i] > 0 && C.bottom[i] != C.top[i]) graph[C.top[i]-1].appendNode(C.bottom[i]);
                
                //Find VCG cycles for dogleg/////////////////
                if(C.bottom[i-1] > 0) if(graph[C.bottom[i-1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
                if(C.bottom[i] > 0) if(graph[C.bottom[i]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
                /////////////////////////////////////////////
            }
        }
    }

    return graph;
}


void removeChild(int netNum, std::vector<constraintList>& HCG, std::vector<constraintList>& VCG)
{
    std::vector<int> connections = HCG[netNum].returnList();

    printf("Removed  ");
    for(size_t i=0; i<connections.size(); i++)
    {
        printf("%i from VCG[%i],  ",HCG[netNum].findHead(), connections[i]);
//        printf("\nHCG: %i\n",connections[i]);
//        VCG[connections[i]-1].display();
//        printf("\n\n");
        VCG[connections[i]-1].removeAll(HCG[netNum].findHead());
    }

    printf("\n--------------------------\n");
}


int detectCycle(std::vector<constraintList>& VCG)
{
    int foundCycle = 0;
    std::vector<int> seen;
    std::vector<bool> visited (VCG.size(), false);

    return foundCycle;
}


void dogleg(int parent, int child, std::vector<net> & netlistPairs, std::vector<cell> & cellData, std::vector<constraintList>& VCG, std::vector<constraintList>& HCG, chan& channel)
{
    //take the child and break in two         //child and parent will be the actual IDs of the child and parent
    //Rules for breaking: must happen at a place that does not cause any new vertical conflicts
    //might have to detect cycle before doing any channel routing

    //must break net in a way such that its parent can now be routed ????

    //decide on a split point ???      ...arbitrary one for now (halfway point)
    net* childNet = channel.nets[child];
    net* parentNet = channel.nets[parent];
    
    int splitPoint = abs(childNet->xSrc - childNet->xDest)/2; //approximate center of the child
    
    //add the new net to the end of netlistPairs
    //netlistPairs.push_back(childNet);   need a way to push back a pointer?
    net* newChildNet = &netlistPairs[netlistPairs.size()];
    
    //update netlistPairs original net (A) with the split point as new endpoint (check if greater or lesser than remaining src point), same for new one
    childNet->xDest = splitPoint;
    //childNet.x1 = ???, childNet.x2 = ???
    
    newChildNet->xSrc = splitPoint;
    //newChildNet.x1 = ???, newChildNet.x2 = ???


    //add an implicit cell to cellData that is 1x1 at the split point and give it a lower left corner (use the default terminal offset and terminal coords cases)
    cell newCell;
    newCell.x = splitPoint;
    newCell.y = childNet->y;
    newCell.r = 0;
    newCell.nets = 2;
    newCell.cell = cellData.size();

    cellData.push_back(newCell);

    //point both nets to that new cell
    std::pair<int,int> newCellPair (1,0);
    childNet->dest = newCellPair;
    newChildNet->src = newCellPair;
    
    //once new cells have been formed and pointed to the new cell, add one to the width and numNets of channel
    channel.width ++;
    channel.numNets ++;
    //update netpointer

    //modify top and bottom boundaries as needed, including inserting or whatever...


    //recalculate VCG and HCG (but not in the full way, just update them)

    //HCG = anything with a connection to the current net should be checked against both the new partial net and the old partial net, updated accordingly
    std::vector<int> HConstraints = HCG[child].returnList();
}














