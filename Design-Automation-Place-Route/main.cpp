#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cmath>

#include "lin.h"
#include "celllist.h"
#include "calculatecutset.h"
#include "globalRouting.hpp"
#include "channelRouting.hpp"
#include "layoutOperations.hpp"
#include "structures.h"
#include "classifyNets.hpp"




int main()	    //use argc and argv to pass command prompt arguments to main()
{
    //initialize files
    //std::ifstream fileIn ("Resources/v1.2/1");
    std::ifstream fileIn ("1");
    std::ofstream outFile ("out.txt");
    std::ofstream outCSV ("magicCSV.csv");

    //intitilaize vectors
    std::vector<numberList> cellList;
    std::vector<cell> cellData;
    std::vector<std::vector<int> > netArray, layout;
    std::vector<int> gains, fullPartition, mainPartition, boundaries;
    std::vector<std::pair <int,int> > netlist;

    //initialize variables
    int numOfNets, cutset=0, numOfCells, numOfPartitions;
    double m;

    fileIn >> numOfCells;
    fileIn >> numOfNets;

    //add the numbers to the vector while the file is open
    if (fileIn)
    {
        int n1, n2, n3, n4, n5;
        while (fileIn >> n1 and fileIn >> n2 and fileIn >> n3 and fileIn >> n4 and fileIn >> n5)
        {
            std::pair <int,int> netPair1 (n2-1, n3);
            std::pair <int,int> netPair2 (n4-1, n5);

            netlist.push_back(netPair1);
            netlist.push_back(netPair2);
        }
    }

    //if an odd number of cells is given, add one to make it even

    cellData.resize(numOfCells);                          //vector that holds "cell" structures

    //calculate the number of partitions to be found
    m = log(numOfCells)/log(2);

    if (m!=(int) m)
    {
        m+=1;
    }
    numOfPartitions = m;

    //split the cells into 2 partitions of equal size.

    //adding net info to cellData
    for (int i=0; i<netlist.size(); i++)
    {
        cellData[netlist[i].first].nets++;
        printf("cell %i has %i nets\n",netlist[i].first+1,cellData[netlist[i].first].nets);
    }

    //create the list of cells and point them to their net pair
    createCellList(numOfNets, netArray, cellList, netlist, numOfCells);

    std::cout<<"\npropagated input\n";

    //calculate the cutset
    calculateCutset(fullPartition, numOfCells, netArray, cellList, numOfCells, numOfNets, cutset, mainPartition);




    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //Routing///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    //converting dummies to -1
    for (int i=0; i<mainPartition.size(); i++)
    {
        if (mainPartition[i] > numOfCells-1) mainPartition[i] = -1;
    }

    //initial layout generation
    //layout.resize(6, std::vector<int>(7*mainPartition.size()-1));   //sizing the empty layout for single row placement (will want to make this 2:1 placement eventually)
    layout.resize(1, std::vector<int>(1));
    addRows(5, layout);


    int xPos = 1;

    for (int i=0; i<mainPartition.size(); i++)
    {
        int cellNum = mainPartition[i];     //grab cell number from mainPartition vector (base 0)

        cellData[cellNum].x = xPos;         //x-coord of LL corner
        cellData[cellNum].y = 5;            //y-coord of LL corner
        if (cellNum < numOfCells) cellData[cellNum].r = 1;            //all cells unrotated (rotation = 1)

        //cellData[cellNum].nets = 0;         //assumed 0 nets during initialization
        cellData[cellNum].cell = cellNum+1; //cell's number


        xPos += 7;                          //Placing cells side by side along x axis + one extra space in between

        addCols(7, layout);
        makeCell(cellData[cellNum], layout);//create the cell in the 2D "layout" vector
    }


    //First: Global Routing
    std::vector<int> netsGlobal, netsChannel;
    classifyNets(cellData, layout, netsGlobal, netsChannel);

    global();

    //Second: Channel Routing
    channel();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////




    //output the cutset and partitions to output file
    outFile<<cutset<<"\n";
    std::cout<<"\ncutset: "<<cutset<<"\n";

    for(int i=0; i<mainPartition.size()/2; i++)
    {
        outFile<<mainPartition[i]+1<<"\t";
        std::cout<<mainPartition[i]<<"\t";
    }

    outFile<<"\n";
    std::cout<<"\n";

    for(int i=0; i<mainPartition.size()/2; i++)
    {
        outFile<<mainPartition[i+mainPartition.size()/2]+1<<"\t";
        std::cout<<mainPartition[i+mainPartition.size()/2]<<"\t";
    }

    //print CSV -- using to paste into spreadsheet for debugging
    for (int i=0; i<layout.size(); i++)
    {
        printf("\n\n");
        int index = layout[i].size();

        for(int j=0; j<index; j++)
        {
            printf("%i ",layout[i][j]);
            outCSV << layout[i][j] << ",";
        }
        outCSV << "\n";
    }
}

