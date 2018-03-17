//
//  classifyNets.hpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/16/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef classifyNets_hpp
#define classifyNets_hpp

#include <stdio.h>
#include <vector>
#include "structures.h"

std::vector<bool> findBoundaries(std::vector<cell> cells, std::vector<std::vector<int> > & layout);

#endif /* classifyNets_hpp */
