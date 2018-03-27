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
    std::pair<int,int> c1, c2;  //first and second cells connected by the net
    int num;                    //net number
    bool placed;                //whether the net has been placed yet or not
    bool global;                //true if net needs to be routed globally
    int channel;                //channel number if channel routing is possible
};

struct coord
{
    int x, y;
};

    struct chan
    {
        std::vector<int> top, bottom;
    };

#endif /* structures_h */
