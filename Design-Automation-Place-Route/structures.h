//
//  structures.h
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef structures_h
#define structures_h

#include <stdio.h>
#include <iostream>
#include <vector>

struct cell
{
    int x;                      //x-coordinate of lower left block
    int y;                      //y-coordinate of lower left block
    int r;                      //rotation of cell (1, 2, 3, or 4, or 5 for through-cell)
    int nets;                   //number of nets attached to cell
    int cell;                   //cell number (base 0)
    std::vector<int> netIndex;  //indeces in the netPairs array where up to four nets could exist
};

struct net
{
    std::pair<int,int> src, dest;   //first and second cells connected by the net <cell,term>
    int num;                        //net number
    bool routed;                    //whether the net has been routed yet or not
    bool global;                    //true if net needs to be routed globally
    bool dogleg;                    //specifies whether net is a dogleg or not
    int channel;                    //channel number if channel routing is possible
    int x1, x2, y;                  //beginning column and end column of the net's trunk, row of trunk
    int xSrc, xDest;                //x-positions of Source/Destination Cell terminals
    int span;                       //number of columns covered by the horizontal net component
    
    void setSpan(int srcX, int destX)
    {
        xSrc = srcX;
        xDest = destX;
        
        if(srcX <= destX)
        {
            x1 = srcX;
            x2 = destX;
        }
        else
        {
            x1 = destX;
            x2 = srcX;
        }
        
        span = x2-x1;
    }
};

struct coord
{
    int x, y;

    coord(int X, int Y)
    {
        x = X;
        y = Y;
    }
    void adjust(int adjX, int adjY)
    {
        x += adjX;
        y += adjY;
    }
};

struct chan
{
    std::vector<int> top, bottom;   //connectivity vectors holding ID-numbers of the nets
    size_t width;                   //total number of columns in the channel
    int numNets;                    //number of full and partial nets in the channel
    std::vector<net *> nets;        //pointers to the nets in netlistPairs contained in channel in ID-number order

    chan(size_t chanWidth, int initVal)
    {
        top.resize(chanWidth, initVal);
        bottom.resize(chanWidth, initVal);
        width = chanWidth;
    }
};

#endif /* structures_h */
