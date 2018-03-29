#include <stdlib.h>
#include <iostream>
#include "lin.h"
#include <stdio.h>

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
    listNode *newNode= new listNode;
    newNode=head;

    if(newNode == NULL)
    {
        std::cout<<"[empty]";
    }

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

bool numberList::isEmpty(int self)
{
    listNode *newNode = new listNode;
    newNode = head;

    while(newNode != NULL)
    {
        if(newNode->value != self)
        {
            printf("\n\nfound something!!! %i - %i\n", self, newNode->value);
            return false;
        }
        else
        {
            newNode = newNode->next;
        }
    }
    return true;
//    if (newNode==NULL)
//    {
//        return true;
//    }
//    else
//    {
//        return false; //change back to false after implementing dogleg
//    }
}

bool numberList::findVal(int val)
{
    listNode *newNode = new listNode;
    newNode = head;

    while(newNode!=NULL)
    {
        if (newNode->value==val)
        {
            return true;
        }
        else
        {
            newNode=newNode->next;
        }
    }
    return false;
}

std::vector<int> numberList::returnList()
{
    listNode *newNode = new listNode;
    newNode = head;
    std::vector<int> list;

    while(newNode!=NULL)
    {
        list.push_back(newNode->value);
        newNode = newNode->next;
    }

    return list;
}

void numberList::removeAll(int num)
{
    listNode *currentNode = head;
    listNode *previousNode = NULL;

    while(currentNode!=NULL)
    {
        if(currentNode->value == num)
        {
            if(currentNode == head)
            {
                head = head->next;
                free(currentNode);
                currentNode = head;
            }
            else
            {
                previousNode->next = currentNode->next;
                free(currentNode);
                currentNode = previousNode->next;
            }
        }
        else
        {
            previousNode = currentNode;
            currentNode = currentNode->next;
        }
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
    }
    return true;
}

