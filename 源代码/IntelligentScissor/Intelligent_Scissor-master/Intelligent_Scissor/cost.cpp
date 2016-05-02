#include "stdafx.h"
#include"cost.h"


void cal_cost_map(node** cost_map, Mat& l_img)
{
	double RGB_cost[3][8];
	double max_cost = 0;
	//calculate the D(link)
	for(int i = 1; i < l_img.rows - 1; i++)
	{
		for(int j = 1; j < l_img.cols - 1; j++)
		{
			//calculate D(link) in R, G, and B
			for(int k = 0; k < 3; k++)
			{
				//horizontal 
				RGB_cost[k][0] = abs((l_img.at<Vec3b>(i-1,j)[k] + l_img.at<Vec3b>(i-1,j+1)[k]) / 2.0 - (l_img.at<Vec3b>(i+1,j)[k] + l_img.at<Vec3b>(i+1,j+1)[k]) / 2.0) / 2.0;
				RGB_cost[k][4] = abs((l_img.at<Vec3b>(i-1,j-1)[k] + l_img.at<Vec3b>(i-1,j)[k]) / 2.0 - (l_img.at<Vec3b>(i+1,j-1)[k] + l_img.at<Vec3b>(i+1,j)[k]) / 2.0) / 2.0;
				//vectical 
				RGB_cost[k][2] = abs((l_img.at<Vec3b>(i-1,j-1)[k] + l_img.at<Vec3b>(i,j-1)[k]) / 2.0 - (l_img.at<Vec3b>(i-1,j+1)[k] + l_img.at<Vec3b>(i,j+1)[k]) / 2.0) / 2.0;
				RGB_cost[k][6] = abs((l_img.at<Vec3b>(i,j-1)[k] + l_img.at<Vec3b>(i+1,j-1)[k]) / 2.0 - (l_img.at<Vec3b>(i,j+1)[k] + l_img.at<Vec3b>(i+1,j+1)[k]) / 2.0) / 2.0;
				//diagonal
				RGB_cost[k][1] = abs(l_img.at<Vec3b>(i,j + 1)[k] - l_img.at<Vec3b>(i-1,j)[k]) / sqrt(2.0);
				RGB_cost[k][3] = abs(l_img.at<Vec3b>(i,j - 1)[k] - l_img.at<Vec3b>(i-1,j)[k]) / sqrt(2.0);
				RGB_cost[k][5] = abs(l_img.at<Vec3b>(i,j - 1)[k] - l_img.at<Vec3b>(i+1,j)[k]) / sqrt(2.0);
				RGB_cost[k][7] = abs(l_img.at<Vec3b>(i,j + 1)[k] - l_img.at<Vec3b>(i+1,j)[k]) / sqrt(2.0);
			}

			//combine the RGB Dlink to D(link) and update the maxD
			for(int k = 0; k < 8; k++)
			{
				cost_map[i-1][j-1].cost[k] = sqrt((RGB_cost[0][k] * RGB_cost[0][k] + RGB_cost[1][k] * RGB_cost[1][k] + RGB_cost[2][k] * RGB_cost[2][k]) / 3.0);
				if(cost_map[i-1][j-1].cost[k] > max_cost)
					max_cost = cost_map[i-1][j-1].cost[k];
			}
		}
	}

	//calcualte the cost in 8 directions
	for(int i = 0; i < l_img.rows - 2; i++)
	{
		for(int j = 0; j < l_img.cols - 2; j++)
		{
			cost_map[i][j].pos[0] = i;
			cost_map[i][j].pos[1] = j;
			for(int k = 0; k < 8; k++)
			{
				if(k == 0 || k == 2 || k == 4 || k == 6)
					cost_map[i][j].cost[k] = max_cost - cost_map[i][j].cost[k];
				else
					cost_map[i][j].cost[k] = (max_cost - cost_map[i][j].cost[k]) * sqrt(2.0);
			}
		}
	}
}

int dir_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int dir_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};

void generate_tree(node** cost_map, Vec2i seed, int row, int col, vector<node*>& node_list)
{
	//priority_queue<node,vector<node>, cmp_node>  q;
	FibHeap q;
	cost_map[seed[1]][seed[0]].total_cost = 0;
	cost_map[seed[1]][seed[0]].pre[0] = -1;
	cost_map[seed[1]][seed[0]].pre[1] = -1;
	//cout<<seed[1]<<" "<<seed[0]<<endl;
	q.Insert(&cost_map[seed[1]][seed[0]]);
	node* tem;
	node now; 

	Scalar gap(5, 5, 0);
	int count = 0;
	int x,y;
	node_list.clear();
	while((tem = (node*) q.ExtractMin ()) != NULL)
	{
		//cout<<tem.state<<endl;

		if(cost_map[tem->pos[0]][tem->pos[1]].state != 2)
		{
			cost_map[tem->pos[0]][tem->pos[1]].state = 2;
			node_list.push_back(tem);
			for(int i = 0; i < 8; i++)
			{
				x = tem->pos[0] + dir_x[i];
				y = tem->pos[1] + dir_y[i];
				if(x >= 0 && x < row && y >=0 && y < col)
				{
					if(cost_map[x][y].state != 2)
					{
						if(cost_map[x][y].state == 0)
						{
							count++;
							//update the predecessor
							cost_map[x][y].pre[0] = tem->pos[0];
							cost_map[x][y].pre[1] = tem->pos[1];
							//update the cost
							cost_map[x][y].total_cost = tem->total_cost + tem->cost[i];
							//update state
							cost_map[x][y].state = 1;
							//update yellow color
							cost_map[x][y].yellow = tem->yellow + gap;
							//push to the queue
							q.Insert(&cost_map[x][y]);
						}
						else if (cost_map[x][y].state == 1 && cost_map[x][y].total_cost > (tem->total_cost + tem->cost[i]))
						{
							count++;
							now = cost_map[x][y];
							now.pre[0] = tem->pos[0];
							now.pre[1] = tem->pos[1];
							now.yellow = tem->yellow + gap;
							//update the cost
							now.total_cost = tem->total_cost + tem->cost[i];
							q.DecreaseKey(&cost_map[x][y],now);
						}
					}
				}
			}
		}
	}

	cout << "size: " << node_list.size() << endl;
	cout<<row<<" "<<col<<" "<< count<<" ";
}

node** new_tree(node** cost_map, int row, int col)
{
	/*for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			cost_map[i][j].state = 0;
		}
	}*/

	node** new_cost_map = new node*[row];
	for(int i = 0; i < row; i++)
		new_cost_map[i] = new node [col];

	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			for(int k = 0; k < 8; k++)
			{
				new_cost_map[i][j].cost[k] = cost_map[i][j].cost[k];
			}
			new_cost_map[i][j].pos[0] = cost_map[i][j].pos[0];
			new_cost_map[i][j].pos[1] = cost_map[i][j].pos[1];
		}
	}

	return new_cost_map;
}