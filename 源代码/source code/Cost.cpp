
#include "stdafx.h"
#include "Cost.h"


void cal_local_cost(Node** nodes, Mat& l_img)
{
	double RGBCost[3][8];
	double maxCost = 0;

	for(int i = 1; i < l_img.rows - 1; i++)
	{
		for(int j = 1; j < l_img.cols - 1; j++)
		{

			for(int k = 0; k < 3; k++)
			{
		
				RGBCost[k][0] = abs((l_img.at<Vec3b>(i-1,j)[k] + l_img.at<Vec3b>(i-1,j+1)[k]) / 2.0 - (l_img.at<Vec3b>(i+1,j)[k] + l_img.at<Vec3b>(i+1,j+1)[k]) / 2.0) / 2.0;
				RGBCost[k][4] = abs((l_img.at<Vec3b>(i-1,j-1)[k] + l_img.at<Vec3b>(i-1,j)[k]) / 2.0 - (l_img.at<Vec3b>(i+1,j-1)[k] + l_img.at<Vec3b>(i+1,j)[k]) / 2.0) / 2.0;

				RGBCost[k][2] = abs((l_img.at<Vec3b>(i-1,j-1)[k] + l_img.at<Vec3b>(i,j-1)[k]) / 2.0 - (l_img.at<Vec3b>(i-1,j+1)[k] + l_img.at<Vec3b>(i,j+1)[k]) / 2.0) / 2.0;
				RGBCost[k][6] = abs((l_img.at<Vec3b>(i,j-1)[k] + l_img.at<Vec3b>(i+1,j-1)[k]) / 2.0 - (l_img.at<Vec3b>(i,j+1)[k] + l_img.at<Vec3b>(i+1,j+1)[k]) / 2.0) / 2.0;

				RGBCost[k][1] = abs(l_img.at<Vec3b>(i,j + 1)[k] - l_img.at<Vec3b>(i-1,j)[k]) / sqrt(2.0);
				RGBCost[k][3] = abs(l_img.at<Vec3b>(i,j - 1)[k] - l_img.at<Vec3b>(i-1,j)[k]) / sqrt(2.0);
				RGBCost[k][5] = abs(l_img.at<Vec3b>(i,j - 1)[k] - l_img.at<Vec3b>(i+1,j)[k]) / sqrt(2.0);
				RGBCost[k][7] = abs(l_img.at<Vec3b>(i,j + 1)[k] - l_img.at<Vec3b>(i+1,j)[k]) / sqrt(2.0);
			}

		
			for(int k = 0; k < 8; k++)
			{
				nodes[i-1][j-1].cost[k] = sqrt((RGBCost[0][k] * RGBCost[0][k] + RGBCost[1][k] * RGBCost[1][k] + RGBCost[2][k] * RGBCost[2][k]) / 3.0);
				if(nodes[i-1][j-1].cost[k] > maxCost)
					maxCost = nodes[i-1][j-1].cost[k];
			}
		}
	}


	for(int i = 0; i < l_img.rows - 2; i++)
	{
		for(int j = 0; j < l_img.cols - 2; j++)
		{
			nodes[i][j].pos[0] = i;
			nodes[i][j].pos[1] = j;
			for(int k = 0; k < 8; k++)
			{
				if(k == 0 || k == 2 || k == 4 || k == 6)
					nodes[i][j].cost[k] = maxCost - nodes[i][j].cost[k];
				else
					nodes[i][j].cost[k] = (maxCost - nodes[i][j].cost[k]) * sqrt(2.0);
			}
		}
	}
}

int dir_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int dir_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};

void gen_path_tree(Node** nodes, Vec2i seed, int row, int col, vector<Node*>& node_list)
{

	FibHeap q;
	nodes[seed[1]][seed[0]].total_cost = 0;
	nodes[seed[1]][seed[0]].pre[0] = -1;
	nodes[seed[1]][seed[0]].pre[1] = -1;

	q.Insert(&nodes[seed[1]][seed[0]]);
	Node* tem;
	Node now; 

	Scalar gap(5, 5, 0);
	int count = 0;
	int x,y;
	node_list.clear();
	while((tem = (Node*) q.ExtractMin ()) != NULL)
	{


		if(nodes[tem->pos[0]][tem->pos[1]].state != 2)
		{
			nodes[tem->pos[0]][tem->pos[1]].state = 2;
			node_list.push_back(tem);
			for(int i = 0; i < 8; i++)
			{
				x = tem->pos[0] + dir_x[i];
				y = tem->pos[1] + dir_y[i];
				if(x >= 0 && x < row && y >=0 && y < col)
				{
					if(nodes[x][y].state != 2)
					{
						if(nodes[x][y].state == 0)
						{
							count++;
							nodes[x][y].pre[0] = tem->pos[0];
							nodes[x][y].pre[1] = tem->pos[1];
							nodes[x][y].total_cost = tem->total_cost + tem->cost[i];
							nodes[x][y].state = 1;
							nodes[x][y].yellow = tem->yellow + gap;
							q.Insert(&nodes[x][y]);
						}
						else if (nodes[x][y].state == 1 && nodes[x][y].total_cost > (tem->total_cost + tem->cost[i]))
						{
							count++;
							now = nodes[x][y];
							now.pre[0] = tem->pos[0];
							now.pre[1] = tem->pos[1];
							now.yellow = tem->yellow + gap;
							now.total_cost = tem->total_cost + tem->cost[i];
							q.DecreaseKey(&nodes[x][y],now);
						}
					}
				}
			}
		}
	}

}

Node** gen_new_tree(Node** nodes, int row, int col)
{

	Node** newNodes = new Node*[row];
	for(int i = 0; i < row; i++)
		newNodes[i] = new Node [col];

	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			for(int k = 0; k < 8; k++)
			{
				newNodes[i][j].cost[k] = nodes[i][j].cost[k];
			}
			newNodes[i][j].pos[0] = nodes[i][j].pos[0];
			newNodes[i][j].pos[1] = nodes[i][j].pos[1];
		}
	}

	return newNodes;
}