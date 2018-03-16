//
//  layoutOperations.hpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef layoutOperations_hpp
#define layoutOperations_hpp

#include <stdio.h>
#include <vector>

#include "structures.h"


void makeCell(cell C, std::vector<std::vector<int> > & layout);

void addRows(int numRows, std::vector<std::vector<int> > & layout);

void addCols(int numCols, std::vector<std::vector<int> > & layout);


#endif /* layoutOperations_hpp */
