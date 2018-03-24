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


void lees(std::vector<net> & globalNets, std::vector<cell> cellData)
{
    //Lee's Algorithm on one net at a time
    std::pair<int,int> source, destination;
    
    for(int i=0; i<globalNets.size(); i++)
    {
        //source = terminalCoords(globalNets[i].c1, cellData);
        //destination = terminalCoords(globalNets[i].c2, cellData);
        
        //wave(source, destination, layout)
    }
    
}

coord terminalCoords(std::pair<int,int> cell_term, std::vector<cell> cell_data)
{
    coord result;
    int cellNum = cell_term.first;
    int termNum = cell_term.second;
    
    result.x = cell_data[cellNum].x;
    result.y = cell_data[cellNum].y;
    
    switch(cell_data[cellNum].r)
    {
        case 1 :
            switch(termNum)
        {
            case 1 : result.x+=1; result.y-=5;
            case 2 : result.x+=4; result.y-=5;
            case 3 : result.x+=1; result.y-=0;
            case 4 : result.x+=4; result.y-=0;
        }
        case 2 :
            switch(termNum)
        {
            case 1 : result.x+=4; result.y-=5;
            case 2 : result.x+=4; result.y-=0;
            case 3 : result.x+=1; result.y-=5;
            case 4 : result.x+=1; result.y-=0;
        }
        case 3 :
            switch(termNum)
        {
            case 1 : result.x+=4; result.y-=0;
            case 2 : result.x+=1; result.y-=0;
            case 3 : result.x+=4; result.y-=5;
            case 4 : result.x+=1; result.y-=5;
        }
        case 4 :
            switch(termNum)
        {
            case 1 : result.x+=1; result.y-=0;
            case 2 : result.x+=1; result.y-=5;
            case 3 : result.x+=4; result.y-=0;
            case 4 : result.x+=4; result.y-=5;
        }
        default : result.x+=1; result.y-=5;
    }
}

void global()
{
    printf("\n\nDid global routing\n\n");
}
