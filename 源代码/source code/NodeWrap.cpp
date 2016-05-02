
#include "stdafx.h"
#include "NodeWrap.h"

void NodeWrap::Print()
{
     FibHeapNode::Print();
     cout << totalCost;
}

void NodeWrap::operator =(float NewKeyVal)
{
	NodeWrap Temp;

    Temp.totalCost = totalCost = NewKeyVal;
    FHN_Assign(Temp);
}

void NodeWrap::operator =(FibHeapNode& RHS)
{
     FHN_Assign(RHS);
	 for(int i = 0; i < 8; i++)
		 cost[i] = ((NodeWrap&) RHS).cost[i];

	 state = ((NodeWrap&) RHS).state;
     totalCost = ((NodeWrap&) RHS).totalCost;
}

int  NodeWrap::operator ==(FibHeapNode& RHS)
{
     if (FHN_Cmp(RHS)) return 0;
     return totalCost == ((NodeWrap&) RHS).totalCost ? 1 : 0;
}

int  NodeWrap::operator <(FibHeapNode& RHS)
{
    int X;

     if ((X=FHN_Cmp(RHS)) != 0)
	  return X < 0 ? 1 : 0;

     return totalCost < ((NodeWrap&) RHS).totalCost ? 1 : 0;
};