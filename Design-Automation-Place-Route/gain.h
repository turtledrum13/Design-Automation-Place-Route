#ifndef GAIN_H_INCLUDED
#define GAIN_H_INCLUDED
#include <vector>
#include "lin.h"

void gain(int numOfCells, std::vector<numberList> &cellList, int &maxGain, int &aValue, int &bValue,
          std::vector<std::vector<int> > &dummyA, std::vector<std::vector<int> > &dummyB);

#endif
