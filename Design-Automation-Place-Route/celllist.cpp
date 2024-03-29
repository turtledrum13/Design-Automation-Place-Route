#include <vector>
#include <stdlib.h>
#include <algorithm>
#include "celllist.h"
#include "lin.h"

void createCellList(int numOfNets, std::vector<std::vector<int> > &netArray, std::vector<numberList> &cellList,
                    std::vector<std::pair<int,int> > &netlist, int numOfCells)
{
    //variabe for future use
    int a = 0;

    //resize the cellList, accounting for the dummy cells that will also be used
    cellList.resize(numOfCells+1);

    //resize the array of nets
    netArray.resize(2, std::vector<int>(numOfNets, 0));

    for (int i = 0; i<numOfNets; i++)
    {
        for (int n= 0; n<2; n++)
        {
            netArray[n][i] = netlist[a].first;
            ++a;
        }
    }

    //create an array of linked lists for the cells. each list contains the cells that each cell shares a net with
    for (int i = 0; i<numOfNets; i++)
    {
        if (!cellList[netArray[0][i]].weightChange(netArray[1][i]))
        {
            cellList[netArray[0][i]].appendNode(netArray[1][i]);
        }

        if (!cellList[netArray[1][i]].weightChange(netArray[0][i]))
        {
            cellList[netArray[1][i]].appendNode(netArray[0][i]);
        }
    }
}

//a function used to sort the D values
bool sortDValue(const std::vector<int> &vect1, const std::vector<int> &vect2)
{
    if(vect1.size() > 0 && vect2.size() > 0)
    {
        return vect1[0] > vect2[0];
    }
    return false;
}
