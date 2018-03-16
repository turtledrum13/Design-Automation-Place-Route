#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include "lin.h"
#include "celllist.h"
#include "calculatecutset.h"

int main()	    //use argc and argv to pass command prompt arguments to main()
{
    //initialize files
    std::ifstream fileIn ("b4.net");
    std::ofstream outFile ("out.txt");
    std::ofstream outCSV ("magicCSV.csv");
    
    //intitilaize variables
    std::vector<numberList> cellList;
    std::vector<std::vector<int> > netArray;
    std::vector<int> netlist, partitionA, partitionB, gains, fullPartition, mainPartition;
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
            netlist.push_back(n2-1);
            netlist.push_back(n4-1);
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

    std::cout<<"propagated input"<<"\n";

    //calculate the cutset
    calculateCutset(fullPartition, totalCells, netArray, cellList, numOfCells, numOfNets, cutset, mainPartition);

    

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
}
