#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "lin.h"
#include "gain.h"
#include "celllist.h"
#include "dvalues.h"
#include "pass.h"
#include <cmath>

void calculateCutset(std::vector<int> &fullPartition, int totalCells, std::vector<std::vector<int> > &netArray,
                     std::vector<numberList> &cellList, int numOfCells, int numOfNets, int &cutset,
                     std::vector<int> &mainPartition, bool isRowPlacement)
{
    int sum, Gmax, k, maxGain, pass=0, cutset2=0, newCells, numOfPartitions, addNum;
    std::vector<int> partitionAPrime, partitionBPrime, A, B, X, Y, gains, partitionA, partitionB, dummyPartition;
    std::vector<bool> truth(numOfNets, false);
    bool buffer=true;
    double n, f;

    //used to find the row size of the layout. If doing row placement, make the size equal to 2.
    if(!isRowPlacement)
    {
        //calculate the number of rows and cells per row to ensure a square in the final layout
        f = std::sqrt(totalCells-1);
        if(f != (int) f)
        {
            f += 1;
        }
        f = int(f);

        n = f/std::sqrt(2);
        if(n != (int) n)
        {
            n += 1;
        }

        n = int(n);

        numOfPartitions = n*2;
    }
    else
    {
        n = numOfCells;
        numOfPartitions = 2;
    }


    //if this is the first run of the partitioning for placement and it is not doing row placement
    if(totalCells==(numOfCells+1) && !isRowPlacement)
    {
        //m used to determine how many dummy rows to add for partitioning
        double m = log(n)/log(2);

        if (m!=(int) m)
        {
            m+=1;
        }
        m = int(m);
        m = std::pow(2, m);

        //create the first full partition on the first run
        for (int i=0; i<totalCells/2; i++)
        {
            fullPartition.push_back(i);
            fullPartition.push_back(i+totalCells/2);
        }
        //calculate the total number of dummy cells needed to be added for partitioning
        addNum = ((n*numOfPartitions)) + ((m-n)*numOfPartitions);

        //add the dummy cells to the partition
        for(int i=numOfCells; i<addNum; i++)
        {
            fullPartition.push_back(totalCells-1);
            numOfCells+=1;
        }
    }

    //vector to keep track of the D values
    std::vector<dValues> D(totalCells);

    //create the 2 partitions for this run of the partitioning
    for (int i=0; i<numOfCells; i+=2)
    {
        partitionA.push_back(fullPartition[i]);
        partitionB.push_back(fullPartition[i+1]);
    }

    //find whether each net is in the same partition or not
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

    //increase the cutset for nets that are not in the same partition and update the partition list
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
        for (unsigned int i = 0; i<k+1; i++)
        {
            partitionA.erase(std::remove(partitionA.begin(), partitionA.end(), X[i]), partitionA.end());
            partitionB.erase(std::remove(partitionB.begin(), partitionB.end(), Y[i]), partitionB.end());
            partitionA.push_back(Y[i]);
            partitionB.push_back(X[i]);
        }
        //recalculate cutset
        cutset = cutset - Gmax;

        //finish the algorithm

        buffer=false;

        X.clear();
        Y.clear();
    }

    //create a dummy partition to combine partition A and B
    dummyPartition = partitionA;

    for(size_t i=0; i<partitionB.size(); i++)
    {
        dummyPartition.push_back(partitionB[i]);
    }

    //erase any of the dummy cells from the dummy partition
    dummyPartition.erase(std::remove(dummyPartition.begin(), dummyPartition.end(), totalCells-1), dummyPartition.end());

    partitionA.resize(0);
    partitionB.resize(0);
    int difference = numOfCells - dummyPartition.size();

    //re-add the dummy cells to the end of the partition so that all the dummy cells are at the end of the layout
    for(int i=0; i<difference; i++)
    {
        dummyPartition.push_back(totalCells-1);
    }

    for(int i=0; i<numOfCells/2; i++)
    {
        partitionA.push_back(dummyPartition[i]);
        partitionB.push_back(dummyPartition[i+numOfCells/2]);
    }

    newCells = numOfCells/2;

    //if the current partition size is larger than the calculated row size, then recursively call the partition
    //algorithm until the bisection partitioning is finished for partition A
    if(partitionA.size() > numOfPartitions)
    {
        calculateCutset(partitionA, totalCells, netArray, cellList, newCells, numOfNets, cutset2, mainPartition, isRowPlacement);
    }
    else
    {
        for(int i=0; i<numOfPartitions; i++)
        {
            mainPartition.push_back(partitionA[i]);
        }
    }

    newCells = numOfCells/2;

    //perform the same recursive bisection fuction for partition B
    if(partitionB.size() > numOfPartitions)
    {
        calculateCutset(partitionB, totalCells, netArray, cellList, newCells, numOfNets, cutset2, mainPartition, isRowPlacement);
    }
    else
    {
        for(int i=0; i<numOfPartitions; i++)
        {
            mainPartition.push_back(partitionB[i]);
        }
    }

}

//do partitioning algorithm for each row in the layout.
void rowPlacement(std::vector<std::vector<int> > &netArray, std::vector<numberList> &cellList, int numOfCells,
                  int numOfNets, std::vector<int> &mainPartition, int n)
{

    int cutset=0, z=0;
    std::vector<int> fullPartition, partition;
    std::vector<std::vector<int> > rowPartition;

    rowPartition.resize(n, std::vector<int>(n, 0));

    //m used to determine how many dummy cells to add for partitioning
    double m = log(n)/log(2);

    if (m!=(int) m)
    {
        m+=1;
    }
    m = int(m);
    m = std::pow(2, m);

    //add dummy cells to each row
    for (int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            rowPartition[i][j] = mainPartition[z];
            z++;
        }
    }

    //add the dummy cells to the partition
    for(int i=0; i<n; i++)
    {
        for(int j=n; j<m; j++)
        {
            rowPartition[i].push_back(numOfCells);
        }
    }

    //do partitioning on each row
    for (int i=0; i<n; i++)
    {
        partition.resize(0);
        calculateCutset(rowPartition[i], numOfCells+1, netArray, cellList, m, numOfNets, cutset, partition, true);
        rowPartition[i] = partition;
    }

    z = 0;

    //update the minpartition with the new rows
    for(int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            mainPartition[z] = rowPartition[i][j];
            z++;
        }
    }
}
