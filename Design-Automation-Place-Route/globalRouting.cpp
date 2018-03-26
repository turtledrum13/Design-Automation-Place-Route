//
//  globalRouting.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "globalRouting.hpp"
#include "classifyNets.hpp"
#include "layoutOperations.hpp"
#include "structures.h"
#include <stdio.h>


void global(std::vector<net> & globalNets, std::vector<net> & channelNets, std::vector<net> & netlistPairs, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<int> boundaries,  std::vector<std::pair<int,int> > & channels, std::ofstream & file)
{
//    int size = netlistPairs.size();  //temporary until we fix looping through the newly added nets after the split
//    for(int i=0; i<size; i++)

    int i = 0;
    bool finished = false;
    while(not finished)
    {
        if(netlistPairs[i].global)
        {
            coord source = terminalCoords(netlistPairs[i].c1, cellData);
            coord destination = terminalCoords(netlistPairs[i].c2, cellData);
            bool topTerminal;    //true if the pin of the pass-through cell that source is connecting to is on top

            //update the layout and cellData vector with new pass through cells
            coord newCoord = findVertical(source, destination, layout, boundaries, topTerminal);
            updateLayout(newCoord, layout, cellData.size()+1);
            updateCellsX(cellData, newCoord);

            //adding pass-through cell to end of cellData
            cell newCell;
            newCell.r = 5;
            newCell.x = newCoord.x;
            newCell.y = newCoord.y;
            newCell.cell = cellData.size()+1;

            cellData.push_back(newCell);

            //net splitting
            std::pair<int,int> srcNet, destNet;
            srcNet.first = cellData.size()-1;
            destNet.first = cellData.size()-1;

            if(topTerminal)
            {
                srcNet.second = 1;
                destNet.second = 2;
            }
            else
            {
                srcNet.second = 2;
                destNet.second = 1;
            }

            netlistPairs.push_back(netlistPairs[i]);    //copy the current netlist of focus to the end of the list

            netlistPairs[i].c2 = srcNet;                //replace second cell with new pass through cell
            isGlobal(channels, netlistPairs[i], cellData);

            netlistPairs.back().c1 = destNet;           //replace first cell with new pass through cell
            isGlobal(channels, netlistPairs.back(), cellData);

        }

        //printf("\n<%i,%i> <%i,%i> global = %i",netlistPairs[i].c1.first+1,netlistPairs[i].c1.second,netlistPairs[i].c2.first+1,netlistPairs[i].c2.second, netlistPairs[i].global);

        i++;
        if(i == netlistPairs.size()) finished = true;// printOut(file, layout);}
    }
}

coord terminalCoords(std::pair<int,int> cell_term, std::vector<cell> cell_data)
{
    coord result;
    int cellNum = cell_term.first;
    //printf("\ncell num: %i  ",cellNum+1);

    int termNum = cell_term.second;
    //printf("term num: %i  \n",termNum);

    result.x = cell_data[cellNum].x;
    result.y = cell_data[cellNum].y;

    switch(cell_data[cellNum].r)
    {
    case 1 :
        switch(termNum)
        {
        case 1 :
            result.x+=1;
            result.y-=5;
            break;
        case 2 :
            result.x+=4;
            result.y-=5;
            break;
        case 3 :
            result.x+=1;
            result.y-=0;
            break;
        case 4 :
            result.x+=4;
            result.y-=0;
            break;
        }
        break;

    case 2 :
        switch(termNum)
        {
        case 1 :
            result.x+=4;
            result.y-=5;
            break;
        case 2 :
            result.x+=4;
            result.y-=0;
            break;
        case 3 :
            result.x+=1;
            result.y-=5;
            break;
        case 4 :
            result.x+=1;
            result.y-=0;
            break;
        }
        break;

    case 3 :
        switch(termNum)
        {
        case 1 :
            result.x+=4;
            result.y-=0;
            break;
        case 2 :
            result.x+=1;
            result.y-=0;
            break;
        case 3 :
            result.x+=4;
            result.y-=5;
            break;
        case 4 :
            result.x+=1;
            result.y-=5;
            break;
        }
        break;
    case 4 :
        switch(termNum)
        {
        case 1 :
            result.x+=1;
            result.y-=0;
            break;
        case 2 :
            result.x+=1;
            result.y-=5;
            break;
        case 3 :
            result.x+=4;
            result.y-=0;
            break;
        case 4 :
            result.x+=4;
            result.y-=5;
            break;
        }
        break;
    case 5 :
        switch(termNum)
        {
        case 1 :
            result.x+=1;
            result.y-=5;
            break;
        case 2 :
            result.x+=1;
            result.y-=0;
            break;
        }
        break;
    default : break;
    }

    return result;
}

coord findVertical(coord src, coord dest, std::vector<std::vector<int> > layout, std::vector<int> bound, bool & topTerm)
{
    bool up = dest.y < src.y;           //if the destination is above the source in the layout
    bool right = dest.x >= src.x;       //if the destination is to the right of the source in the layout
    bool top = false;

    for(int i=0; i<bound.size(); i++)   //determine if the boundary is on a top edge or bottom edge
    {
        if (src.y == bound[i])
        {
            if (i%2 != 0)
            {
                top = true;
                break;
            }
        }
    }

    //find where the x,y coordinate this net will terminate, conditional upon direction of desintation and boundary top/bottom
    coord result;

    //find the boundary row in which the partial net will terminate. This is the y-coordinate
    if(top)
    {
        if(up)
        {
            result.y = src.y-2;         //top edge & destination is above
            topTerm = false;
        }
        else
        {
            result.y = src.y+5;         //top edge & destination is below
            topTerm = true;
        }
    }
    else //if bottom
    {
        if(up)
        {
            result.y = src.y;           //bottom edge & destination is above
            topTerm = false;
        }
        else
        {
            result.y = src.y+7;         //bottom edge & destination is below
            topTerm = true;
        }
    }

    //search in the direction of the destination for a column in which the partial net can terminate. This is the x-coordinate
    if(right)
    {
        int index = src.x;
        while (true)
        {
            if(layout[result.y][index] < 1 )//|| layout[result.y][index] > 4)
            {
                result.x = index;
                break;
            }
            if(index<layout[result.y].size()-1) {index++;}
            else {result.x = index-5; break;}
        }
    }
    else
    {
        int index = src.x;
        while (true)
        {
            if(layout[result.y][index] < 1)//|| layout[result.y][index] > 4)
            {
                result.x = index;
                break;
            }

            if(index>0){index--;}
            else{result.x = 0; break;}
        }
    }

    //printf("top = %d and result = %i,%i\n", top, result.x, result.y);
    return result;
}


void updateCellsX(std::vector<cell> &cellData, coord XY)
{
    for (size_t i=0; i<cellData.size(); i++)
    {
        if (cellData[i].y==XY.y)
        {
            if(cellData[i].x >= XY.x)
            {
                cellData[i].x += 3;
            }
        }
    }
}


void updateLayout(coord XY, std::vector<std::vector<int> > &layout, int cellNum)
{
    //insert pass through
    for(int i=0; i<6; i++)
    {
        layout[XY.y-i].insert(layout[XY.y-i].begin()+XY.x,3, cellNum);
    }
    
    //check end of rows
    int expansion = 0;
    for(int i=1; i<4; i++)
    {
        std::vector<int> layoutRow = layout[XY.y];
        if(layoutRow[layoutRow.size()-i] != 0) expansion++;
    }

    //expand columns as much as necessary
    addCols(expansion, layout);
    
    //chop the nub
    for(int i=0; i<6; i++)
    {
        layout[XY.y-i].resize(layout[XY.y-i].size()-3);
    }
}


void printOut(std::ofstream & file, std::vector<std::vector<int> > & layout)
{
    //print CSV -- using to paste into spreadsheet for debugging
    for (int i=0; i<layout.size(); i++)
    {
        size_t index = layout[i].size();
        
        for(int j=0; j<index; j++)
        {
            file << layout[i][j] << ",";
        }
        file << "\n";
    }
}

