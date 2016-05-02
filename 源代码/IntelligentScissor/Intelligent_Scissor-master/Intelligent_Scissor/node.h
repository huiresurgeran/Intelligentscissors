#ifndef NODE_H
#define NODE_H

//---------------------------------------------------
// This header file defines the data structure node, 
// which we used in cost graphy
//----------------------------------------------------

#include<iostream>
#include<vector>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<math.h>
#include"fibheap.h"
using namespace cv;
using namespace std;

class node: public FibHeapNode
{
	public:
	node()
	{
		for(int i = 0; i < 8; i++)
			cost[i] = 0;
		state = 0;
		total_cost = 0;
		yellow = Scalar(118, 118, 0);
	}
	double cost[8];
	int state;
	double total_cost;
	Vec2i pre;
	Vec2i pos;
	Scalar yellow;
	virtual void operator =(FibHeapNode& RHS);
	virtual int  operator ==(FibHeapNode& RHS);
    virtual int  operator <(FibHeapNode& RHS);

    virtual void operator =(float NewKeyVal);
    virtual void Print();
    double GetKeyValue() { return total_cost; };
	void SetKeyValue(float inkey) { total_cost = inkey; };
};
#endif