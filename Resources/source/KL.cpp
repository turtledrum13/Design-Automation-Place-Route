//  main_2.cpp
//  DesignAutomation1
//
//  Created by Aaron J Cunningham on 2/17/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "KL.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>


///////////////////////////////////////////////////////////////////////////
//Structures and Classes///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
//Global Vairables////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



std::vector<int> extCost, intCost, Dvals;
std::vector<int> extCost_best, intCost_best, Dvals_best;


///////////////////////////////////////////////////////////////////////////
//Prototyping Functions///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

crs_matrix create(std::vector<std::pair<int,int> >& edgePair);
bool compare(const std::pair<int,int>& inp1, const std::pair<int,int>& inp2);

std::vector<bool> copyPartition(std::vector<bool>& toCopy);

std::vector<int> findMaxD(std::vector<int>& DVec, std::vector<bool>& partitionVec, std::vector<bool>& unvisitedVec);

int cCalc(int a, int b, crs_matrix& netlist);

void updateCost(int cell, std::vector<int>& DVec, std::vector<int>& extVec, std::vector<int>& intVec, std::vector<bool>& PartitionVec, crs_matrix& net, crs_matrix& net_trans);

int cutsize(std::vector<bool>& partitionVec);


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void KL(std::vector<int> & initPartition, std::vector<int> & partitionA, std::vector<int> & partitionB, crs_matrix & net_list)
{

    int cells;
    int N;

    if (initPartition.size() % 2 == 0)
    {
        cells = initPartition.size();
    }
    else
    {
        initPartition.push_back(0);    //add a dummy zero
        cells = initPartition.size();
    }

    N = cells/2;  //half the total number of cells for easy index counting
    //generate an initial partitioning

    for (int i = 0; i < N; i++)
    {
        partitionA(i) = i;
    }

    for (int i = N; i < cells; i++)
    {
        partitionB(i) = i;
    }


    //printf("cells: %i\tnets: %i\tN: %i\n\n",cells,nets,N);


    //Load in edge data from the file/////////////////////////////////////


    //Reading Net Weights into Condensed Sparse Row matrix ///////////////

    //Bottom half of triangular matrix

    //printf("\nNetList Matrix Created\t\t(%lds)\n\n",(time(NULL)-runTime));

    //Top half of triangular matrix

    //printf("NetList Transpose Created\t(%lds)\n\n",(time(NULL)-runTime));


    //Creating initial partitions/////////////////////////////////////////

    //(false = partition 1, true = partition 2)
    std::vector<bool> partitions(cells, false);

    for(int i = N; i<cells; i++)
    {
        partitions[i] = true;
    }

    std::vector<bool> bestPartitions = copyPartition(partitions);
    std::vector<bool> veryBestPartitions = copyPartition(partitions);

    //////////////////////////////////////////////////////////////////////
    //Initial Calculation of D values/////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    extCost.resize(cells,0); extCost_best.resize(cells,0);
    intCost.resize(cells,0); intCost_best.resize(cells,0);
    Dvals.resize(cells,0); Dvals_best.resize(cells,0);

    for(int a = 0; a<cells; a++)
    {
        bool partition = partitions[a];

        {
            for(int c = netlist.row_ptr[a]; c<netlist.row_ptr[a+1]; c++)
            {
                if(partition == partitions[netlist.col_ind[c]-1])
                {
                    intCost[a] += netlist.values[c];
                    Dvals[a] -= netlist.values[c];
                }
                else
                {
                    extCost[a] += netlist.values[c];
                    Dvals[a] += netlist.values[c];
                }
            }

            for(int r = netlist_transpose.row_ptr[a]; r<netlist_transpose.row_ptr[a+1]; r++)
            {
                if(partition == partitions[netlist_transpose.col_ind[r]-1])
                {
                    intCost[a] += netlist_transpose.values[r];
                    Dvals[a] -= netlist_transpose.values[r];
                }
                else
                {
                    extCost[a] += netlist_transpose.values[r];
                    Dvals[a] += netlist_transpose.values[r];
                }
            }
        }
    }


    for(int c=0; c<cells; c++)
    {
        Dvals_best[c] = Dvals[c];
        extCost_best[c] = extCost[c];
        intCost_best[c] = intCost[c];
    }

    //////////////////////////////////////////////////////////////////////
    //Initializing other container variables and flags////////////////////
    //////////////////////////////////////////////////////////////////////

    int origCutSize = cutsize(partitions);

    int queueDepth = 1;
    int bestQueueDepth = 1;
    int veryBestQueueDepth = 1;
    std::vector<int> maxPair(2,0);

    int cutSize_best, cutSize, new_cutSize;
    int previous = origCutSize;
    int previous_best = previous+1;

    int trials = 0;
    int epoch = 0;
    int epoch_optimal = 0;
    int regressions;
    bool errorFlag = false;

    bool finished = false;

    //printf("Variables Initialized...\t\t(%lds)\n",(time(NULL)-runTime));


    //////////////////////////////////////////////////////////////////////
    //Begin Running Passes////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    while((previous < previous_best) and (trials < 7))
    {
        trials++;

        //Reset the variables and cost vectors

        std::vector<bool> unvisited(cells, true);  // 1 means not visited
        queueDepth = 1;

        for(int c=0; c<cells; c++)
        {
            Dvals[c] = Dvals_best[c];
            extCost[c] = extCost_best[c];
            intCost[c] = intCost_best[c];
        }

        partitions = copyPartition(bestPartitions);

        origCutSize = cutsize(partitions);
        cutSize_best = origCutSize;
        cutSize = origCutSize;
        previous_best = previous;
        regressions = 0;

        printf("Initial Cut Size: %i\n\n", origCutSize);


        //Begin Pass Iterations///////////////////////////////////////////

        while(queueDepth <= N and not finished)
        {
            //Find best D values to swap (only unvisited ones)

            maxPair = findMaxD(Dvals,partitions,unvisited);
            int a = maxPair[0];
            int b = maxPair[1];


            //Calculate Gain of swapping these two -- G = Da + Db -2Cab

            int C_ab = cCalc(a, b, netlist);
            int gain = Dvals[a] + Dvals[b] - 2*C_ab;


            //Mark two swapped cells as visited

            unvisited[a] = false;
            unvisited[b] = false;


            //Update D values and costs of a and b
            //assumes no edge between a and b

            std::swap(extCost[a], intCost[a]);
            Dvals[a] *= -1;

            std::swap(extCost[b], intCost[b]);
            Dvals[b] *= -1;


            //Update D values and costs of any cell attached to a or b
            //Dx' = Dx + 2Cxa - 2Cxb , Dy' = Dy + 2Cyb - 2Cya

            updateCost(a, Dvals, extCost, intCost, partitions, netlist, netlist_transpose);
            updateCost(b, Dvals, extCost, intCost, partitions, netlist, netlist_transpose);


            //Correction factor in case a and b shared an edge
            //since "updateCost" function does not check for that

            if(C_ab != 0)
            {
                intCost[a] -= 2*C_ab;
                extCost[a] += 2*C_ab;
                Dvals[a]   += 4*C_ab;

                intCost[b] -= 2*C_ab;
                extCost[b] += 2*C_ab;
                Dvals[b]   += 4*C_ab;
            }


            //Swap a into Partition B and b into Partition A

            partitions.swap(partitions[a], partitions[b]);
            new_cutSize = cutsize(partitions);

            if(new_cutSize != (cutSize-gain))
            {
                errorFlag = true;
                printf("\n\n\t\t\t\t[ Calculation Error: Aborting ]\n\n");
                printf("%i != %i\n\n",new_cutSize, (cutSize-gain));
                break;
            }



            //Checking exit conditions////////////////////////////////////

            //if too many consecutive regressions on the cut size
            if((new_cutSize >= cutSize) and regressions > 1000)
            {
                finished = true;
                printf("\n\n\n\t\t[[[ Fully reduced to cutsize of %i in %i passes ]]]\n\n\n", cutSize_best, queueDepth-1);
            }
            else if((new_cutSize >= cutSize) and (regressions <= 1000))
            {
                regressions++;
            }
            else if (new_cutSize <= cutSize_best)
            {
                regressions = 0;
                cutSize_best = new_cutSize;
                bestPartitions = copyPartition(partitions);
                bestQueueDepth = queueDepth;

                for(int c=0; c<cells; c++)
                {
                    Dvals_best[c] = Dvals[c];
                    extCost_best[c] = extCost[c];
                    intCost_best[c] = intCost[c];
                }
            }

            cutSize = new_cutSize;
            printf("\nOrig Cut:%i This Cut:%i Current Best Cut: %i  Old Best Cut:%i Pass:%i Queue Depth:%i\n_\t_\t_\t_\t_\t_\t_\n", origCutSize,new_cutSize,cutSize_best,previous_best, epoch+1, queueDepth);

            queueDepth++;
        }                                   //return to iteration loop top

        if(errorFlag)
        {
            break;
        }

        if(cutSize_best < previous)
        {
            previous = cutSize_best;
            veryBestPartitions = copyPartition(bestPartitions);
            veryBestQueueDepth = bestQueueDepth;
            epoch_optimal = epoch + 1;

            if(previous_best == 0)                    //If fully optimized
            {
                break;
            }
        }

        partitions = copyPartition(bestPartitions);

        finished = false;
        queueDepth = 1;
        epoch++;

        //printf("\n\nFinished %i iteration(s)\t\t\t\t\t(%lds)\n\n\n",epoch,(time(NULL)-runTime));
    }                                            //return to Pass loop top


    //printf("\n\n\nBENCH %s ...\n%i total iterations performed. Best result on Pass %i at k=%i.\n",benchNum.c_str(),epoch, epoch_optimal, veryBestQueueDepth);


    //Output Printing/////////////////////////////////////////////////////

    /*if(not errorFlag)
    {
        file_out << previous_best << "\n"; printf("\n%i\n",previous_best);

        for(int p = 0; p<cells; p++)
        {
            if(not veryBestPartitions[p])
            {
                file_out << p+1 << " "; printf("%i ",p+1);
            }
        }

        file_out << "\n"; printf("\n");

        for(int q = 0; q<cells; q++)
        {
            if(veryBestPartitions[q])
            {
                file_out << q+1 << " "; printf("%i ",q+1);
            }
        }

        printf("\n\nCompleted bench %s in %lds\n\n",benchNum.c_str(),(time(NULL)-runTime));
    }*/
}


//////////////////////////////////////////////////////////////////////////
/////External Function Definitions////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

std::vector<bool> copyPartition(std::vector<bool>& toCopy)
{
    std::vector<bool> toWrite(toCopy.size(),false);
    for(unsigned int i=0; i<toCopy.size(); i++)
    {
        toWrite[i] = toCopy[i];
    }

    return toWrite;
}


std::vector<int> findMaxD(std::vector<int>& DVec, std::vector<bool>& partitionVec, std::vector<bool>& unvisitedVec)
{
    std::vector<int> maxLoc(2,0);
    int maxVal[2] = {-999999, -999999};

    for(unsigned int k=0; k<DVec.size(); k++)
    {
        if(unvisitedVec[k])
        {
            if(not partitionVec[k])  //If the cell belongs to Partition 1
            {
                if(DVec[k] > maxVal[0])
                {
                    maxVal[0] = DVec[k];
                    maxLoc[0] = k;
                }
            }
            else                     //If the cell belongs to Partition 2
            {
                if(DVec[k] > maxVal[1])
                {
                    maxVal[1] = DVec[k];
                    maxLoc[1] = k;
                }
            }
        }
    }

    return maxLoc;
}


int cCalc(int a, int b, crs_matrix& netlist)
{
    int weight = 0;
    int A, B;

    if(a>b)
    {
        A = a;
        B = b;
    }
    else
    {
        A = b;
        B = a;
    }

    for(int p=netlist.row_ptr[A]; p<netlist.row_ptr[A+1]; p++)
    {
        if(netlist.col_ind[p] == (B+1))
        {
            weight = netlist.values[p];
        }
    }

    return weight;
}


void updateCost(int cell, std::vector<int>& DVec, std::vector<int>& extVec, std::vector<int>& intVec, std::vector<bool>& PartitionVec, crs_matrix& net, crs_matrix& net_trans)
{
    for(int k=net.row_ptr[cell]; k<net.row_ptr[cell+1]; k++)
    {
        int index = net.col_ind[k] - 1;

        if(PartitionVec[cell] == PartitionVec[index])
        {
            DVec[index] += 2*net.values[k];
            extVec[index] += net.values[k];
            intVec[index] -= net.values[k];
        }
        else
        {
            DVec[index] -= 2*net.values[k];
            extVec[index] -= net.values[k];
            intVec[index] += net.values[k];
        }
    }

    for(int l=net_trans.row_ptr[cell]; l<net_trans.row_ptr[cell+1]; l++)
    {
        int index = net_trans.col_ind[l] - 1;

        if(PartitionVec[cell] == PartitionVec[index])
        {
            DVec[index] += 2*net_trans.values[l];
            extVec[index] += net_trans.values[l];
            intVec[index] -= net_trans.values[l];
        }
        else
        {
            DVec[index] -= 2*net_trans.values[l];
            extVec[index] -= net_trans.values[l];
            intVec[index] += net_trans.values[l];
        }
    }
}


int cutsize(std::vector<bool>& partitionVec)
{
    int CUTSIZE = 0;
    for(int k = 0; k<partitionVec.size(); k++)
    {
        if(partitionVec[k])
        {
            CUTSIZE += extCost[k];
        }
    }

    return CUTSIZE;
}
