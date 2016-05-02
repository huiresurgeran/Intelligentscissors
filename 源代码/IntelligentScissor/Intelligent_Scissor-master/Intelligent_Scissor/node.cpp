#include "stdafx.h"
#include"node.h"

void node::Print()
{
     FibHeapNode::Print();
     cout << total_cost;
}

void node::operator =(float NewKeyVal)
{
	node Temp;

    Temp.total_cost = total_cost = NewKeyVal;
    FHN_Assign(Temp);
}

void node::operator =(FibHeapNode& RHS)
{
     FHN_Assign(RHS);
	 for(int i = 0; i < 8; i++)
		 cost[i] = ((node&) RHS).cost[i];

	 //pre = ((node&) RHS).pre;
	 //pos = ((node&) RHS).pos; 
	 state = ((node&) RHS).state;
     total_cost = ((node&) RHS).total_cost;
}

int  node::operator ==(FibHeapNode& RHS)
{
     if (FHN_Cmp(RHS)) return 0;
     return total_cost == ((node&) RHS).total_cost ? 1 : 0;
}

int  node::operator <(FibHeapNode& RHS)
{
int X;

     if ((X=FHN_Cmp(RHS)) != 0)
	  return X < 0 ? 1 : 0;

     return total_cost < ((node&) RHS).total_cost ? 1 : 0;
};