
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
                boundaries[boundaries.size()-1] += 2;
                channels[channels.size()-1].first += 2;
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
            printf("\nUnrouted: ");
            for(size_t i=0; i<HCG.size(); i++)
            {
                if(!netlistPairs[channelVec[N].netPointer[i]].routed) printf("%i, ",HCG[i].findHead());
            }
            printf("\n");
            //////////PRINT UNROUTED NETS///////////


            if(!cycle)  //if not caught in a cycle, add a new track
            {
                addTrack(2, atRow, cellData, netlistPairs, layout, boundaries, channels);
                previousPlacement = 0;
            }
            else        //caught in a cycle and need to dogleg
            {
                for(size_t i=0; i<HCG.size(); i++)
                {
                    net& currentNet = netlistPairs[channelVec[N].netPointer[i]];
                    if(!currentNet.routed)  //for the first unrouted net
                    {
                        //////////TEMP DOGLEG WORKAROUND///////////
                        currentNet.routed = true;
                        netsRemaining --;
                        //////////TEMP DOGLEG WORKAROUND///////////
                    }
                }
            }


            cycle = true; //assume cycle is true before scanning HCG (which could prove this assumption false)

            for(size_t i=0; i<HCG.size(); i++)
            {
                net& currentNet = netlistPairs[channelVec[N].netPointer[i]];
                int ID = HCG[i].findHead();

                if(!currentNet.routed && !HCG[i].findVal(previousPlacement) && VCG[ID-1].isEmpty(ID))
                {
                    makeTrunk(currentNet, atRow, layout);
                    removeChild(i, HCG, VCG);
                    previousPlacement = ID;
                    netsRemaining--;
                    cycle = false;
                }
            }
        }


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


std::vector<constraintList> makeHCG(chan C, std::vector<net> & netlistPairs)
{
    std::vector<std::vector<int> > netGraph(C.width);
    std::vector<constraintList> graph(C.numNets);
    int x1, x2;

    for(int i=0; i<C.numNets; i++)
    {
        graph[i].appendNode(i+1); //create a vector of list heads in number order

        x1 = netlistPairs[C.netPointer[i]].x1-1; //-1 to block adjacent nets
        x2 = netlistPairs[C.netPointer[i]].x2+1; //+1 to block adjacent nets

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
    std::vector<constraintList> orderedGraph;
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


std::vector<constraintList> makeVCG(chan C)
{
    std::vector<constraintList> graph(C.numNets);

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

void removeChild(int netNum, std::vector<constraintList>& HCG, std::vector<constraintList>& VCG)
{
    std::vector<int> connections = HCG[netNum].returnList();

    printf("Removed  ");
    for(size_t i=0; i<connections.size(); i++)
    {
        printf("%i from VCG[%i],  ",HCG[netNum].findHead(), connections[i]);
        VCG[connections[i]-1].removeAll(HCG[netNum].findHead());
//        printf("\n");
//        VCG[connections[i]-1].display();
//        printf("\n\n");
    }

    printf("\n");
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
    net& childNet = netlistPairs[channel.netPointer[child]];
    net& parentNet = netlistPairs[channel.netPointer[parent]];

    int splitPoint = abs(childNet.xSrc-childNet.xDest)/2; //approximate center of the child

    //add the new net to the end of netlistPairs
    netlistPairs.push_back(childNet);
    net& newChildNet = netlistPairs[netlistPairs.size()];

    //update netlistPairs original net (A) with the split point as new endpoint (check if greater or lesser than remaining src point), same for new one
    childNet.xDest = splitPoint;
    //childNet.x1 = ???, childNet.x2 = ???

    newChildNet.xSrc = splitPoint;
    //newChildNet.x1 = ???, newChildNet.x2 = ???


    //add an implicit cell to cellData that is 1x1 at the split point and give it a lower left corner (use the default terminal offset and terminal coords cases)
    cell newCell;
    newCell.x = splitPoint;
    newCell.y = childNet.y;
    newCell.r = 0;
    newCell.nets = 2;
    newCell.cell = cellData.size();

    cellData.push_back(newCell);

    //point both nets to that new cell
    std::pair<int,int> newCellPair (1,0);
    childNet.dest = newCellPair;
    newChildNet.src = newCellPair;

    //once new cells have been formed and pointed to the new cell, add one to the width and numNets of channel
    channel.width ++;
    channel.numNets ++;
    //update netpointer

    //modify top and bottom boundaries as needed, including inserting or whatever...


    //recalculate VCG and HCG (but not in the full way, just update them)

    //HCG = anything with a connection to the current net should be checked against both the new partial net and the old partial net, updated accordingly
    std::vector<int> HConstraints = HCG[channel.netPointer[child]].returnList();
}














