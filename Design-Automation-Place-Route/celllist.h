#ifndef CELLLIST_H
#define CELLLIST_H
#include <vector>
#include <stdlib.h>
#include "lin.h"

void createCellList(int numOfNets, std::vector<std::vector<int> > &netArray, std::vector<int> &partitionA,
               std::vector<int> &partitionB, int &cutset, std::vector<numberList> &cellList,
               std::vector<int> &netlist);

bool sortDValue(const std::vector<int> &vect1, const std::vector<int> &vect2);

#endif
