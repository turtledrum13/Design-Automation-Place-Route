//
//  layoutOperations.hpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef layoutOperations_hpp
#define layoutOperations_hpp

#include <stdio.h>
#include <vector>

#include "structures.h"


void makeCell(cell C, std::vector<std::vector<int> > & layout);

void addRows(int numRows, std::vector<std::vector<int> > & layout);

void addCols(int numCols, std::vector<std::vector<int> > & layout);

void createArray(std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells,
                 std::vector<std::vector<int> > &layout);
void funct4(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct8(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct16(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct32(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct64(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct128(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct256(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct512(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
void funct1024(int xNum, int yNum, int &z, std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
#endif /* layoutOperations_hpp */
