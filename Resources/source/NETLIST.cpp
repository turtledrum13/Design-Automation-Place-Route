//This is the NETLIST function

#include "NETLIST.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

/////////////////////////

bool compare(const std::pair<int,int>& inp1, const std::pair<int,int>& inp2)
{
        return inp1.first < inp2.first;
}

crs_matrix create(std::vector<std::pair<int,int> >& edgePair, int nets, int cells)
{
    crs_matrix CSR;
    CSR.nnz = nets;        //# nonzero elements in matrix=total # of nets
    CSR.num_rows = cells;  //square matrix where row,col= edge between a,b
    CSR.num_cols = cells;

    //Phase 1: Row ordering///////////////////////////////////////////////

    std::sort(edgePair.begin(), edgePair.end(), compare);
    CSR.offset = edgePair[0].first-1;


    //Phase 2: Column Ordering////////////////////////////////////////////

    bool sorted = false;
    int startLoc = 0;
    int stopLoc = 0;
    bool lookAhead = true;
    int current_row = edgePair[0].first;

    while(stopLoc < CSR.nnz)
    {
        while(lookAhead)
        {
            if(edgePair[startLoc].first == edgePair[stopLoc].first)
            {
                stopLoc++;
            }
            else
            {
                lookAhead = false;
            }
        }

        sorted = false;             //reset finished flag on each new row

        //sort algorithm for the column values in this particular row
        while(!sorted)
        {
            sorted = true;

            for(int c = startLoc; c<stopLoc-1; c++)
            {
                if(edgePair[c].second>edgePair[c+1].second)
                {
                    std::swap(edgePair[c].second,edgePair[c+1].second);
                    sorted = false;
                }
            }
        }
        startLoc = stopLoc;
        current_row++;
        lookAhead = true;
    }


    //Phase 3: Duplicate Net Compression and Weighting////////////////////

    //Value enumeration -- Uniform weights across netlist
    CSR.values.resize(CSR.nnz, 1);

    for(unsigned int v = 0; v<edgePair.size()-1; v++)
    {
        if((edgePair[v].second==edgePair[v+1].second) and (edgePair[v].first==edgePair[v+1].first))
        {
            edgePair.erase(edgePair.begin()+v+1);
            CSR.values.erase(CSR.values.begin()+v+1);

            CSR.values[v]++;
            v--;
        }
    }
    //update to number nonzero once combination of duplicates is finished
    CSR.nnz = edgePair.size();


    //Phase 4: Row pointer updating///////////////////////////////////////

    //initialize to the first row containing a value
    int lastNum = edgePair[0].first;

    //this first row will get a 0, so index is first row+1 to start
    int index = CSR.offset+1;

    //one row pointer for every row, plus one for the end value
    CSR.row_ptr.resize(CSR.num_rows+1,CSR.nnz);

    for(int i = 0; i<edgePair[0].first; i++)
    {
        CSR.row_ptr[i] = 0;
    }

    for(int p = 1; p<CSR.nnz; p++) {
        if(edgePair[p].first != lastNum)
        {
            //if row with no nonzero values detected, repeat last number
            while(edgePair[p].first-lastNum > 1)
            {
                CSR.row_ptr[index] = p;
                lastNum++;
                index++;
            }
            CSR.row_ptr[index] = p;
            lastNum = edgePair[p].first;
            index++;
        }
    }

    //one additional row pointer to mark the end of the netlist
    CSR.row_ptr[CSR.num_rows] = CSR.nnz;

    //Fill column indicies once initial vector is sorted and condensed
    CSR.col_ind.resize(edgePair.size(),0);
    for(unsigned int p=0; p<edgePair.size(); p++)
    {
        CSR.col_ind[p] = edgePair[p].second;
    }

    return CSR;
}
