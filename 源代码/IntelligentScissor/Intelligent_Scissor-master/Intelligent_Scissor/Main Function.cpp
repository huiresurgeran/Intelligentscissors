#include "stdafx.h"
#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<math.h>
#include<Windows.h>
#include"cost.h"
#include"my_draw.h"
#include"scale.h"
#include<stack>

using namespace std;
using namespace cv;

int seed_count = 0;
bool draw = false;
bool cal = true;
bool close = false;
Vec2i b_seed(0,0);
Vec2i seed(0,0);
Vec2i current(0,0);
Vec2d scale(1.0,1.0);

void my_mouse_callback( int event, int x, int y, int flags, void* img )
{
	Mat* image = (Mat*) img;
	switch(event)
	{
		case CV_EVENT_LBUTTONDOWN:
			draw = true;
			cal = false;
			seed_count++;
			//caution: the x is number of col, and y is the number of row
			//This convention is consistent with the draw function, but is reverse for our cost_map
			//in the cost map, x should be row, and y should be col
			seed[0] = current[0] = x / (scale[0] * scale[1]);
			seed[1] = current[1] = y / (scale[0] * scale[1]);
			if(seed_count == 1)
			{
				b_seed[0] = x / (scale[0] * scale[1]);
				b_seed[1] = y / (scale[0] * scale[1]);
			}
			break;

		case CV_EVENT_MOUSEMOVE:
			if(draw)
			{
				current[0] = x / (scale[0] * scale[1]);
				current[1] = y / (scale[0] * scale[1]);
			}
	
			break;
	}
}

int main(int argc, char* argv[])
{
	//read the image
	const char* name1 = "avatar.tif";
	const char* name2 = "lena.tif";
	const char* name3 = "logo_eye.bmp";
	Mat o_image = imread(name1);
	Mat image;
	o_image.copyTo(image);
	Mat tem1;
	image.copyTo(tem1);
	while(image.rows > 260)
	{
		scale[0] = scale[0] * 2;
		smaller(image);
	}
	cout<<scale[0]<<endl;

	//l_img is larger image 
	Mat l_img(image.rows+2, image.cols+2, CV_8UC3);
	larger(image,l_img);

	//get the cost map
	stack<node**> map_stack;
	node** cost_map = new node*[image.rows];
	for(int i = 0; i < image.rows; i++)
		cost_map[i] = new node [image.cols];
	cal_cost_map(cost_map, l_img);

	//set up the window and mouse call back
	namedWindow("Intelligent Scissor");
	cvSetMouseCallback("Intelligent Scissor", my_mouse_callback, (void*) &tem1);
	Mat tem2;
	Mat mask(image.rows, image.cols, CV_8UC3, Scalar(100,100,100));
	stack<Mat> mask_stack;
	stack<Mat> tem1_stack;

	while(1)
	{
		tem1.copyTo(tem2);

		if(draw && !cal && !close)
		{
			if(seed_count > 1)
			{

				double dis = sqrt((double)(seed[0] - b_seed[0]) * (seed[0] - b_seed[0]) + (seed[1] - b_seed[1]) * (seed[1] - b_seed[1]));
				if(dis < 5)
				close = true;
			}
		}

		//draw path and generate tree if not close
		if(draw && !cal)
		{
			if(close)
			{
				draw_path(tem1, mask, cost_map, b_seed, true, scale[0] * scale[1]);
				draw_circle(tem1, b_seed, 0.5, false, scale);
				save_stack(mask,mask_stack);
				save_stack(tem1,tem1_stack);
				save_mask(mask, o_image, scale);
				imshow("ok",mask);
				cal = true;
			}
			else
			{
				if(seed_count > 1)
				{
					draw_path(tem1, mask, cost_map, seed, true, scale[0] * scale[1]);
					cost_map = new_tree(cost_map, image.rows, image.cols);
				}
				draw_circle(tem1, seed, 0.5, true, scale);
				save_stack(mask, mask_stack);
				save_stack(tem1, tem1_stack);
				cout<<"computing...";
				vector<node*> temp;
				generate_tree(cost_map, seed, image.rows, image.cols, temp);
				map_stack.push(cost_map);
				cout<<"finished"<<endl;
				cal = true;
			}
		}

		if(draw && !close)
		{
			//draw the path from seed to current position
			draw_path(tem2, mask, cost_map, current, false, scale[0] * scale[1]);
			double dis = sqrt((double)(current[0] - b_seed[0]) * (current[0] - b_seed[0]) + (current[1] - b_seed[1]) * (current[1] - b_seed[1]));
			if(dis < 5)
			{
				draw_circle(tem2,current,0.5,false, scale);
			}
		}

		imshow("Intelligent Scissor",tem2);
		imshow("ok", mask);

		
		int a = cvWaitKey(15);
		if(a == 27) //Esc
			break;
		else if (a == 115) //s
		{
			smaller(tem1);
			scale[1] = scale[1] / 2.0;
		}
		else if(a == 98) //b
		{
			bigger(tem1);
			scale[1] = scale[1] * 2.0;
		}
		else if(a == 102) //f
		{
			//finish the countor
			if(!close && seed_count > 1)
			{
				cal = false;
				seed[0] = b_seed[0];
				seed[1] = b_seed[1];
			}
		}
		else if(a == 117) // u
		{
			//undo the seed point
			if(seed_count != 0)
			{
				if(seed_count == 1)
				{
					//stop(tem1, o_image, mask, scale[1], map_stack, tem1_stack, mask_stack, cost_map, image.rows, image.cols, draw, cal, close, seed_count);
				}
				else
				{
					//cost_map = undo(tem1, mask, map_stack, mask_stack, tem1_stack);
					seed_count--;
				}
			}
		}
		else if(a == 97) //a
		{
			// stop(tem1, o_image, mask, scale[1], map_stack, tem1_stack, mask_stack, cost_map, image.rows, image.cols,  draw, cal, close, seed_count);
		}
	}
	
	waitKey();
	return 0;
}