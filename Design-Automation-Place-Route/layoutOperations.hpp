//
//  layoutOperations.hpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.
//

#ifndef layoutOperations_hpp
#define layoutOperations_hpp

#include <stdio.h>
#include <vector>
#include "structures.h"


void makeCell(cell C, std::vector<std::vector<int> >& layout);

void makeTrunk(net* currentNet, int atRow, std::vector<std::vector<int> > & layout, std::vector<cell> &cellData, std::vector<net>& netlistPairs);

void makeBranches(std::vector<cell>& cellData, std::vector<net>& netlistPairs, std::vector<std::vector<int> >& layout);

void insertRows(int numRows, int atRow, std::vector<std::vector<int> > & layout);

void appendCols(int numCols, std::vector<std::vector<int> >& layout);

void appendRows(int numRows, std::vector<std::vector<int> >& layout);

void createArray(std::vector<cell> &cellData, std::vector<int> &mainPartition, int numOfCells);

#endif /* layoutOperations_hpp */
