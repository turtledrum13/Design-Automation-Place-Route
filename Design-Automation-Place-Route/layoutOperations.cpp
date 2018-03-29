//
//  layoutOperations.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "layoutOperations.hpp"
#include "classifyNets.hpp"
#include <vector>
#include <iostream>
#include <cmath>


void makeCell(cell C, std::vector<std::vector<int> > & layout)
{
    bool isEmpty = true;

    int size = 6;           //default cell size is 6x6
    if (C.r > 4) size = 3;  //if the rotation code is 5 or 6 then cell is a pass through cell

    for (int row=C.y; row>C.y-6; row--)
    {
        for (int col=C.x; col<C.x+size; col++)
        {
            if (layout[row][col] > 0) isEmpty = false;
        }
    }

    if (isEmpty)
    {
        for (int row=C.y; row>C.y-6; row--)
        {
            for (int col=C.x; col<C.x+size; col++)
            {
                layout[row][col] = C.r;
            }
        }
    }
    else
    {
        printf("failed to add cell here\n");
    }
}

void makeTrunk(net & N, int atRow, std::vector<std::vector<int> > & layout)
{
    for(int i=N.x1; i < N.x2+1; i++)
    {
        layout[atRow][i] = 7;
    }
    N.y = atRow;
    N.placed = true;
}

void makeBranches(std::vector<cell>& cellData, std::vector<net>& netlistPairs, std::vector<std::vector<int> >& layout)
{
    int yTrunk, ySrc, yDest, xSrc, xDest, iSrc, iDest;

    for(size_t i=0; i<netlistPairs.size(); i++)
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
        layout[yTrunk][xSrc] = 9;
        layout[yTrunk][xDest] = 9;
    }
}

void addRows(int numRows, int atRow, std::vector<std::vector<int> > & layout)
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
    double n = std::sqrt(numOfCells);

    //get the closest sqrt that is equal or greater than the number of cells
    if(n != (int) n)
    {
        n += 1;
    }

    n = int(n);

    //add the x and y coordinates to the cells in an nxn manner
    for (int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            if (mainPartition[z]!=numOfCells)
            {
                cellData[mainPartition[z]].x = i*7;
                cellData[mainPartition[z]].y = j*7+5;
                cellData[mainPartition[z]].cell = mainPartition[z]+1;
            }
            z++;
        }
    }
}
