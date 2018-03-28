#include <stdlib.h>
#include <iostream>
#include "lin.h"

numberList::numberList()
{
    head=NULL;
    tail=NULL;
}

void numberList::appendNode(int num)
{
    listNode *newNode;        //to point to a new node

    //allocate a new node and store the values there
    newNode = new listNode;
    newNode->value=num;
    newNode->weight=1;
    newNode->same=false;    //if the cells are in the same partition, this is first set to true
    newNode->next=NULL;

    //make newnode the 1st node if no nodes in list
    if(!head)
    {
        head=newNode;
        tail=newNode;
    }
    else                 //insert newnode at end
    {
        tail->next=newNode;     //make the newnode the tail
        tail=newNode;
    }
}

bool numberList::weightChange(int num)
{
    listNode *newNode=new listNode;
    newNode=head;

    while(newNode != NULL)
    {
        if (newNode->value == num)
        {
            newNode->weight++;
            return true;
        }
        newNode=newNode->next;
    }
    return false;
}


void numberList::updateDValue(std::vector<dValues> &D, const numberList &cellList, int bValue)
{
    listNode *newNode=new listNode;
    newNode=head;

    while(newNode!=NULL)
    {

        if(newNode->value == bValue)
        {
            D[newNode->value].updateD(newNode->weight, newNode->same, 0);
        }
        else
        {
            D[newNode->value].updateD(newNode->weight, newNode->same, cellList.lookUp(newNode->value));
        }
        newNode=newNode->next;
    }
}

void numberList::updateSame(int a, bool var)
{
    listNode *newNode=new listNode;
    newNode = head;

    while(newNode!= NULL)
    {
        if (newNode->value == a)
        {
            newNode->same = var;
        }
        newNode=newNode->next;
    }

}

//look up the other cell to get the weight to calulate new Dvalue
int numberList::lookUp(int num) const
{
    listNode *newNode=new listNode;
    newNode=head;

    while(newNode!=NULL)
    {
        if (newNode->value==num)
        {
            return newNode->weight;
        }
        else
        {
            newNode=newNode->next;
        }
    }
    return 0;
}

void numberList::display()
{
    listNode *newNode=new listNode;
    newNode=head;

    while(newNode!=NULL)
    {
        std::cout<<newNode->value<<"--> ";
        newNode=newNode->next;
    }
}

//unlock all the D values for the chosen linked list
void numberList::unlockD(std::vector<dValues> &D)
{
    listNode *newNode = new listNode;
    newNode = head;

    while(newNode!=NULL)
    {
        D[newNode->value].unlock();
        newNode=newNode->next;
    }
}

int numberList::getWeight(int num)
{
    listNode *newNode = new listNode;
    newNode = head;

    while(newNode!=NULL)
    {
        if(newNode->value==num)
        {
            return newNode->weight;
        }
        newNode=newNode->next;
    }
    return 0;
}

int numberList::findHead()
{
    listNode *newNode = new listNode;
    newNode = head;

    return newNode->value;
}

bool numberList::notEmpty()
{
    listNode *newNode = new listNode;
    newNode = head;

    if (newNode==NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool numberList::isFree(int num)
{
    listNode *newNode = new listNode;
    newNode = head;

    while(newNode!=NULL)
    {
        if(newNode->value==num)
        {
            return false;
        }
        else
        {
                newNode=newNode->next;
        }

        return true;
    }

}
