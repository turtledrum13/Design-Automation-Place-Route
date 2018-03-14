#include "dvalues.h"

dValues::dValues()
{
    dValue = 0;
    locked = false;
}

void dValues::add()
{
    if(!locked)
    {
        dValue++;
    }
}

void dValues::subtract()
{
    if(!locked)
    {
        dValue--;
    }
}

//lock the Dvalue so it cant be changed
void dValues::lock()
{
    locked = true;
}

void dValues::unlock()
{
    locked = false;
}

int dValues::getValue() const
{
    return dValue;
}
void dValues::setValue(int num)
{
    dValue = num;
}

//update the Dvalue based on the costs and lock it so it doesn't update if the swapped cell also has the Dvalue
void dValues::updateD(int weight, bool a, int b)
{
    if(!locked)
    {
        if (a)
        {
            dValue = dValue + 2*weight - 2*b;
        }
        else
        {
            dValue = dValue - 2*weight + 2*b;
        }
        locked = true;
    }
}
