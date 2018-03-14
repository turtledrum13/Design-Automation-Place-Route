#ifndef CALCULATECUTSET_H
#define CALCULATECUTSET_H
#include<vector>
#include<stdlib.h>
#include "lin.h"

void calculateCutset(std::vector<int> &fullPartition, int totalCells,
                     std::vector<std::vector<int> > &netArray,
                     std::vector<numberList> &cellList, int numOfCells, int numOfNets, int &cutset, std::vector<int> &mainPartition);

#endif
