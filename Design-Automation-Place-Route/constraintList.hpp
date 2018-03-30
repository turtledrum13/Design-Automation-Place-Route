//
//  constraintList.hpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/29/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#ifndef constraintList_hpp
#define constraintList_hpp

#include <stdio.h>
#include <vector>

class constraintList
{
private:

    //creating a structure for the list

    struct graphNode
    {
        int value;        //the value in this node
        bool visited;      //whether the cell has been visited in the cycle detection search
        graphNode *next;    //to point to the next node
    };
    graphNode *head, *tail;    //list head and tail pointer

public:
    //constructor
    constraintList();

    //create a new node
    void appendNode(int num);

    void display();

    int lookUp(int num) const;

    int findHead();

    bool notEmpty();

    bool isEmpty(int self);

    bool findVal(int val);

    std::vector<int> returnList();

    void removeAll(int num);
};

#endif /* constraintList_hpp */
