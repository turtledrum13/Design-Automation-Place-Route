
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
    std::vector<constraintList> HCG;
    std::vector<constraintList> VCG;
    
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
        
        
        netlistPairs[i].dogleg = false;
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

    for(size_t N=4  ; N<5; N++) //channelVec.size()
    {
        printf("\n\n\n");
        printf("\n\nCHANNEL %zu\n\n",N+1);

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

        ///////////////////////////////////////////////////
        //DETECT CYCLES WHILE FORMING GRAPHS///////////////
        ///////////////////////////////////////////////////
        
        int cycleChild = NULL;
        int cycleParent = 1;                                //initialize non-zero to enter loop
        bool cycleBroken = false;
        
        while(cycleParent > 0)
        {
            HCG = makeHCG(channelVec[N], netlistPairs);     //Loop through boundary vectors to create HCG (undirected graph)
            VCG = makeVCG(channelVec[N]);                   //Loop through boundary vectors to create VCG (directed graph)
            
            //printing VCG///////////////////
            std::cout << "\n\n\nVertical Constraint Graph:\n\n";
            for(size_t i=0; i<VCG.size(); i++)
            {
                std::cout << i+1 <<":\t";
                VCG[i].display();
                std::cout << "\n\n";
            }
            std::cout << "\n\n\n\n";
            
            
            cycleParent = detectCycle(VCG);                 //find a cycle
            std::vector<int> cycle_list;
            if(cycleParent!= 0) cycle_list = cycleList(VCG, cycleParent);
            
            if(cycleParent != 0)                            //get child from cycle parent for splitting
            {
                cycleChild = VCG[cycleParent-1].returnList()[0];
                while(cycleChild != NULL)
                {
                    cycleBroken = dogleg(cycleParent, cycleChild, netlistPairs, cellData, VCG, HCG, channelVec[N]);
                    
                    if(cycleBroken)
                    {
                        break;
                    }
                    else
                    {
                        cycleParent = cycleChild;
                        cycleChild = VCG[cycleParent-1].returnList()[0];
                    }
                }
            }
        }

        
        ///////////////////////////////////////////////////
        //ROUTE USING LEFT-EDGE ALGORITHM//////////////////
        ///////////////////////////////////////////////////
        
        while(netsRemaining > 0)
        {
            //////////PRINT UNROUTED NETS///////////
            printf("\n\n%i Unrouted: ", netsRemaining);
            for(size_t i=0; i<HCG.size(); i++)
            {
                int ind = HCG[i].findHead()-1;

                if(!channelVec[N].nets[ind]->routed)
                {
                    printf("%i(#%i)  ",HCG[i].findHead(),channelVec[N].nets[ind]->num);
                    //printf("NET#%i    %i-[%i->%i]\t",channelVec[N].nets[ind]->num, HCG[i].findHead(), channelVec[N].nets[ind]->x1, channelVec[N].nets[ind]->x2);
                    //printf("src cell: %i @ (%i,%i),\t", channelVec[N].nets[ind]->src.first, cellData[channelVec[N].nets[ind]->src.first].x, cellData[channelVec[N].nets[ind]->src.first].y);
                    //printf("dest cell: %i @ (%i,%i)\n", channelVec[N].nets[ind]->dest.first, cellData[channelVec[N].nets[ind]->dest.first].x, cellData[channelVec[N].nets[ind]->dest.first].y);

                }
            }
            printf("\n\n");
            //////////PRINT UNROUTED NETS///////////


            if(!cycle)  //if not caught in a cycle, add a new track
            {
                addTrack(2, atRow, cellData, netlistPairs, layout, boundaries, channels);
                numTracks++;
                printf("Track: %i\n",numTracks);
                printf("\n-------------------------------------------------------\n");
                previousPlacement = 0;
            }
            else        //caught in a cycle and need to dogleg
            {
                printf("\n\nCYCLE  --  %i nets remaining\n", netsRemaining);
                
                for(size_t i=0; i<HCG.size(); i++)
                {
                    net* cycleNet = channelVec[N].nets[i];
                    printf("\t\tHCG SIZE %zu\n\n\n",HCG.size());
                    if(!cycleNet->routed)  //for the first unrouted net
                    {
                        //////////TEMP DOGLEG WORKAROUND///////////
                        printf("skipping net %i\n\n\n",cycleNet->num);
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
                    makeTrunk(currentNet, atRow, layout, cellData, netlistPairs);                   //draw horizontal component on layout
                    removeChild(i, HCG, VCG);                               //remove net as a child from any parents in the VCG
                    printf("%i  ",HCG[i].findHead());//channelVec[N].nets[HCG[i].findHead()-1]->num);
                    if(currentNet->span != 0) previousPlacement = ID;       //keep track of the most recently placed non-zero-span net
                    netsRemaining--;                                        //decrement counter for while loop
                    cycle = false;                                          //reset cycle flag since a net was placed
                }
            }
                       
            printf("\n-------------------------------------------------------\n\n");
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

    for(int i=0; i<C.numNets; i++)              //for each of the nets in the channel
    {
        graph[i].appendNode(i+1);               //create put a list head into the vector in ID-number order (not left-edge)

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


std::vector<constraintList> makeVCG(chan C)//, std::vector<constraintList>& HCG)
{
    std::vector<constraintList> graph(C.numNets);

    //operate on normal boundary lines
    for(size_t i=1; i<C.width-1; i++)  //Don't need to check first or last as they will always be zero
    {
        if(C.top[i] > 0)
        {
            if(C.bottom[i-1] > 0 && C.bottom[i-1] != C.top[i])  graph[C.top[i]-1].appendNode(C.bottom[i-1]);
            if(C.bottom[i] > 0 && C.bottom[i] != C.top[i])      graph[C.top[i]-1].appendNode(C.bottom[i]);
            if(C.bottom[i+1] > 0 && C.bottom[i+1] != C.top[i])  graph[C.top[i]-1].appendNode(C.bottom[i+1]);
            
            //Find VCG cycles (length-2 only)////////////
            if(C.bottom[i-1] > 0) if(graph[C.bottom[i-1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
            if(C.bottom[i] > 0) if(graph[C.bottom[i]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
            if(C.bottom[i+1] > 0) if(graph[C.bottom[i+1]-1].findVal(C.top[i])) printf("\n\n\t\t\tCYCLE DETECTED @ %i and %i\n\n", C.top[i], C.bottom[i]);
            /////////////////////////////////////////////
        }
    }
    
    
    //operate on dogleg nets
    for(int i=0; i<C.numNets; i++)       //scanning through all nets looking for dogleg
    {
        printf("net %i/%i  -  dogleg=%i\n",i+1,C.numNets, C.nets[i]->dogleg);
        if(C.nets[i]->dogleg)               //when we come across a dogleg
        {
            //removeChild(i, HCG, graph);     //remove the net from all VCG lists. Going to add again -- only do if the whole net was added previously
            
            net* dogleg = C.nets[i];        //create pointer to the pointer which accesses the net in netlistPairs
            int id = i+1;                   //ID-number of the dogleg net
            int x1 = dogleg->x1;            //left terminal of dogleg net
            int x2 = dogleg->x2;            //right terminal of dogleg net
            
            int xSplit = x1;                //initial guess for floating terminal of dogleg net
            if(C.top[x2] != id && C.bottom[x2] != id)
            {
                xSplit = x2;                //switch it to x2 if "id" can't be found on either boundary @ x2
            }
            printf("\n-----------------------------");
            printf("\nnet %i floating at %i\n\n",id, xSplit);
            
            printf("Top:   %i  %i  %i\n\n",C.top[xSplit-1],C.top[xSplit],C.top[xSplit+1]);
            printf("Btm:   %i  %i  %i\n\n\n",C.bottom[xSplit-1],C.bottom[xSplit],C.bottom[xSplit+1]);

            
            //add dogleg net to the VCG of the upper terminal --> for the x value that is not already there --> && !graph[C.top[xSplit+1]-1].findVal(i)
            printf("\n@%i  %i: ",xSplit-1,C.top[xSplit-1]); if(C.top[xSplit-1] > 0) graph[C.top[xSplit-1]-1].display();
            printf("\n@%i  %i: ",xSplit,C.top[xSplit]);     if(C.top[xSplit] > 0)   graph[C.top[xSplit]-1].display();
            printf("\n@%i  %i: ",xSplit+1,C.top[xSplit+1]); if(C.top[xSplit+1] > 0) graph[C.top[xSplit+1]-1].display();
            
            if(C.top[xSplit-1] > 0 && C.top[xSplit-1] != id)    {graph[C.top[xSplit-1]-1].appendNode(id); printf("\nadded left");}
            if(C.top[xSplit] > 0 && C.top[xSplit] != id)        {graph[C.top[xSplit]-1].appendNode(id); printf("\nadded middle");}
            if(C.top[xSplit+1] > 0 && C.top[xSplit+1] != id)    {graph[C.top[xSplit+1]-1].appendNode(id); printf("\nadded right");}
            
            printf("\n\nTop after: \n");
            printf("\n@%i  %i: ",xSplit-1,C.top[xSplit-1]); if(C.top[xSplit-1] > 0) graph[C.top[xSplit-1]-1].display();
            printf("\n@%i  %i: ",xSplit,C.top[xSplit]);     if(C.top[xSplit] > 0)   graph[C.top[xSplit]-1].display();
            printf("\n@%i  %i: ",xSplit+1,C.top[xSplit+1]); if(C.top[xSplit+1] > 0) graph[C.top[xSplit+1]-1].display();
            
            //add the lower terminals to the VCG of the dogleg net
            
            printf("\n\nDogleg VCG:\n%i: ",id);
            graph[id-1].display();
            
            if(C.bottom[xSplit-1] > 0 && C.bottom[xSplit-1] != id)  {graph[id-1].appendNode(C.bottom[xSplit-1]);printf("\nadded %i left",C.bottom[xSplit-1]);}
            if(C.bottom[xSplit] > 0 && C.bottom[xSplit] != id)      {graph[id-1].appendNode(C.bottom[xSplit]);  printf("\nadded %i middle",C.bottom[xSplit]);}
            if(C.bottom[xSplit+1] > 0 && C.bottom[xSplit+1] != id)  {graph[id-1].appendNode(C.bottom[xSplit+1]);printf("\nadded %i right",C.bottom[xSplit+1]);}
            
            printf("\n\n-----------------------------");
            printf("\ndone with if statement on %i\n\n",i+1);
        }
    }
    
    return graph;
}


void removeChild(int netNum, std::vector<constraintList>& HCG, std::vector<constraintList>& VCG)
{
    std::vector<int> connections = HCG[netNum].returnList();

    //printf("Removed  ");
    for(size_t i=0; i<connections.size(); i++)
    {
//        printf("%i from VCG[%i],  ",HCG[netNum].findHead(), connections[i]);
//        printf("\nHCG: %i\n",connections[i]);
//        VCG[connections[i]-1].display();
//        printf("\n\n");
        VCG[connections[i]-1].removeAll(HCG[netNum].findHead());
    }

//    printf("\n--------------------------\n");
}


int detectCycle(std::vector<constraintList> &VCG)
{
    bool detected = false;
    int foundCycle = 0;
    std::vector<bool> visited;

    for(int i=0; i<VCG.size(); i++)
    {
        visited.resize(0);
        visited.resize(VCG.size(), false);
        detected = VCG[i].cycleDetection(i+1, VCG, visited);
        if (detected)
        {
            foundCycle = i+1;
            break;
        }
    }

    return foundCycle;
}


bool dogleg(int parent, int child, std::vector<net> & netlistPairs, std::vector<cell> & cellData, std::vector<constraintList>& VCG, std::vector<constraintList>& HCG, chan& channel)
{
    //take the child and break in two         //child and parent will be the actual IDs of the child and parent
    //Rules for breaking: must happen at a place that does not cause any new vertical conflicts
    //might have to detect cycle before doing any channel routing

    //must break net in a way such that its parent can now be routed ????

    
    
    //decide on a split point ???      ...arbitrary one for now (halfway point)
    net* childNet = channel.nets[child-1];
    net* parentNet = channel.nets[parent-1];
    
    printf("\nparent(%i): %i<-->%i\n",parent, parentNet->x1, parentNet->x2);
    printf("child(%i): %i<-->%i\n",child, childNet->x1, childNet->x2);

    
    bool goRight = false;
    int splitPoint = childNet->x1 + 1;//abs(childNet->xSrc - childNet->xDest)/2; //approximate center of the child
    
    if(abs(childNet->x1-parentNet->x1) < 2 || abs(childNet->x1-parentNet->x2) < 2)
    {
        splitPoint = childNet->x1 + 1;//abs(childNet->xSrc - childNet->xDest)/2; //approximate center of the child
        goRight = true;
    }
    else if(abs(childNet->x2-parentNet->x1) < 2 || abs(childNet->x2-parentNet->x2) < 2)
    {
        splitPoint = childNet->x2 + 1;//abs(childNet->xSrc - childNet->xDest)/2; //approximate center of the child
        goRight = false;
    }
    
    
    while (true)
    {
        if(abs(parentNet->x1-splitPoint) > 1 && abs(parentNet->x2-splitPoint) > 1) break;
        else
        {
            if(goRight) splitPoint += 1;
            else splitPoint -= 1;
        }
    }

    
    printf("\n<<Splitting child at %i>>\n\n",splitPoint);
    
    
    //copy the new net to the end of netlistPairs
    netlistPairs.push_back(*childNet);          //channel.nets[child]);   //need a way to push back a pointer?
    net* newChildNet = &netlistPairs[netlistPairs.size()-1];
    
    
    //update netlistPairs original net (A) with the split point as new endpoint (check if greater or lesser than remaining src point), same for new one
    newChildNet->xSrc = childNet->xDest;
    
    childNet->xDest = splitPoint;
    childNet->x1 = childNet->xSrc; childNet->x2 = childNet->xDest;      //initial guess for child's x1 and x2
    if(childNet->xSrc > childNet->xDest)
    {
        childNet->x1 = childNet->xDest; childNet->x2 = childNet->xSrc;  //flip x1 and x2 if guess was incorrect
    }
    
    newChildNet->xDest = splitPoint;
    newChildNet->x1 = newChildNet->xSrc; newChildNet->x2 = newChildNet->xDest;      //initial guess for new child's x1 and x2
    if(newChildNet->xSrc > newChildNet->xDest)
    {
        newChildNet->x1 = newChildNet->xDest; newChildNet->x2 = newChildNet->xSrc;  //flip x1 and x2 if guess was incorrect
    }
    

    //add two implicit cella to cellData that are 1x1 at the split point and give them lower left corners (use the default terminal offset and terminal coords cases)
    cell cellA;
    cellA.x = splitPoint;
    cellA.r = 0;
    cellA.nets = 2;
    cellA.cell = cellData.size();
    
    cell cellB = cellA; //newCell_Dest will be routed BEFORE parent. Its y-value is unknown for now, but metal 2 will be taken care of by the other
    cellB.cell++;

    cellData.push_back(cellA);
    cellData.push_back(cellB);


    //point new nets to new cells
    std::pair<int,int> cellAPair (cellA.cell,0);
    std::pair<int,int> cellBPair (cellB.cell,0);

    newChildNet->src = childNet->dest;
    childNet->dest = cellAPair;
    newChildNet->dest = cellBPair;
    childNet->dogleg = true;
    newChildNet->dogleg = true;
    
    //printf("netlistPairs[%i].dogleg = %i\n\n",netlistPairs.size()-1, netlistPairs[netlistPairs.size()-1].dogleg);
    
    //once new cells have been formed and pointed to the new cells, update channel structure
    channel.numNets ++;                                             //increase number of nets by one
    channel.nets.push_back(&netlistPairs[netlistPairs.size()-1]);     //create pointer for the new net in the vector of pointers

    
    printf("new child(%zu): %i<-->%i\n",channel.nets.size(),newChildNet->x1, newChildNet->x2);

    
    //modify top and bottom boundaries as needed, including inserting or whatever...
    if(channel.top[newChildNet->xSrc] == child)
    {
        channel.top[newChildNet->xSrc] = channel.numNets;
    }
    else if(channel.bottom[newChildNet->xSrc] == child)
    {
        channel.bottom[newChildNet->xSrc] = channel.numNets;
    }
    
    
    //printing out channel boundaries for debugging
    printf("\n");
    for(int j=childNet->x1; j<newChildNet->x2+1; j++)
    {
        printf("%i ",channel.top[j]);
    }
    printf("\n\n");
    for(int j=childNet->x1; j<newChildNet->x2+1; j++)
    {
        printf("%i ",channel.bottom[j]);
    }
    printf("\n\n\n");
    
    
    printf("Num of Cells: %zu\n\n",cellData.size());
    

    return true;
}

std::vector<int> cycleList(std::vector<constraintList> & VCG, int parent)
{
    std::vector<int> list;
    list.push_back(parent);
    
    while(true)
    {
            list.push_back(VCG[list.back()-1].returnList()[0]);
        
            if(VCG[list.back()-1].returnList()[0] == parent) break;

    }
    
    for(int i=0; i<list.size(); i++)
    {
        printf("%i ", list[i]);
    }
    
    return list;
}














