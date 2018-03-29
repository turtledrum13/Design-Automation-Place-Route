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
    int channel;                    //channel number if channel routing is possible
    int x1, x2, xSrc, xDest, y;     //beginning column and end column of the net's trunk, row of trunk
    
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
    std::vector<int> top, bottom, netPointer;
    size_t width;
    int numNets;

    chan(size_t chanWidth, int initVal)
    {
        top.resize(chanWidth, initVal);
        bottom.resize(chanWidth, initVal);
        width = chanWidth;
    }
};

struct netPos
{
  int x, y, net, track;
  bool placed;
};

#endif /* structures_h */
