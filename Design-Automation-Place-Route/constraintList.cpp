//
//  constraintList.cpp
//  Design-Automation-Place-Route
//
//  Created by Aaron J Cunningham on 3/29/18.
//  Copyright © 2018 Aaron Cunningham. All rights reserved.
//

#include "constraintList.hpp"

#include <stdlib.h>
#include <iostream>

constraintList::constraintList()
{
    head=NULL;
    tail=NULL;
}

void constraintList::appendNode(int num)
{
    graphNode *newNode;        //to point to a new node

    //allocate a new node and store the values there
    newNode = new graphNode;
    newNode->value=num;
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

void constraintList::display()
{
    graphNode *newNode= new graphNode;
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

int constraintList::lookUp(int num) const
{
    graphNode *newNode=new graphNode;
    newNode=head;

    while(newNode!=NULL)
    {
        if (newNode->value==num)
        {
            return 1;
        }
        else
        {
            newNode=newNode->next;
        }
    }
    return 0;
}


int constraintList::findHead()
{
    graphNode *newNode = new graphNode;
    newNode = head;

    return newNode->value;
}

bool constraintList::notEmpty()
{
    graphNode *newNode = new graphNode;
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

bool constraintList::isEmpty(int self)
{
    graphNode *newNode = new graphNode;
    newNode = head;

    while(newNode != NULL)
    {
        if(newNode->value != self)
        {
            //printf("\n\nfound something!!! %i - %i\n", self, newNode->value);
            return false;
        }
        else
        {
            newNode = newNode->next;
        }
    }
    return true;
}

bool constraintList::findVal(int val)
{
    graphNode *newNode = new graphNode;
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

std::vector<int> constraintList::returnList()
{
    graphNode *newNode = new graphNode;
    newNode = head;
    std::vector<int> list;

    while(newNode!=NULL)
    {
        list.push_back(newNode->value);
        newNode = newNode->next;
    }

    return list;
}

void constraintList::removeAll(int num)
{
    graphNode *currentNode = head;
    graphNode *previousNode = NULL;

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

