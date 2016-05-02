
#include "stdafx.h"
#include <iostream>
#include <stack>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <Windows.h>
#include "Cost.h"
#include "Drawer.h"
#include "Scale.h"

#define NDEBUG

using namespace std;
using namespace cv;

#pragma comment(lib, "opencv_highgui249d.lib")
#pragma comment(lib, "opencv_needCalib3d249d.lib")
#pragma comment(lib, "opencv_videostab249d.lib")
#pragma comment(lib, "opencv_videoio249d.lib")
#pragma comment(lib, "opencv_video249d.lib")
#pragma comment(lib, "opencv_ts249d.lib")
#pragma comment(lib, "opencv_superres249d.lib")
#pragma comment(lib, "opencv_stitching249d.lib")
#pragma comment(lib, "opencv_shape249d.lib")
#pragma comment(lib, "opencv_photo249d.lib")
#pragma comment(lib, "opencv_objdetect249d.lib")
#pragma comment(lib, "opencv_imgproc249d.lib")
#pragma comment(lib, "opencv_ml249d.lib")
#pragma comment(lib, "opencv_imgcodecs249d.lib")
#pragma comment(lib, "opencv_flann249d.lib")
#pragma comment(lib, "opencv_features2d249d.lib")
#pragma comment(lib, "opencv_core249d.lib")

int seedCount = 0;
bool needDraw = false;
bool needCal = true;
bool isClose = false;

Point initSeed(0,0);
Point curSeed(0,0);
Point curPos(0,0);
Vec2d scale(1.0,1.0);

double cal_distance(const Point & p1,const Point & p2)
{
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return sqrt(dx * dx + dy * dy);
}

void mouse_callback( int event, int x, int y, int flags, void* img )
{
	Mat* image = (Mat*) img;
	switch(event)
	{
		case CV_EVENT_LBUTTONDOWN:
			needDraw = true;
			needCal = false;
			seedCount++;
			curSeed = Point(x / (scale[0] * scale[1]), (scale[0] * scale[1]));
			if(seedCount == 1)
			{
				initSeed = Point(x / (scale[0] * scale[1]), (scale[0] * scale[1]));
			}
			break;

		case CV_EVENT_MOUSEMOVE:
			if(needDraw)
			{
				curPos = Point(x / (scale[0] * scale[1]), (scale[0] * scale[1]));
			}
	
			break;
	}
}

int main(int argc, char* argv[])
{

	const char* imgName1 = "avatar.tif";
	const char* imgName2 = "lena.tif";
	const char* imgName3 = "logo_eye.bmp";

	Mat srcImg = imread(imgName1);
	srcImg.copyTo(image);

	Mat image;
	Mat tmpImg1;
	image.copyTo(tmpImg1);
	Mat tmpImg2;
	Mat mask(image.rows, image.cols, CV_8UC3, Scalar(100,100,100));

	stack<Mat> maskStack;
	stack<Mat> tmpStack1;

	while(image.rows > 260)
	{
		scale[0] = scale[0] * 2;
		smaller(image);
	}

	Mat l_img(image.rows+2, image.cols+2, CV_8UC3);
	larger(image,l_img);

	stack<Node**> mapStack;
	Node** nodes = new Node*[image.rows];
	for(int i = 0; i < image.rows; i++)
		nodes[i] = new Node [image.cols];

	cal_local_cost(nodes, l_img);

	namedWindow("Intelligent Scissor");
	cvSetMouseCallback("Intelligent Scissor", mouse_callback, (void*) &tmpImg1);

	while(true)
	{
		tmpImg1.copyTo(tmpImg2);

		if(needDraw && !needCal && !isClose)
		{
			if(seedCount > 1)
			{
				if(cal_distance(curSeed, initSeed) < 5)
					isClose = true;
			}
		}

		if(needDraw && !needCal)
		{
			if(isClose)
			{
				draw_path(tmpImg1, mask, nodes, initSeed, true, scale[0] * scale[1]);
				draw_circle(tmpImg1, initSeed, 0.5, false, scale);
				save_stack(mask,maskStack);
				save_stack(tmpImg1,tmpStack1);
				save_mask(mask, srcImg, scale);
				imshow("ok",mask);
				needCal = true;
			}
			else
			{
				if(seedCount > 1)
				{
					draw_path(tmpImg1, mask, nodes, curSeed, true, scale[0] * scale[1]);
					nodes = gen_new_tree(nodes, image.rows, image.cols);
				}
				draw_circle(tmpImg1, curSeed, 0.5, true, scale);
				save_stack(mask, maskStack);
				save_stack(tmpImg1, tmpStack1);

				vector<Node*> tmp;
				gen_path_tree(nodes, curSeed, image.rows, image.cols, tmp);
				mapStack.push(nodes);

				needCal = true;
			}
		}

		if(needDraw && !isClose)
		{

			draw_path(tmpImg2, mask, nodes, curPos, false, scale[0] * scale[1]);
	
			if(cal_distance(curPos, initSeed) < 5)
			{
				draw_circle(tmpImg2,curPos,0.5,false, scale);
			}
		}

		imshow("Intelligent Scissor",tmpImg2);
		imshow("ok", mask);

		switch(waitKey(15))
		{
			case 27:break;
			case 98:
				bigger(tmpImg1);
				scale[1] = scale[1] * 2.0;
				break;
			case 102:
				if(!isClose && seedCount > 1)
				{
					needCal = false;
					curSeed[0] = initSeed[0];
					curSeed[1] = initSeed[1];
				}
				break;
			case 115:
				smaller(tmpImg1);
				scale[1] = scale[1] / 2.0;
				break;
			case 117:
				if(seedCount != 0 && seedCount != 1) 
				{
					seedCount--;
				}
				break;
			default:
				break;
			}

		}

	}
	
	waitKey();
	return 0;
}