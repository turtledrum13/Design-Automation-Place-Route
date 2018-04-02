//
//  channelRouting.cpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.
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

        
        netlistPairs[i].dogleg = false; //initialize all dogleg flags false
    }

    for(size_t i=0; i<netID.size(); i++)
    {
        channelVec[i].numNets = netID[i];
    }


    ///////////////////////////////////////////////
    //FOR EACH OF THE CHANNELS IN LAYOUT...////////
    ///////////////////////////////////////////////

    for(size_t N=0  ; N<channelVec.size(); N++) //channelVec.size()
    {
        printf("\n\n\n");
        printf("\n\nCHANNEL %zu\n\n",N+1);

        int atRow = channels[N].first;
        int netsRemaining;
        int previousPlacement;

        //if the bottom channel has nets in it, create space to insert tracks via normal method
        if(N==0 && channelVec[N].numNets>0)
        {
            appendRows(2, layout);
            boundaries[0] += 2;
            channels[0].first += 2;
            atRow += 2;
        }

        
        ///////////////////////////////////////////////////
        //DETECT CYCLES WHILE FORMING GRAPHS///////////////
        ///////////////////////////////////////////////////

        int cycleChild = 0;
        int cycleParent = 1;                                //initialize non-zero to enter loop
        std::vector<int> cycleNets;

        while(cycleParent > 0)
        {
            HCG = makeHCG(channelVec[N], netlistPairs);     //Loop through boundary vectors to create HCG (undirected graph)
            VCG = makeVCG(channelVec[N]);                   //Loop through boundary vectors to create VCG (directed graph)

            cycleParent = detectCycle(VCG, cycleNets);      //find a cycle and return a net within it. Whole cycle stored in cycleNets

            if(cycleParent != 0)                            //get child from cycle parent for splitting
            {
                cycleChild = VCG[cycleParent-1].returnList()[0];
                dogleg(cycleParent, cycleChild, netlistPairs, cellData, VCG, HCG, channelVec[N], layout);
            }
        }


        ///////////////////////////////////////////////////
        //ROUTE USING LEFT-EDGE ALGORITHM//////////////////
        ///////////////////////////////////////////////////

        netsRemaining = channelVec[N].numNets;

        while(netsRemaining > 0)
        {
            insertRows(2, atRow, layout);
            updateBelow(2, atRow, cellData, netlistPairs, boundaries, channels);
            previousPlacement = 0;

            for(size_t i=0; i<HCG.size(); i++)
            {
                int ID = HCG[i].findHead();
                net* currentNet = channelVec[N].nets[ID-1];

                if(!currentNet->routed && !HCG[i].findVal(previousPlacement) && VCG[ID-1].isEmpty(ID))  //if net selected from HCG is routable
                {
                    makeTrunk(currentNet, atRow, layout, cellData, netlistPairs);   //draw horizontal component on layout
                    removeChild(i, HCG, VCG);                                       //remove net as a child from any parents in the VCG
                    if(currentNet->span != 0) previousPlacement = ID;               //keep track of the most recently placed non-zero-span net
                    netsRemaining--;                                                //decrement counter for while loop
                }
            }   //Return to for loop top to find next net
        }   //Return to while loop top to add next track
    }   //Return to for loop top to route next channel
}   //Finished with channel routing



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


std::vector<constraintList> makeVCG(chan C)
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
        }
    }


    //operate on vertical dogleg segments
    for(int i=0; i<C.numNets; i++)       //scanning through all nets looking for dogleg
    {
        if(C.nets[i]->dogleg)               //when we come across a dogleg
        {
            net* dogleg = C.nets[i];        //create pointer to the pointer which accesses the net in netlistPairs
            int id = i+1;                   //ID-number of the dogleg net
            int x1 = dogleg->x1;            //left terminal of dogleg net
            int x2 = dogleg->x2;            //right terminal of dogleg net

            int xSplit = x1;                //initial guess for floating terminal of dogleg net
            if(C.top[x2] != id && C.bottom[x2] != id)
            {
                xSplit = x2;                //switch it to x2 if "id" can't be found on either boundary @ x2
            }
            
            //add dogleg net to the VCG of the upper terminal --> for the x value that is not already there --> && !graph[C.top[xSplit+1]-1].findVal(i)
            if(C.top[xSplit-1] > 0)     graph[C.top[xSplit-1]-1].appendNode(id);
            if(C.top[xSplit] > 0)       graph[C.top[xSplit]-1].appendNode(id);
            if(C.top[xSplit+1] > 0)     graph[C.top[xSplit+1]-1].appendNode(id);

            //add the lower terminals to the VCG of the dogleg ne
            if(C.bottom[xSplit-1] > 0 && C.bottom[xSplit-1] != id)  graph[id-1].appendNode(C.bottom[xSplit-1]);
            if(C.bottom[xSplit] > 0 && C.bottom[xSplit] != id)      graph[id-1].appendNode(C.bottom[xSplit]);
            if(C.bottom[xSplit+1] > 0 && C.bottom[xSplit+1] != id)  graph[id-1].appendNode(C.bottom[xSplit+1]);
        }
    }

    return graph;
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


void removeChild(int netNum, std::vector<constraintList>& HCG, std::vector<constraintList>& VCG)
{
    std::vector<int> connections = HCG[netNum].returnList();

    for(size_t i=0; i<connections.size(); i++)
    {
        VCG[connections[i]-1].removeAll(HCG[netNum].findHead());
    }
}


int detectCycle(std::vector<constraintList> &VCG, std::vector<int>& cycleList)
{
    bool detected = false;
    int foundCycle = 0;
    std::vector<bool> visited;
    for(int i=0; i<VCG.size(); i++)
    {
        visited.resize(0);
        visited.resize(VCG.size(), false);
        detected = VCG[i].cycleDetection(i+1, VCG, visited, cycleList);
        if (detected)
        {
            foundCycle = i+1;
            break;
        }
    }

    return foundCycle;
}


void dogleg(int parent, int child, std::vector<net>& netlistPairs, std::vector<cell>& cellData, std::vector<constraintList>& VCG, std::vector<constraintList>& HCG, chan& channel, std::vector<std::vector<int> >& layout)
{
    net* childNet = channel.nets[child-1];
    net* parentNet = channel.nets[parent-1];
    

    bool goRight = false;
    int splitPoint = childNet->x1;

    if(abs(childNet->x1-parentNet->x1) < 2 || abs(childNet->x1-parentNet->x2) < 2)
    {
        splitPoint = childNet->x1 + 1;
        goRight = true;
    }
    else if(abs(childNet->x2-parentNet->x1) < 2 || abs(childNet->x2-parentNet->x2) < 2)
    {
        splitPoint = childNet->x2 + 1;
        goRight = false;
    }

    
    int top1, top2, top3, bot1, bot2, bot3;
    int prevSplitPoint = splitPoint;
    bool goLeft = false;
    while (true)
    {
        top1 = channel.top[splitPoint-1];
        top2 = channel.top[splitPoint];
        top3 = channel.top[splitPoint+1];
        bot1 = channel.bottom[splitPoint-1];
        bot2 = channel.bottom[splitPoint];
        bot3 = channel.bottom[splitPoint+1];

        
        if(top1 > 0 || top2 > 0 || top3 > 0 || bot1 > 0 || bot2 > 0 || bot3 > 0)
        {
            if(goLeft) splitPoint += 1;
            else splitPoint -= 1;
        }

        if(splitPoint-1 == 0 && !goLeft)
        {
            goLeft=true;
        }

        if(splitPoint+1 == channel.top.size()-1)
        {
            appendCols(3, layout);
            channel.top.push_back(0);
            channel.top.push_back(0);
            channel.top.push_back(0);
            channel.bottom.push_back(0);
            channel.bottom.push_back(0);
            channel.bottom.push_back(0);
            splitPoint += 2;
            break;
        }

        if(prevSplitPoint == splitPoint) break;
        prevSplitPoint = splitPoint;
    }

    printf("\nparent(%i): %i<-->%i\n",parent, parentNet->x1, parentNet->x2);
    printf("child(%i): %i<-->%i\n",child, childNet->x1, childNet->x2);
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
    int cellNum = cellData.size();
    cell cellA = {.x = splitPoint, .r = 0, .nets = 2, .cell = cellNum};

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

    
    //once new cells have been formed and pointed to the new cells, update channel structure
    channel.numNets ++;                                             //increase number of nets by one
    channel.nets.push_back(&netlistPairs[netlistPairs.size()-1]);     //create pointer for the new net in the vector of pointers


    printf("new child(%zu): %i<-->%i\n",channel.nets.size(),newChildNet->x1, newChildNet->x2);


    //modify top and bottom boundaries as needed, including inserting or whatever...
    if(channel.top[newChildNet->xSrc] == child)
        channel.top[newChildNet->xSrc] = channel.numNets;

    else if(channel.bottom[newChildNet->xSrc] == child)
        channel.bottom[newChildNet->xSrc] = channel.numNets;
}













