//
//  layoutOperations.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "layoutOperations.hpp"
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
                layout[row][col] = C.cell;
            }
        }
    }
    else
    {
        printf("failed to add cell here\n");
    }
}

//layout[XY.y-i].insert(layout[XY.y-i].begin()+XY.x,3, cellNum);
void addRows(int numRows, int atRow, std::vector<std::vector<int> > & layout)
{
    for (int i=0; i<numRows; i++)
    {
        size_t size = (layout[atRow].size());
        std::vector<int> insertVec (size, 0);
        
        layout.insert(layout.begin()+atRow,1,insertVec);
    }
}

void addCols(int numCols, std::vector<std::vector<int> > & layout)
{
    for (int i=0; i<numCols; i++)
    {
        for (int i=0; i<layout.size(); i++)
        {
            layout[i].push_back(0);
        }
    }
}

void createArray(std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    int z = 0;
    if (mainPartition.size()==4) funct4(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==8) funct8(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==16) funct16(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==32) funct32(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==64) funct64(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==128) funct128(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==256) funct256(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==512) funct512(0, 0, z, cellData, mainPartition, numOfCells);
    else if (mainPartition.size()==1024) funct1024(0, 0, z, cellData, mainPartition, numOfCells);
}

void funct4(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    for (int i=xNum; i<xNum+2; i++)
    {
        for(int j=yNum; j<yNum+2; j++)
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

void funct8(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct4(xNum, yNum, z, cellData, mainPartition, numOfCells);
    funct4(xNum, yNum+2, z, cellData, mainPartition, numOfCells);
}
void funct16(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct8(xNum, yNum, z, cellData, mainPartition, numOfCells);
    funct8(xNum+2, yNum, z, cellData, mainPartition, numOfCells);
}
void funct32(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct16(xNum, yNum, z, cellData, mainPartition, numOfCells);
    funct16(xNum, yNum+4, z, cellData, mainPartition, numOfCells);

}
void funct64(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct32(xNum, yNum, z, cellData, mainPartition, numOfCells);
    funct32(xNum+4, yNum, z, cellData, mainPartition, numOfCells);
}
void funct128(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct64(xNum, yNum, z, cellData, mainPartition, numOfCells);

    funct64(xNum, yNum+8, z, cellData, mainPartition, numOfCells);
}
void funct256(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct128(xNum, yNum, z, cellData, mainPartition, numOfCells);
    funct128(xNum+8, yNum, z, cellData, mainPartition, numOfCells);
}
void funct512(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct256(xNum, yNum, z, cellData, mainPartition, numOfCells);
    funct256(xNum, yNum+16, z, cellData, mainPartition, numOfCells);
}
void funct1024(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct512(xNum, yNum, z, cellData, mainPartition, numOfCells);
    funct512(xNum+16, yNum, z, cellData, mainPartition, numOfCells);
}
