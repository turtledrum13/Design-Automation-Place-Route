#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <cctype>

#include "lin.h"
#include "celllist.h"
#include "calculatecutset.h"
#include "globalRouting.hpp"
#include "channelRouting.hpp"
#include "layoutOperations.hpp"
#include "structures.h"
#include "classifyNets.hpp"

int main()//int argc,char *argv[])
{
    //Extract bench number from user input////////////////////////////////
    
//    std::string inputFile;
//    std::string benchNum;
//
//    for(int i = 1; i < argc; i++) inputFile += argv[i];
//
//    benchNum = inputFile[0];
//    if(inputFile.size() >= 2)
//    {
//        if(isdigit(inputFile[1])) benchNum += inputFile[1];
//    }
//    else
//    {
//        benchNum = "X";
//    }
//
//
//    std::ifstream fileIn (argv[1]);
//    std::ofstream outMag (argv[2]);


    //initiate program timer
    std::clock_t startTime;
    startTime = std::clock();

    //initialize files
    std::string benchNum = "10";
    std::ifstream fileIn ("Resources/v1.2/"+benchNum);
    std::ofstream outFile ("output"+benchNum+".csv");

    std::ofstream outCSV ("magicCSV.csv");
    std::ofstream outMag ("magFile.mag");

    //intitilaize vectors
    std::vector<numberList> cellList;
    std::vector<cell> cellData;
    std::vector<net> netlistPairs;
    std::vector<std::vector<int> > netArray, layout, dummyLayout;
    std::vector<int> gains, fullPartition, mainPartition, boundaries;
    std::vector<std::pair <int,int> > netlist, channels;

    //initialize variables
    int numOfNets, cutset=0, numOfCells, layoutCells;
    double numOfRows, sqrtCells;

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


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //PLACEMENT/////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////


    //calculate the number of rows and cells per row to ensure the final layout is close to a square
    sqrtCells = std::sqrt(numOfCells);
    if(sqrtCells != (int) sqrtCells)
    {
        sqrtCells += 1;
    }
    sqrtCells = int(sqrtCells);

    numOfRows = sqrtCells/std::sqrt(2);
    if(numOfRows != (int) numOfRows)
    {
        numOfRows += 1;
    }

    //the number of rows in the layout
    numOfRows = int(numOfRows);

    //the number of cells per row in the layout
    layoutCells = numOfRows*2;

    cellData.resize(numOfCells);

    //adding net info to cellData
    for (int i=0; i<netlist.size(); i++)
    {
        cellData[netlist[i].first].nets++;
        cellData[netlist[i].first].netIndex.push_back(i);
    }

    //create the list of cells and point them to their net pair
    createCellList(numOfNets, netArray, cellList, netlist, numOfCells);


    //perform the bisection placement partitioning in to columns
    calculateCutset(fullPartition, numOfCells+1, netArray, cellList, numOfCells, numOfNets, cutset, mainPartition, false);


    //perform the placement for each row of the layout
    rowPlacement(netArray, cellList,  numOfCells, numOfNets, mainPartition, numOfRows);


    //Create the 2d array for the placement layout
    createArray(cellData, mainPartition, numOfCells);
    layout.resize(numOfRows*7-1, std::vector<int>(layoutCells*7-1, 0));

    //create the cell layout using the x and y coordinates from the previous function
    for(int i=0; i<cellData.size(); i++)
    {
        cellData[i].r = 1;
        makeCell(cellData[i], layout);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //ROUTING///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    //converting dummy cells to 0
    for (int i=0; i<mainPartition.size(); i++)
    {
        if (mainPartition[i] > numOfCells-1) mainPartition[i] = 0;
        else mainPartition[i]++;  //add 1 to mainPartition Cells (not base 0)
    }

    //First: Global Routing
    classifyNets(cellData, layout, netlistPairs, boundaries, channels);

    global(netlistPairs, cellData, layout, boundaries, channels);


    //Second: Channel Routing
    channel(cellData, layout, netlistPairs, channels, boundaries);

    makeBranches(cellData, netlistPairs, layout);


    ////////////////////////////////////////////////////////////
    ////OUTPUT FILE PRINTING////////////////////////////////////
    ////////////////////////////////////////////////////////////

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


    ////////////////////////////////////////////////////////////////////////////////////////

    int cellArea = 0;
    int wireArea = 0;
    int numVias = 0;

    //print out the magic file head and cells
    outMag << "magic\ntech scmos\ntimestamp\n<< pdiffusion >>\n";
    for (int i=0; i<layout.size(); i++)
    {
        for(int j=0; j<layout[i].size(); j++)
        {
            if(layout[i][j] == 1 || layout[i][j] == 3 || layout[i][j] == 5)
            {
                outMag<<"rect\t"<<j<<"\t"<<i<<"\t"<<j+1<<"\t"<<i+1<<"\n";
                cellArea++;
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
                outMag<<"rect\t"<<j<<"\t"<<i<<"\t"<<j+1<<"\t"<<i+1<<"\n";
                wireArea++;
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
                outMag<<"rect\t"<<j<<"\t"<<i<<"\t"<<j+1<<"\t"<<i+1<<"\n";
                wireArea++;
            }
        }
    }

    //print out the magic file vias
    outMag<<"<< via >>\n";
    for (int i=0; i<layout.size(); i++)
    {
        for(int j=0; j<layout[i].size(); j++)
        {
            if(layout[i][j] == 9)
            {
                outMag<<"rect\t"<<j<<"\t"<<i<<"\t"<<j+1<<"\t"<<i+1<<"\n";
                numVias++;
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////////////

    //output basic program data
    outFile  << "BENCH " << benchNum <<" REPORT:\n";
    std::cout<< "\n\n\n\nBENCH " << benchNum <<" REPORT:\n\n";

    outFile  << "ORIGINAL CELLS," << numOfCells <<"\n";
    std::cout<< "ORIGINAL CELLS: " << numOfCells <<"\n";

    outFile  << "ORIGINAL NETS," << numOfNets <<"\n\n";
    std::cout<< "ORIGINAL NETS: " << numOfNets <<"\n\n";


    int numFeedThru = 0;
    for(size_t i=0; i<cellData.size(); i++) if(cellData[i].r > 4) numFeedThru++;

    outFile  << "FEED-THRU CELLS," << numFeedThru <<"\n";
    std::cout<< "FEED-THRU CELLS: " << numFeedThru <<"\n";

    int numDogleg = cellData.size()-numOfCells-numFeedThru;

    outFile  << "FINAL NETS," << netlistPairs.size()-numDogleg <<"\n";
    std::cout<< "FINAL NETS: " << netlistPairs.size()-numDogleg <<"\n";

    outFile  << "NUMBER OF DOGLEGS," << numDogleg <<"\n\n";
    std::cout<< "NUMBER OF DOGLEGS: " << numDogleg <<"\n\n";


    int totalArea = layout[0].size()*layout.size();
    int emptySpace = totalArea-cellArea-wireArea;

    outFile  << "LAYOUT DIMENSIONS (WxH)," << layout[0].size() <<"," << layout.size() <<"\n\n";
    std::cout<< "LAYOUT DIMENSIONS (WxH): " << layout[0].size() <<"x" << layout.size() <<"\n\n";

    outFile  << "CELL AREA," << cellArea << "," << (float)cellArea/totalArea <<"\n";
    std::cout<< "CELL AREA: " << cellArea << " = " << (float)cellArea/totalArea <<"\n";

    outFile  << "WIRE AREA," << wireArea << "," << (float)wireArea/totalArea <<"\n";
    std::cout<< "WIRE AREA: " << wireArea << " = " << (float)wireArea/totalArea <<"\n";

    outFile  << "NUMBER OF VIAS," << numVias << "," << (float)numVias/totalArea <<"\n";
    std::cout<< "NUMBER OF VIAS: " << numVias << " = " << (float)numVias/totalArea <<"\n";

    outFile  << "EMPTY SPACE," << emptySpace << "\n\n";
    std::cout<< "EMPTY SPACE: " << emptySpace << "\n\n";

    outFile  << "TOTAL AREA," << totalArea << "\n";
    std::cout<< "TOTAL AREA: " << totalArea << "\n";

    outFile  << "FEATURE DENSITY," << (float)(cellArea+wireArea)/totalArea << "\n\n\n";
    std::cout<< "FEATURE DENSITY: " << (float)(cellArea+wireArea)/totalArea << "\n\n\n";


    double endTime = ( std::clock() - startTime ) / (double) CLOCKS_PER_SEC;
    outFile  << "EXECUTION TIME," << endTime << ",s";
    std::cout<< "EXECUTION TIME: " << endTime << " s\n\n";
}
