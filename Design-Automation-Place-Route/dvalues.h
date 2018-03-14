#ifndef DVALUES_H
#define DVALUES_H

class dValues
{
private:
    int dValue;
    bool locked;

public:
    dValues();
    void add();
    void subtract();
    void lock();
    void unlock();
    int getValue() const;
    void setValue(int num);
    void updateD(int weight, bool a, int b);
};
 #endif
