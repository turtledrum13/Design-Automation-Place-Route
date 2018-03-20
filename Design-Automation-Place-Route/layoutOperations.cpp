//
//  layoutOperations.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "layoutOperations.hpp"
#include <vector>

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

void addRows(int numRows, std::vector<std::vector<int> > & layout)
{
    for (int i=0; i<numRows; i++)
    {
        int size = (layout[layout.size()-1].size());
        std::vector<int> insertVec (size, 0);

        layout.push_back(insertVec);
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

void funct4(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    for (int i=n; i<n+2; i++)
    {
        for(int j=m; m<m+2; m++)
        {
            std::cout<<cellData[mainPartition[z]]
            if (mainPartition[z]!=numOfCells)
            {
                cellData[mainPartition[z]].x = i;
                cellData[mainPartition[z]].y = j;
            }
            z++;
        }
    }
}

void funct8(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct4(n, m, z, cellData, mainPartition, numOfCells);
    funct4(n, m+2, z, cellData, mainPartition, numOfCells);
}
void funct16(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct8(n, m, z, cellData, mainPartition, numOfCells);
    funct8(n+2, m, z, cellData, mainPartition, numOfCells);
}
void funct32(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct16(n, m, z, cellData, mainPartition, numOfCells);
    funct16(n, m+4, z, cellData, mainPartition, numOfCells);

}
void funct64(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct32(n, m, z, cellData, mainPartition, numOfCells);
    funct32(n+4, m, z, cellData, mainPartition, numOfCells);
}
void funct128(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct64(n, m, z, cellData, mainPartition, numOfCells);
    funct64(n, m+8, z, cellData, mainPartition, numOfCells);
}
void funct256(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct128(n, m, z, cellData, mainPartition, numOfCells);
    funct128(n+8, m, z, cellData, mainPartition, numOfCells);
}
void funct512(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct256(n, m, z, cellData, mainPartition, numOfCells);
    funct256(n, m+16, z, cellData, mainPartition, numOfCells);
}
void funct1024(int n, int m, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells)
{
    funct512(n, m, z, cellData, mainPartition, numOfCells);
    funct512(n+16, m, z, cellData, mainPartition, numOfCells);
}
