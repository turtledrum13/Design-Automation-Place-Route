#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "lin.h"
#include "gain.h"

void gain(int numOfCells, std::vector<numberList> &cellList, int &maxGain, int &aValue, int &bValue,
          std::vector<std::vector<int> > &dummyA, std::vector<std::vector<int> > &dummyB)
{
    int G, gainValue;
    bool flag;

    for (int i = 0; i<dummyA.size(); i++)
    {
        for (unsigned int n = 0; n<dummyB.size(); i++)
        {
            G = dummyA[i][0]+dummyB[n][0];
            if (i==0&&n==0)
            {
                if(dummyA[i][1] < numOfCells)
                {
                    gainValue = dummyA[i][0]+dummyB[n][0]-2*cellList[dummyA[i][1]].getWeight(dummyB[n][1]);
                }
                maxGain = gainValue;
                aValue = dummyA[i][1];
                bValue = dummyB[n][1];
            }
            else
            {
                if(G > maxGain)
                {
                    if (dummyA[i][1] < numOfCells)
                    {
                        gainValue = dummyA[i][0]+dummyB[n][0]-2*cellList[dummyA[i][1]].getWeight(dummyB[n][1]);
                    }
                    maxGain = gainValue;
                    aValue = dummyA[i][1];
                    bValue = dummyB[n][1];
                }
                else
                {
                    return;
                }
            }
        }
    }

}
