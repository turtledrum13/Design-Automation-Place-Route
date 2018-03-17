#ifndef CELLLIST_H
#define CELLLIST_H
#include <vector>
#include <stdlib.h>
#include "lin.h"

void createCellList(int numOfNets, std::vector<std::vector<int> > &netArray, std::vector<numberList> &cellList,
                    std::vector<std::pair<int,int> > &netlist);

bool sortDValue(const std::vector<int> &vect1, const std::vector<int> &vect2);

#endif
