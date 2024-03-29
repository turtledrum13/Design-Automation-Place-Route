//
//  layoutOperations.cpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.
//

#include "layoutOperations.hpp"
#include "classifyNets.hpp"
#include <vector>
#include <iostream>
#include <cmath>


void makeCell(cell C, std::vector<std::vector<int> > & layout)
{
    int size = 6;           //default cell size is 6x6
    if (C.r > 4) size = 3;  //if the rotation code is 5 or 6 then cell is a pass through cell


    for (int row=C.y; row>C.y-6; row--)
    {
        for (int col=C.x; col<C.x+size; col++)
        {
            layout[row][col] = C.r;
        }
    }

    layout[C.y][C.x] = 0; //make lower left corners 0 to help with feed-through cell placement
}


void makeTrunk(net* currentNet, int atRow, std::vector<std::vector<int> > & layout, std::vector<cell> &cellData, std::vector<net>& netlistPairs)
{
    int num = 0;

    for(int i=currentNet->x1; i < currentNet->x2+1; i++)
    {
        layout[atRow][i] = 7;
    }


    if(currentNet->dogleg)
    {
        for(int i=0; i<netlistPairs.size(); i++)
        {
            if(netlistPairs[i].num == currentNet->num && netlistPairs[i].routed)
            {
                //routedAlready = true;
                num = i;
                break;
            }
        }

        if(num > 0)
        {
            cellData[currentNet->dest.first].y = cellData[netlistPairs[num].dest.first].y;
            cellData[netlistPairs[num].dest.first].y = atRow;
        }
        else
        {
            cellData[currentNet->dest.first].y = atRow;
        }
    }

    currentNet->routed = true;
    currentNet->y = atRow;
}


void makeBranches(std::vector<cell>& cellData, std::vector<net>& netlistPairs, std::vector<std::vector<int> >& layout)
{
    int yTrunk, ySrc, yDest, xSrc, xDest, iSrc, iDest;

    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        if(netlistPairs[i].routed)
        {
            net& currentNet = netlistPairs[i];
            cell&  cellSrc = cellData[currentNet.src.first];
            cell&  cellDest = cellData[currentNet.dest.first];

            yTrunk = currentNet.y;
            ySrc = cellSrc.y + terminalOffset(currentNet.src.second, cellSrc.r);
            yDest = cellDest.y + terminalOffset(currentNet.dest.second, cellDest.r);
            
            xSrc = currentNet.xSrc;
            xDest = currentNet.xDest;
            iSrc=0;
            iDest=0;

            while(yTrunk+iSrc != ySrc)
            {
                layout[yTrunk+iSrc][xSrc] = 8; //draw metal 2 onto layout;

                if(yTrunk<ySrc) iSrc++;
                else iSrc--;
            }

            while(yTrunk+iDest != yDest)
            {
                layout[yTrunk+iDest][xDest] = 8; //draw metal 2 onto layout;

                if(yTrunk<yDest) iDest++;
                else iDest--;
            }

            //draw via at intersections of metal 1 and metal 2
            if(xSrc != xDest)
            {
                layout[yTrunk][xSrc] = 9;
                layout[yTrunk][xDest] = 9;
            }
        }
    }
}


void insertRows(int numRows, int atRow, std::vector<std::vector<int> > & layout)
{
    for (int i=0; i<numRows; i++)
    {
        size_t size = (layout[atRow].size());
        std::vector<int> insertVec (size, 0);

        layout.insert(layout.begin()+atRow,1,insertVec);
    }
}


void appendCols(int numCols, std::vector<std::vector<int> > & layout)
{
    for (int i=0; i<numCols; i++)
    {
        for (int i=0; i<layout.size(); i++)
        {
            layout[i].push_back(0);
        }
    }
}


void appendRows(int numCols, std::vector<std::vector<int> > & layout)
{
    std::vector<int> dummyRow (layout[0].size(), 0);

    for (int i=0; i<numCols; i++)
    {
        layout.push_back(dummyRow);
    }
}


void createArray(std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    int z = 0;
    double f = std::sqrt(numOfCells);
    double n;
    int numOfPartitions;

    //get the number of rows and cells per row to ensure a square in the final layout
    f = std::sqrt(numOfCells);
    if(f != (int) f)
    {
        f += 1;
    }
    f = int(f);

    n = f/std::sqrt(2);
    if(n != (int) n)
    {
        n += 1;
    }

    n = int(n);

    numOfPartitions = n*2;

    //add the x and y coordinates to the cells in an nxn manner
    for (int i=0; i<n; i++)
    {
        for(int j=0; j<numOfPartitions; j++)
        {
            if (mainPartition[z]!=numOfCells)
            {
                cellData[mainPartition[z]].x = j*6;
                cellData[mainPartition[z]].y = i*7+5;
                cellData[mainPartition[z]].cell = mainPartition[z]+1;
            }
            z++;
        }
    }

}
