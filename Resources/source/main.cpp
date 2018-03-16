//This is the main file
//Created 3-12-2018

//new comment
//second new comment - Aaron

#include "KL.h"
#include "NETLIST.h"

#include <iostream>
#include <fstream>
#include <string>


    /////////////////////////

int main (int argc,char *argv[])
{

    std::string inputFile;
    std::string benchNum;
    int cells, nets, N;
    crs_matrix netlist, netlist_transpose;

    for(int i = 1; i < argc; i++)
    {
        inputFile += argv[i];
    }
    if(inputFile.size() >= 7)
    {
        benchNum = inputFile[6];
	if(inputFile[7] != '.') benchNum += inputFile[7];
    }
    else
    {
        benchNum = "X";
    }


    std::ifstream bench (argv[1]);
    std::ofstream file_out (argv[2]);


    bench >> cells; bench >> nets; //populate cells and nets from file
    printf("cells: %i\tnets: %i\t \n\n",cells,nets);


    //Load in edge data from the file/////////////////////////////////////

    std::vector<std::pair<int,int> > edge_pair_hi_lo;
    std::vector<std::pair<int,int> > edge_pair_lo_hi;
    edge_pair_hi_lo.reserve(nets);
    edge_pair_lo_hi.reserve(nets);

    int n1, n2, n3, n4, n5;
    int count = 0;

    while(bench >> n1 and bench >> n2 and bench >> n3 and bench >> n4 and bench >> n5)
    {
        if(n2>n4) {
            edge_pair_hi_lo.push_back(std::make_pair(n2,n4));
            edge_pair_lo_hi.push_back(std::make_pair(n4,n2));
        }
        else
        {
            edge_pair_hi_lo.push_back(std::make_pair(n4,n2));
            edge_pair_lo_hi.push_back(std::make_pair(n2,n4));
        }
        count++;
    }

    netlist = create(edge_pair_hi_lo);
    netlist_transpose = create(edge_pair_lo_hi);

}
