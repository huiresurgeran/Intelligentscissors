#include "stdafx.h"
#include "my_draw.h"
#include <iostream>


void DrawPathTree(Mat& tree_graph, Mat& cost_graph, int node_num, 
				  node** cost_map, Vec2i seed,  
				  int node_map_row, int node_map_col,vector<node*> node_list) {

	cost_graph.copyTo(tree_graph);
	Vec3b yellow[10] = {
		Vec3b(0, 51, 51),
		Vec3b(0, 102, 102),
		Vec3b(0, 128, 128),
		Vec3b(0, 153, 153),
		Vec3b(0, 204, 204),
		Vec3b(0, 255, 255),
		Vec3b(51, 255, 255),
		Vec3b(102, 255, 255),
		Vec3b(153, 255, 255),
		Vec3b(204, 255, 255)};

	int counter = 0, threshold = 10;
	int partial = node_num / threshold;
	double node_x = 0, node_y = 0;

	for (int i = 0; i < node_list.size() && i < node_num; i++) {
		node_x = node_list[i]->pos[0];
		node_y = node_list[i]->pos[1];
		node_x = 3*node_x + 1;
		node_y = 3*node_y + 1;
		counter = i / partial; 
		tree_graph.at<Vec3b>(node_x, node_y) = yellow[counter];
	}
	
}

void draw_circle(Mat& image, Point x,double alpha, bool flag, Vec2d scale)
{
	int thickness = -1;
	int lineType = 8;
	int radius = 10;
	Scalar color;
	if(flag)
		color = Scalar(0,0,255);
	else
		color = Scalar(0,255,0);

	Mat img(image.rows, image.cols, CV_8UC3, Scalar(0,0,0));
	circle(img, x * scale[0] * scale[1], radius * scale[1] , color, thickness, lineType);

	for(int i = 0; i < image.rows; i++)
	{
		for(int j = 0; j < image.cols; j++)
			if(img.at<Vec3b>(i,j) != Vec3b(0,0,0))
				image.at<Vec3b>(i,j) = (1- alpha) * image.at<Vec3b>(i,j) + alpha * img.at<Vec3b>(i,j);
	}
}


void draw_path(Mat& image, Mat& mask, node** cost_map, Vec2i seed, bool flag, double scale, bool is_debug)
{
	int thickness = 2;
	int lineType = 8;
	int x = seed[1], y = seed[0];

	if (is_debug){
		x = (x-1)/3;
		y = (y-1)/3;
	}
	node tem = cost_map[x][y];
	Point start, end;
	Scalar color;
	//cout<<seed[1]<<" "<<seed[0]<<endl;
	if(flag)
		color = Scalar(0,255,255);
	else
		color = Scalar(255,255,0);

	while(tem.pre[0] != -1)
	{
		start = Point(tem.pos[1],tem.pos[0]);
		end = Point(tem.pre[1],tem.pre[0]);
		// cout<<"s "<<tem.pos[0]<<" "<<tem.pos[1]<<endl;
		// cout<<"e "<<tem.pre[0]<<" "<<tem.pre[1]<<endl;
		if(flag)
		{
			line(mask, start, end, Scalar(255,255,255), 2, lineType);
		}

		start = start * scale;
		end = end * scale;
		if (is_debug){
			start = start*3 + Point(1, 1);
			end = end*3 + Point(1, 1);
		}
		line(image, start, end, color, thickness, lineType);
		tem = cost_map[tem.pre[0]][tem.pre[1]];
	}
}

void save_mask(Mat& mask, Mat& image, Vec2d scale)
{
	
	//scan from left
	for(int i = 0; i < mask.rows; i++)
	{
		for(int j = 0; j < mask.cols; j++)
		{
			if(mask.at<Vec3b>(i,j) == Vec3b(255,255,255))
				break;
			else
			{
				mask.at<Vec3b>(i,j) = Vec3b(0,0,0);
			}
		}
	}
	//scan from right
	for(int i = 0; i < mask.rows; i++)
	{
		for(int j = mask.cols - 1; j >=0; j--)
		{
			if(mask.at<Vec3b>(i,j) == Vec3b(255,255,255) || mask.at<Vec3b>(i,j) == Vec3b(0,0,0))
				break;
			else
			{
				mask.at<Vec3b>(i,j) = Vec3b(0,0,0);
			}
		}
	}

	//scan from top
	for(int i = 0; i < mask.cols; i++)
	{
		for(int j = 0; j < mask.rows; j++)
		{
			if(mask.at<Vec3b>(j,i) == Vec3b(255,255,255))
				break;
			else
			{
				mask.at<Vec3b>(j,i) = Vec3b(0,0,0);
			}
			if(i - 1 >= 0)
			{
				if(mask.at<Vec3b>(j,i-1) == Vec3b(100,100,100))
				for(int k = i; k >= 0 ; k--)
				{
					if(mask.at<Vec3b>(j,k) == Vec3b(255,255,255))
						break;
					else
						mask.at<Vec3b>(j,k) = Vec3b(0,0,0);
				}
			}
			if(i + 1 < mask.cols)
			{
				if(mask.at<Vec3b>(j,i+1) == Vec3b(100,100,100))
				for(int k = i; k < mask.cols ; k++)
				{
					if(mask.at<Vec3b>(j,k) == Vec3b(255,255,255))
						break;
					else
						mask.at<Vec3b>(j,k) = Vec3b(0,0,0);
				}
			}
		}
	}

	//scan from bottom
	for(int i = 0; i < mask.cols; i++)
	{
		for(int j = mask.rows - 1; j >= 0; j--)
		{
			if(mask.at<Vec3b>(j,i) == Vec3b(255,255,255))
				break;
			else
			{
				mask.at<Vec3b>(j,i) = Vec3b(0,0,0);
			}

			if(i - 1 >= 0)
			{
				if(mask.at<Vec3b>(j,i-1) == Vec3b(100,100,100))
				for(int k = i; k >= 0 ; k--)
				{
					if(mask.at<Vec3b>(j,k) == Vec3b(255,255,255))
						break;
					else
						mask.at<Vec3b>(j,k) = Vec3b(0,0,0);
				}
			}
			if(i + 1 < mask.cols)
			{
				if(mask.at<Vec3b>(j,i+1) == Vec3b(100,100,100))
				for(int k = i; k < mask.cols ; k++)
				{
					if(mask.at<Vec3b>(j,k) == Vec3b(255,255,255))
						break;
					else
						mask.at<Vec3b>(j,k) = Vec3b(0,0,0);
				}
			}
		}
	}

	while(scale[0] != 1)
	{
		if(scale[0] < 1)
		{
			smaller(mask);
			scale[0] = scale[0] * 2;
		}
		else
		{
			bigger(mask);
			scale[0] = scale[0] / 2;
		}
	}
	bool t1, t2;
	//assign the value
	for(int i = 0; i < mask.rows; i++)
	{
		for(int j = 0; j < mask.cols; j++)
		{
			t1 = mask.at<Vec3b>(i,j)[0] <= 255 && mask.at<Vec3b>(i,j)[1] <= 255 && mask.at<Vec3b>(i,j)[2] <= 255;
			t2 = mask.at<Vec3b>(i,j)[0] >= 100 && mask.at<Vec3b>(i,j)[1] >= 100 && mask.at<Vec3b>(i,j)[2] >= 100;
			if( t1 && t2)
				mask.at<Vec3b>(i,j) = image.at<Vec3b>(i,j);
			else
				mask.at<Vec3b>(i,j) = Vec3b(0,0,0);
		}
	}

	while(scale[1] != 1)
	{
		if(scale[1] < 1)
		{
			smaller(mask);
			scale[1] = scale[1] * 2;
		}
		else
		{
			bigger(mask);
			scale[1] = scale[1] / 2;
		}
	}
}

void save_stack(Mat& image, stack<Mat>& s)
{
	Mat img;
	image.copyTo(img);

	s.push(img);
}

void save_scale(Vec2d& scale, stack<Vec2d>& scale_stack) {
	Vec2d s = scale;
	scale_stack.push(s);
}

void save_node_list_stack(vector<node*>& node_list, stack<vector<node*>>& node_list_stack) {
	vector<node*> temp = node_list;
	node_list_stack.push(temp);
}

node** undo(Mat& tem1, Mat& mask, stack<node**>& map_stack, stack<Mat>& mask_stack, 
			stack<Mat>& tem1_stack, stack<Vec2d>& scale_stack, Vec2d& scale, stack<vector<node*>>& node_list_stack, vector<node*>& node_list)
{
	map_stack.pop();
	mask_stack.pop();
	mask = mask_stack.top();
	tem1_stack.pop();
	tem1 = tem1_stack.top();

	scale_stack.pop();
	scale = scale_stack.top();
	
	node_list_stack.pop();
	node_list = node_list_stack.top();
	
	return map_stack.top();
}

void stop(Mat& tem1, Mat& o_image, Mat& mask, double scale, 
		  stack<node**>& map_stack, stack<Mat> tem1_stack, 
		  stack<Mat> mask_stack, node** cost_map, 
		  int row, int col, bool& close, int& seed_count, stack<Vec2d>& scale_stack, stack<vector<node*>>& node_list_stack)
{
	close = false;
	seed_count = 0;

	while(!map_stack.empty())
	{
		map_stack.pop();
	}

	while(!mask_stack.empty())
	{
		mask_stack.pop();
	}

	while(!tem1_stack.empty())
	{
		tem1_stack.pop();
	}
	
	while (!scale_stack.empty())
	{
		scale_stack.pop();
	}

	while (!node_list_stack.empty())
	{
		node_list_stack.pop();
	}

	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
			cost_map[i][j].state = 0;
	}

	Mat image;
	o_image.copyTo(image);
	while(scale != 1)
	{
		if(scale < 1)
		{
			smaller(image);
			scale = scale * 2;
		}
		else
		{
			bigger(image);
			scale = scale / 2;
		}
	}
	image.copyTo(tem1);
	mask = Mat(row, col, CV_8UC3, Scalar(100,100,100));
}