#ifndef LIN_H
#define LIN_H
#include <vector>
#include "dvalues.h"

class numberList
{
private:

    //creating a structure for the list
    struct listNode
    {
        int value;        //the value in this node
        int weight;     //the edge weight of the node
        bool same;      //whether the cells in this node are in the same partition or not
        listNode *next;    //to point to the next node
    };
    listNode *head, *tail;    //list head and tail pointer

public:
    //constructor
    numberList();

    //create a new node
    void appendNode(int num);

    //if the node is already in the list, increment the weight and return true
    bool weightChange(int num);

    //use the linked list for one swapped cell and the value of the other swapped cell to calculate the new D value
    void updateDValue(std::vector<dValues> &D, const numberList &cellList, int bValue);

    //update whether the cells are in the same partition or not
    void updateSame(int a, bool var);

    //look up the other cell to get the weight to calulate new Dvalue
    int lookUp(int num) const;

    void display();

    //unlock all the D values for the chosen linked list
    void unlockD(std::vector<dValues> &D);

    int getWeight(int num);

    int findHead();

    bool notEmpty();
    
    bool isEmpty(int self);
    
    bool findVal(int val);
    
    std::vector<int> returnList();
    
    void removeAll(int num);

    bool isFree(int num);

};

#endif
