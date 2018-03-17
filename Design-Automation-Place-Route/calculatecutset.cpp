#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "lin.h"
#include "gain.h"
#include "celllist.h"
#include "dvalues.h"
#include "pass.h"

void calculateCutset(std::vector<int> &fullPartition, int totalCells, std::vector<std::vector<int> > &netArray,
                     std::vector<numberList> &cellList, int numOfCells, int numOfNets, int &cutset,
                     std::vector<int> &mainPartition)
{
    int sum, Gmax, k, maxGain, pass=0, cutset2=0, newCells;
    std::vector<int> partitionAPrime, partitionBPrime, A, B, X, Y, gains, partitionA, partitionB;
    std::vector<bool> truth(numOfNets, false);
    std::vector<dValues> D(totalCells);
    bool buffer=true;

    for (int i=0; i<numOfCells; i+=2)
    {
        partitionA.push_back(fullPartition[i]);
        partitionB.push_back(fullPartition[i+1]);
    }

    for(int i=0; i<numOfNets; i++)
    {
        if ((std::find(partitionA.begin(), partitionA.end(), netArray[0][i]) != partitionA.end() &&
                std::find(partitionA.begin(), partitionA.end(), netArray[1][i]) != partitionA.end()) ||
                (std::find(partitionB.begin(), partitionB.end(), netArray[0][i]) != partitionB.end() &&
                 std::find(partitionB.begin(), partitionB.end(), netArray[1][i]) != partitionB.end()))
        {
            truth[i] = true;
        }
    }

    for (int i=0; i<numOfNets; i++)
    {
        if(std::find(fullPartition.begin(), fullPartition.end(), netArray[0][i]) != fullPartition.end() &&
                std::find(fullPartition.begin(), fullPartition.end(), netArray[1][i]) != fullPartition.end())
        {
            if (!truth[i])
            {
                cutset += 1;
            }
            else
            {
                cellList[netArray[0][i]].updateSame(netArray[1][i], truth[i]);
                cellList[netArray[1][i]].updateSame(netArray[0][i], truth[i]);
            }
        }
    }

    gains.resize(numOfCells/2, 0);
    //create loop to perform a pass
    while(buffer)
    {
        partitionAPrime = partitionA;
        partitionBPrime = partitionB;
        A = partitionAPrime;
        B = partitionBPrime;

        //unlock all the D-values for editing
        for(int i=0; i<totalCells; i++)
        {
            D[i].unlock();
        }

        //create a loop to calculate the maximum gain for the current pass
        passRun(A, numOfNets, netArray, B, numOfCells, D, X, Y, cellList, partitionAPrime, partitionBPrime,
                gains, maxGain, totalCells);

        pass++;
        sum = 0;
        Gmax = 0;
        k = 0;

        //find the max gain from the previous pass and find the k value
        for (unsigned int i = 0; i<numOfCells/2; i++)
        {
            for (unsigned int n = 0; n<i+1; n++)
            {
                if(n < gains.size())
                {
                    sum += gains[n];
                }
            }
            if (sum > Gmax)
            {
                Gmax = sum;
                k = i;
            }
            sum = 0;
        }

        //swap the values in the partitions up to the k value
        /*if (Gmax > 0)
        {
            for (unsigned int i = 0; i<k+1; i++)
            {
                partitionA.erase(std::remove(partitionA.begin(), partitionA.end(), X[i]), partitionA.end());
                partitionB.erase(std::remove(partitionB.begin(), partitionB.end(), Y[i]), partitionB.end());
                partitionA.push_back(Y[i]);
                partitionB.push_back(X[i]);
            }
            cutset = cutset - Gmax;                       //recalculate cutset
        }
        else
        {*/
            buffer=false;                                 //finish the algorithm
        //}

        X.clear();
        Y.clear();
    }

    newCells = numOfCells/2;

    if(newCells%2!=0)
    {
        partitionA.push_back(totalCells-1);
        newCells+=1;
    }

    fullPartition.clear();

    if(partitionA.size() != 1 && partitionA.size() != 2)
    {
        calculateCutset(partitionA, totalCells, netArray, cellList, newCells, numOfNets, cutset2, mainPartition);
    }
    else if(partitionA.size()==1)
    {
        mainPartition.push_back(partitionA[0]);
    }
    else if (partitionA.size()==2)
    {
        mainPartition.push_back(partitionA[0]);
        mainPartition.push_back(partitionA[1]);
    }

    newCells = numOfCells/2;

    if(newCells%2!=0)
    {
        partitionB.push_back(totalCells-1);
        newCells+=1;
    }

    if(partitionB.size() != 1 && partitionB.size() != 2)
    {
        calculateCutset(partitionB, totalCells, netArray, cellList, newCells, numOfNets, cutset2, mainPartition);
    }
    else if(partitionB.size()==1)
    {
        mainPartition.push_back(partitionB[0]);
    }
    else if (partitionB.size()==2)
    {
        mainPartition.push_back(partitionB[0]);
        mainPartition.push_back(partitionB[1]);
    }

}
