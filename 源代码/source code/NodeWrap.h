
#ifndef _NODEWRAP_H
#define _NODEWRAP_H

#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "FibHeap.h"

using namespace cv;
using namespace std;

class NodeWrap: public FibHeapNode
{

public:
	NodeWrap(){
		for(int i = 0; i < 8; i++)
			cost[i] = 0;
		state = 0;
		totalCost = 0;
		color = Scalar(118, 118, 0);
	}

	double cost[8];
	int state;
	double totalCost;
	Point pre;
	Point pos;
	Scalar color;

	virtual void operator =(FibHeapNode& RHS);
	virtual int  operator ==(FibHeapNode& RHS);
    virtual int  operator <(FibHeapNode& RHS);

    virtual void operator =(float NewKeyVal);
    virtual void Print();
    double get_key_value() { return totalCost; };
	void set_key_value(float inkey) { totalCost = inkey; };
};

typedef NodeWrap Node;

#endif