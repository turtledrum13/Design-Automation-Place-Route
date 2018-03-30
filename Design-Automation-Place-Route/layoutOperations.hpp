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


void makeCell(cell C, std::vector<std::vector<int> >& layout);

void makeTrunk(net& N, int atRow, std::vector<std::vector<int> >& layout);

void makeBranches(std::vector<cell>& cellData, std::vector<net>& netlistPairs, std::vector<std::vector<int> >& layout);

void addRows(int numRows, int atRow, std::vector<std::vector<int> > & layout);

void appendCols(int numCols, std::vector<std::vector<int> >& layout);

void appendRows(int numRows, std::vector<std::vector<int> >& layout);


void createArray(std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);
#endif /* layoutOperations_hpp */
