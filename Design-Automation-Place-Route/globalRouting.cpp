//
//  globalRouting.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "globalRouting.hpp"
#include "structures.h"
#include <stdio.h>


void lees(std::vector<net> & globalNets, std::vector<cell> cellData, std::vector<std::vector<int> > layout, std::vector<int> boundaries)
{
    //Lee's Algorithm on one net at a time
    coord source, destination;

    for(int i=0; i<globalNets.size(); i++)
    {
        source = terminalCoords(globalNets[i].c1, cellData);
        destination = terminalCoords(globalNets[i].c2, cellData);
        
        findVertical(source, destination, layout, boundaries);
    }

}

coord terminalCoords(std::pair<int,int> cell_term, std::vector<cell> cell_data)
{
    coord result;
    int cellNum = cell_term.first;
    printf("cell num: %i  ",cellNum);
    int termNum = cell_term.second;
    printf("term num: %i  ",termNum);

    result.x = cell_data[cellNum-1].x;
    result.y = cell_data[cellNum-1].y;

    switch(cell_data[cellNum-1].r)
    {
    case 1 :
        switch(termNum)
        {
        case 1 :
            result.x+=1;
            result.y-=5;
        case 2 :
            result.x+=4;
            result.y-=5;
        case 3 :
            result.x+=1;
            result.y-=0;
        case 4 :
            result.x+=4;
            result.y-=0;
        }
    case 2 :
        switch(termNum)
        {
        case 1 :
            result.x+=4;
            result.y-=5;
        case 2 :
            result.x+=4;
            result.y-=0;
        case 3 :
            result.x+=1;
            result.y-=5;
        case 4 :
            result.x+=1;
            result.y-=0;
        }
    case 3 :
        switch(termNum)
        {
        case 1 :
            result.x+=4;
            result.y-=0;
        case 2 :
            result.x+=1;
            result.y-=0;
        case 3 :
            result.x+=4;
            result.y-=5;
        case 4 :
            result.x+=1;
            result.y-=5;
        }
    case 4 :
        switch(termNum)
        {
        case 1 :
            result.x+=1;
            result.y-=0;
        case 2 :
            result.x+=1;
            result.y-=5;
        case 3 :
            result.x+=4;
            result.y-=0;
        case 4 :
            result.x+=4;
            result.y-=5;
        }
    default :
        result.x+=1;
        result.y-=5;
    }

    return result;
}

coord findVertical(coord src, coord dest, std::vector<std::vector<int> > layout, std::vector<int> bound)
{
    bool up = dest.y < src.y;           //if the destination is above the source in the layout
    bool right = dest.x >= src.x;        //if the destination is to the right of the source in the layout
    bool top;
    
    for(int i=0; i<bound.size(); i++)
    {
        if (src.y == bound[i]) top = i; break;
    }

    //find where the x,y coordinate this net will terminate at, conditional upon direction of desintation and boundary top/bottom
    coord result;
    
    //find the boundary row in which the partial net will terminate. This is the y-coordinate
    if(top)
    {
        if(not up) result.y = src.y+5;        //top edge & destination is below
        else result.y = src.y-2;            //top edge & destination is above
    }
    else
    {
        if(up) result.y = src.y;            //bottom edge & destination is above
        else result.y = src.y+2;            //bottom edge & destination is below
    }
    
    //search in the direction of the destination for a column in which the partial net can terminate. This is the x-coordinate
    if(right)
    {
        int index = src.x;
        while (true)
        {
            if(layout[result.y][index] < 1 || layout[result.y][index] > 4)
            {
                result.x = index;
                break;
            }
            index++;
        }
    }
    else
    {
        int index = src.x;
        while (true)
        {
            if(layout[result.y][index] < 1 || layout[result.y][index] > 4)
            {
                result.x = index;
                break;
            }
            index--;
        }
    }

    return result;
}


void global()
{
    printf("\n\nDid global routing\n\n");
}

void updateCells(std::vector<cell> &cellData, int numX, int numY)
{
    for (int i=0; i<cellData.size(); i++)
    {
        if (cellData[i].y==numY)
        {
            if(cellData[i].x > numX)
            {
                cellData[i].x += 3;
            }
        }
    }
}

void updateLayout(int numX, int numY, std::vector<std::vector<int> > &layout)
{
    //std::vector<int>::iterator it;
    //it = layout[numX][numY];
    for(int i=0; i<6; i++)
    {
        layout[numX+i].insert(layout[numX+i].begin()+numY,3, 0);
    }
}
