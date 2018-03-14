#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include "pass.h"
#include "gain.h"
#include "dvalues.h"
#include "celllist.h"
#include "lin.h"

void passRun(std::vector<int> &A, int &numOfNets, std::vector<std::vector<int> > &netArray, std::vector<int> &B,
             int &numOfCells, std::vector<dValues> &D, std::vector<int> &X, std::vector<int> &Y,
             std::vector<numberList> &cellList, std::vector<int> &partitionAPrime,
             std::vector<int> &partitionBPrime, std::vector<int> &gains, int &maxGain,
             int &totalCells)
{
    std::vector<bool> truth2(numOfNets, false);
    std::vector<int> DummyPartition;
    std::vector<std::vector<int> > dummyA, dummyB;
    int numCells = numOfCells;
    int aValue, bValue, b=0;
    int val1, val2;

    DummyPartition = A;

    for (int i=0; i<numOfCells/2; i++)
    {
        DummyPartition.push_back(B[i]);
    }

    for(int i=0; i<numOfNets; i++)
    {
        if ((std::find(A.begin(), A.end(), netArray[0][i]) != A.end() &&
                std::find(A.begin(), A.end(), netArray[1][i]) != A.end()) ||
                (std::find(B.begin(), B.end(), netArray[0][i]) != B.end() &&
                 std::find(B.begin(), B.end(), netArray[1][i]) != B.end()))
        {
            truth2[i] = true;
        }
    }

    //set all dvalues to zero
    for (int i=0; i<totalCells; i++)
    {
        D[i].setValue(0);
    }
    //calculate the intial d-values
    for (int i=0; i<numOfNets; i++)
    {
        if(std::find(DummyPartition.begin(), DummyPartition.end(), netArray[0][i]) != DummyPartition.end() &&
                std::find(DummyPartition.begin(), DummyPartition.end(), netArray[1][i]) != DummyPartition.end())
        {
            if (truth2[i])
            {
                D[netArray[0][i]].subtract();
                D[netArray[1][i]].subtract();
            }
            else
            {
                D[netArray[0][i]].add();
                D[netArray[1][i]].add();
            }
        }
    }

    while (!partitionAPrime.empty())
    {
        if(b != 0)
        {
            if (aValue < numOfCells && bValue < numOfCells)
            {
                cellList[aValue].updateDValue(D, cellList[bValue], bValue);
                cellList[bValue].updateDValue(D, cellList[aValue], aValue);
                cellList[aValue].unlockD(D);
                cellList[bValue].unlockD(D);
            }
        }

        for(int i = 0; i < numCells/2; i++)
        {
            if(i==0)
            {
                val1 = D[partitionAPrime[0]].getValue();

                aValue = partitionAPrime[0];
                val2 = D[partitionBPrime[0]].getValue();
                bValue = partitionBPrime[0];
            }

            if(D[partitionAPrime[i]].getValue() > val1)
            {
                val1 = D[partitionAPrime[i]].getValue();
                aValue = partitionAPrime[i];
            }

            if(D[partitionBPrime[i]].getValue() > val2)
            {
                val2 = D[partitionBPrime[i]].getValue();
                bValue = partitionBPrime[i];
            }
        }

        //dummyA.clear();
        //dummyB.clear();
        //dummyA.resize(numCells/2, std::vector<int>(2, 0));
        //dummyB.resize(numCells/2, std::vector<int>(2, 0));

        //create a vector to keep track of the d values
        /*for (unsigned int i =0; i<dummyA.size(); i++)
        {
            if(i<partitionAPrime.size())
            {
                dummyA[i][0] = D[partitionAPrime[i]].getValue();
                dummyA[i][1] = partitionAPrime[i];
            }
        }

        for (unsigned int i =0; i<dummyB.size(); i++)
        {
            if(i<partitionAPrime.size())
            {
                dummyB[i][0] = D[partitionBPrime[i]].getValue();
                dummyB[i][1] = partitionBPrime[i];
            }
        }*/

        //sort the d-values for each partition
        //std::sort(dummyA.begin(), dummyA.end(), sortDValue);
        //std::sort(dummyB.begin(), dummyB.end(), sortDValue);

        maxGain = val1+val2-2*cellList[aValue].getWeight(bValue);

        //gain(numOfCells, cellList, maxGain, aValue, bValue, dummyA, dummyB);

        X.push_back(aValue);        //add the value from the A partition to X
        Y.push_back(bValue);        //add the value from the B partition to Y
        if(aValue < numOfCells && bValue < numOfCells)
        {
            D[aValue].lock();       //lock the D value for a
            D[bValue].lock();       //lock the D value for b
        }

        //remove the values from their respective partitions
        partitionAPrime.erase(std::remove(partitionAPrime.begin(), partitionAPrime.end(), aValue), partitionAPrime.end());
        partitionBPrime.erase(std::remove(partitionBPrime.begin(), partitionBPrime.end(), bValue), partitionBPrime.end());

        //update the new partition for the next gain find
        A = partitionAPrime;
        B = partitionBPrime;
        for(unsigned int i= 0; i<b+1; i++)
        {
            if(i < Y.size() && i < X.size())
            {
                A.push_back(Y[i]);
                B.push_back(X[i]);
            }
        }
        //keep track of all the gains calculated
        if(b<gains.size())
        {
            gains[b] = maxGain;
            //std::cout<<gains[b]<<"\n";
        }
        b++;
        numCells = numCells - 2;

       /*     for(int i=0; i<numCells/2; i++)
    {
        std::cout<<partitionAPrime[i]<<"\t";
    }
    std::cout<<"\n";
    for(int i=0; i<numCells/2; i++)
    {
        std::cout<<partitionBPrime[i]<<"\t";
    }*/

    }
    //std::cout<<"\n";

}
