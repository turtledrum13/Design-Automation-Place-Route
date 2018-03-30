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

int main()        //use argc and argv to pass command prompt arguments to main()
{
    //initialize files
    std::ifstream fileIn ("Resources/v1.2/1");
    std::ofstream outFile ("out.txt");
    std::ofstream outCSV ("magicCSV.csv");
    std::ofstream outMag ("magFile.mag");

    //intitilaize vectors
    std::vector<numberList> cellList;
    std::vector<cell> cellData;
    std::vector<net> netlistPairs, netsGlobal, netsChannel;
    std::vector<std::vector<int> > netArray, layout, dummyLayout;
    std::vector<int> gains, fullPartition, mainPartition, boundaries;
    std::vector<std::pair <int,int> > netlist, channels;

    //initialize variables
    int numOfNets, cutset=0, numOfCells, numOfPartitions;
    double isSquare, n;

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

            //net structure which contains two pairs (cell, terminal) and (cell, terminal), net number, placed/not
            net newNet;
            newNet.src = netPair1;
            newNet.dest = netPair2;
            newNet.num = n1;
            newNet.routed = false;

            netlistPairs.push_back(newNet);
        }
    }

    n = std::sqrt(numOfCells);

    //calculate the closest sqrt equal or greater than the current number of cells
    if(n != (int) n)
    {
        n += 1;
    }

    n = int(n);

    cellData.resize(numOfCells);

    //adding net info to cellData
    for (int i=0; i<netlist.size(); i++)
    {
        cellData[netlist[i].first].nets++;
        cellData[netlist[i].first].netIndex.push_back(i);
        //printf("cell %i has %i nets\n",netlist[i].first+1,cellData[netlist[i].first].nets);
    }

    //create the list of cells and point them to their net pair
    createCellList(numOfNets, netArray, cellList, netlist, numOfCells);
    std::cout<<"\npropagated input\n";

    //perform the bisection placement partitioning in to columns
    calculateCutset(fullPartition, numOfCells+1, netArray, cellList, numOfCells, numOfNets, cutset, mainPartition, false);
    std::cout<<"\ncutset\n";

    //perform the placement for each row of the layout
    rowPlacement(netArray, cellList,  numOfCells, numOfNets, mainPartition, n);
    std::cout<<"\nrow Placement Finished";

    //Create the 2d array for the placement layout
    createArray(cellData, mainPartition, numOfCells);
    std::cout<<"\narray created\n";

    layout.resize(n*7-1, std::vector<int>(n*7-1, 0));

    std::cout<<"\nlayout resized\n";

    //create the cell layout using the x and y coordinates from the previous function
    for(int i=0; i<cellData.size(); i++)
    {
        cellData[i].r = 1;
        makeCell(cellData[i], layout);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //Routing///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    //converting dummies to -1
    for (int i=0; i<mainPartition.size(); i++)
    {
        if (mainPartition[i] > numOfCells-1) mainPartition[i] = 0;
        else mainPartition[i]++;                        //add 1 to mainPartition Cells (not base 0)
    }


    //First: Global Routing
    classifyNets(cellData, layout, netsGlobal, netsChannel, netlistPairs, boundaries, channels);

    std::cout<<"\nnets classified\n";

    global(netsGlobal, netsChannel, netlistPairs, cellData, layout, boundaries, channels, outCSV);


    //Second: Channel Routing
    channel(cellData, layout, netlistPairs, channels, boundaries);

    makeBranches(cellData, netlistPairs, layout);

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
        //printf("\n\n");
        size_t index = layout[i].size();

        for(int j=0; j<index; j++)
        {
            //printf("%i ",layout[i][j]);
            outCSV << layout[i][j] << ",";
        }
        outCSV << "\n";
    }

    //print out the magic file head and cells
    outMag << "magic\ntech scmos\ntimestamp\n<< pdiffusion >>\n";
    for (int i=0; i<layout.size(); i++)
    {
        for(int j=0; j<layout[i].size(); j++)
        {
            if(layout[i][j] == 1 || layout[i][j] == 3 || layout[i][j] == 5)
            {
                outMag<<"rect\t"<<i<<"\t"<<j<<"\t"<<i+1<<"\t"<<j+1<<"\n";
            }
        }
    }

    //print out the magic file metal1
    outMag<<"<< metal1 >>\n";
    for (int i=0; i<layout.size(); i++)
    {
        for(int j=0; j<layout[i].size(); j++)
        {
            if(layout[i][j] == 7)
            {
                outMag<<"rect\t"<<i<<"\t"<<j<<"\t"<<i+1<<"\t"<<j+1<<"\n";
            }
        }
    }

    //print out the magic file metal2
    outMag<<"<< metal2 >>\n";
    for (int i=0; i<layout.size(); i++)
    {
        for(int j=0; j<layout[i].size(); j++)
        {
            if(layout[i][j] == 8)
            {
                outMag<<"rect\t"<<i<<"\t"<<j<<"\t"<<i+1<<"\t"<<j+1<<"\n";
            }
        }
    }

}
