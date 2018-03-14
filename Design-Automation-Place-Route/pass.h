#ifndef PASS_H
#define PASS_H
#include <vector>
#include <stdlib.h>
#include "dvalues.h"
#include "lin.h"

void passRun(std::vector<int> &A, int &numOfNets, std::vector<std::vector<int> > &netArray, std::vector<int> &B,
          int &numOfCells, std::vector<dValues> &D, std::vector<int> &X, std::vector<int> &Y,
          std::vector<numberList> &cellList, std::vector<int> &partitionAPrime,
          std::vector<int> &partitionBPrime, std::vector<int> &gains, int &maxGain,
          int &totalCells);

#endif
