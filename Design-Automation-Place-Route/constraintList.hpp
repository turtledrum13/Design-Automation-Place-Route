//
//  constraintList.hpp
//  Design-Automation-Place-Route
//
//  Created on 3/29/18.
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

    int findHead();

    bool notEmpty();

    bool isEmpty(int self);

    bool findVal(int val);

    std::vector<int> returnList();

    void removeAll(int num);

    bool cycleDetection(int num, std::vector<constraintList> &VCG, std::vector<bool> &visited, std::vector<int> &cycleList);
};

#endif /* constraintList_hpp */
