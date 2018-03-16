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




int main()	    //use argc and argv to pass command prompt arguments to main()
{
    //initialize files
    std::ifstream fileIn ("b4.net");
    std::ofstream outFile ("out.txt");
    std::ofstream outCSV ("magicCSV.csv");
    
    //intitilaize vectors
    std::vector<numberList> cellList;
    std::vector<cell> cellData;
    std::vector<std::vector<int> > netArray, layout;
    std::vector<int> partitionA, partitionB, gains, fullPartition, mainPartition;
    std::vector<std::pair <int,int> > netlist;
    
    //initialize variables
    int numOfCells, numOfNets, cutset=0, numOfCells2, numOfPartitions, totalCells;
    double m;

    
    fileIn >> numOfCells2;
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
    if(numOfCells2 %2 !=0)
    {
        numOfCells=numOfCells2+1;
    }
    else
    {
        numOfCells=numOfCells2;
    }

    totalCells = numOfCells;

    
    //calculate the number of partitions to be found
    m = log(numOfCells2)/log(2);

    if (m!=(int) m)
    {
        m+=1;
    }
    numOfPartitions = m;

    cellList.resize(numOfCells);
    
    
    //printing netlist for debugging
                for (int i=0; i<netlist.size(); i=i+2)
                {
                    printf("(%i,%i) --> (%i,%i)\n",netlist[i].first+1,netlist[i].second,netlist[i+1].first+1,netlist[i+1].second);
                }
    //end print netlist
    
    
//    for (int i=0; i<netArray.size(); i++)
//    {
//        int index = netArray[i].size();
//        printf("%i: \n",index);
//
//        for(int j=0; j<index; j++)
//        {
//            printf("%i ",netArray[i][j]);
//        }
//    }
    
    

    //split the cells into 2 partitions of equal size.
    for (int i=0; i<numOfCells/2; i++)
    {
        partitionA.push_back(i);
        partitionB.push_back(i+numOfCells/2);
        fullPartition.push_back(i);
        fullPartition.push_back(i+numOfCells/2);
    }

    //create the list of cells and point them to their net pair
    createCellList(numOfNets, netArray, partitionA, partitionB, cutset, cellList, netlist);

    std::cout<<"\npropagated input\n";

    //calculate the cutset
    calculateCutset(fullPartition, totalCells, netArray, cellList, numOfCells, numOfNets, cutset, mainPartition);

    
    
    //Routing
    
    //First: Global Routing
    layout.resize(6, std::vector<int>(7*mainPartition.size()-1)); // single row placement
    
    cellData.resize(mainPartition.size());
    int iterate = 0;
    
    for (int i=0; i<mainPartition.size(); i++)
    {
        int cellNum = mainPartition[i];
        cellData[cellNum].x = 5;
        cellData[cellNum].y = iterate;
        cellData[cellNum].r = 1;
        cellData[cellNum].nets = 0;
        cellData[cellNum].cell = cellNum;
        
        iterate += 7;
        //create cell in layout
        makeCell(cellData[cellNum], layout);
    }
    
    global();
    
    //Second: Channel Routing
    channel();
    
    
    

    //output the cutset and partitions to output file
    outFile<<cutset<<"\n";
    std::cout<<"\n"<<cutset<<"\n";
    
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

