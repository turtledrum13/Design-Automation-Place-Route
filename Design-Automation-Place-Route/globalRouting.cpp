//
//  globalRouting.cpp
//  Design-Automation-Place-Route
//
//  Created on 3/16/18.
//

#include "globalRouting.hpp"
#include "classifyNets.hpp"
#include "layoutOperations.hpp"
#include "structures.h"
#include <stdio.h>
#include <string>


void global(std::vector<net> & netlistPairs, std::vector<cell> & cellData, std::vector<std::vector<int> > & layout, std::vector<int> boundaries,  std::vector<std::pair<int,int> > & channels)
{
    //for each net in the netlist
    for(size_t i=0; i<netlistPairs.size(); i++)
    {
        if(netlistPairs[i].global)
        {
            coord source = terminalCoords(netlistPairs[i].src, cellData);
            coord destination = terminalCoords(netlistPairs[i].dest, cellData);
            bool topTerminal;    //true if the pin of the pass-through cell that source is connecting to is on top

            //update the layout and cellData vector with new pass through cells
            coord newCoord = findVertical(source, destination, layout, boundaries, topTerminal);
            updateLayout(newCoord, layout, "feed-through");
            updateRight(cellData, newCoord, "feed-through");

            //adding pass-through cell to end of cellData
            int cellNum = cellData.size()+1;
            cell newCell = {.r = 5, .x = newCoord.x, .y = newCoord.y, .cell = cellNum};
            cellData.push_back(newCell);

            //net splitting
            std::pair<int,int> oldDest, newSrc;
            oldDest.first = cellData.size()-1;
            newSrc.first = cellData.size()-1;

            if(topTerminal)
            {
                oldDest.second = 1;
                newSrc.second = 2;
            }
            else
            {
                oldDest.second = 2;
                newSrc.second = 1;
            }

            netlistPairs.push_back(netlistPairs[i]);            //copy the current netlist of focus to the end of the list

            netlistPairs[i].dest = oldDest;                     //replace second cell with new pass through cell
            isGlobal(channels, netlistPairs[i], cellData);

            netlistPairs.back().src = newSrc;                   //replace first cell with new pass through cell
            netlistPairs.back().num = cellData.size();          //assign new net number to the new net segment
            isGlobal(channels, netlistPairs.back(), cellData);
        }
    }

    for(size_t i=0; i<cellData.size(); i++)                     //put the lower left corners of layout cells back to normal
    {
        layout[cellData[i].y][cellData[i].x] = cellData[i].r;
    }
    
    for(size_t i=0; i<channels.size()-1; i++)
    {
        int counter = 0;
        int rowNum = channels[i].second;
        for(int j=0; j<layout[rowNum].size(); j++)
        {
            if(layout[rowNum][j] > 0 && layout[rowNum][j] < 4) counter++;
            else counter = 0;
            
            if(counter == 7)
            {
                coord newCoord(j, rowNum);
                updateLayout(newCoord, layout, "space");
                updateRight(cellData, newCoord, "space");
                counter = 0;
            }
        }
    }

}   //Finished with global routing///////////////////////////////////////



coord terminalCoords(std::pair<int,int> cell_term, std::vector<cell> cell_data)
{
    coord result(0,0);
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
            case 1 : result.adjust(1,-5); break;
            case 2 : result.adjust(4, -5); break;
            case 3 : result.adjust(1,0); break;
            case 4 : result.adjust(4,0); break;
            default : break;
        }
        break;

    case 2 :
        switch(termNum)
        {
            case 1 : result.adjust(4,-5); break;
            case 2 : result.adjust(4, 0); break;
            case 3 : result.adjust(1,-5); break;
            case 4 : result.adjust(1,0); break;
            default : break;
        }
        break;

    case 3 :
        switch(termNum)
        {
            case 1 : result.adjust(4,0); break;
            case 2 : result.adjust(1, 0); break;
            case 3 : result.adjust(4,-5); break;
            case 4 : result.adjust(1,-5); break;
            default : break;
        }
        break;
    case 4 :
        switch(termNum)
        {
            case 1 : result.adjust(1,0); break;
            case 2 : result.adjust(1, -5); break;
            case 3 : result.adjust(4,0); break;
            case 4 : result.adjust(4,-5); break;
            default : break;
        }
        break;
    case 5 :
        switch(termNum)
        {
            case 1 : result.adjust(1,-5); break;
            case 2 : result.adjust(1, 0); break;
            default : break;
        }
        break;
    case 6 :
        switch(termNum)
    {
        case 1 : result.adjust(1,0); break;
        case 2 : result.adjust(1, -5); break;
        default : break;
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

    for(int i=1; i<bound.size(); i++)   //determine if the boundary is on a top cell edge or bottom cell edge
    {
        if (src.y == bound[i])
        {
            if (i%2 == 0) top = true;
            else top = false;
            break;
        }
    }

    //find where the x,y coordinate this net will terminate, conditional upon direction of desintation and boundary top/bottom
    coord result(0,0);

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
    if(right) //search to the right
    {
        int index = src.x;
        while (true)
        {
            if(layout[result.y][index] < 1 && layout[result.y][index-1] != 0)
            {
                result.x = index;
                break;
            }

            if(index<layout[result.y].size()-1) index++;
            else {result.x = index-5; break;}
        }
    }
    else //search to the left
    {
        int index = src.x;
        while (true)
        {
            if(layout[result.y][index] > 0 && layout[result.y][index+1] == 0)
            {
                result.x = index+1;
                break;
            }

            if(index>0) index--;
            else{result.x = 0; break;}
        }
    }

    //printf("top = %d and result = %i,%i\n", top, result.x, result.y);
    return result;
}


void updateRight(std::vector<cell> &cellData, coord XY, std::string updateSetting)
{
    int width = 0;
    
    if(updateSetting == "feed-through") width = 3;
    else if(updateSetting == "space") width = 1;
    
    
    for (size_t i=0; i<cellData.size(); i++)
    {
        if (cellData[i].y==XY.y)
        {
            if(cellData[i].x >= XY.x)
            {
                cellData[i].x += width;
            }
        }
    }
}


void updateLayout(coord XY, std::vector<std::vector<int> > &layout, std::string updateSetting)
{
    int insertionVal = 0;
    int width = 0;
    
    if(updateSetting == "feed-through")
    {
        insertionVal = 5;
        width = 3;
    }
    else if(updateSetting == "space")
    {
        insertionVal = 0;
        width = 1;
    }
    
    
    //insert new element
    for(int i=0; i<6; i++)
    {
        layout[XY.y-i].insert(layout[XY.y-i].begin()+XY.x,width, insertionVal);
    }

    //check end of rows
    int expansion = 0;
    for(int i=1; i<width+1; i++)
    {
        std::vector<int> layoutRow = layout[XY.y];
        if(layoutRow[layoutRow.size()-i] != 0) expansion++;
    }

    //expand columns as much as necessary
    appendCols(expansion, layout);

    //chop the nub
    for(int i=0; i<6; i++)
    {
        layout[XY.y-i].resize(layout[XY.y-i].size()-width);
    }
}
