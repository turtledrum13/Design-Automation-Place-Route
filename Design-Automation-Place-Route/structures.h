//
//  structures.h
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef structures_h
#define structures_h

struct cell
{
    int x;      //x-coordinate of lower left block
    int y;      //y-coordinate of lower left block
    int r;      //rotation of cell (1, 2, 3, or 4)
    int nets;   //number of nets attached to cell
    int cell;   //cell number (base 0)
};

#endif /* structures_h */
