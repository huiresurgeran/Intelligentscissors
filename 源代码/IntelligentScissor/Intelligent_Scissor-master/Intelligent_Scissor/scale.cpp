#include "stdafx.h"
#include"scale.h"

void CreatePixelAndCostGraph(Mat& image, Mat& pixel_graph, Mat& cost_graph, node** cost_map) {
	pixel_graph = Mat(image.rows*3, image.cols*3, CV_8UC3);
	cost_graph = Mat(image.rows*3, image.cols*3, CV_8UC3);

	int dx[] = {0, -1, -1, -1, 0, 1, 1, 1};
	int dy[] = {1, 1, 0, -1, -1, -1, 0, 1};
	int dir[] = {0, 1, 2, 3, 4, 5, 6, 7};

	int row = 0, col = 0, cost_index = -1;
	double color = 0;

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			pixel_graph.at<Vec3b>(3*i+1, 3*j+1) = image.at<Vec3b>(i,j);
			cost_graph.at<Vec3b>(3*i+1, 3*j+1) = image.at<Vec3b>(i,j);
			for (int k = 0; k < 8; k++) {
				row = 3*i + 1 + dx[k];
				col = 3*j + 1 + dy[k];
				cost_index = dir[k];
				color = cost_map[i][j].cost[cost_index] / 2;
				
				pixel_graph.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
				cost_graph.at<Vec3b>(row, col) = Vec3b(color, color, color);
			}
		}
	}


}


void larger(Mat& image, Mat& l_img)
{
	//4 corner points
	l_img.at<Vec3b>(0,0) = image.at<Vec3b>(0,0);
	l_img.at<Vec3b>(0,image.cols+1) = image.at<Vec3b>(0,image.cols-1);
	l_img.at<Vec3b>(image.rows+1,0) = image.at<Vec3b>(image.rows-1,0);
	l_img.at<Vec3b>(image.rows+1,image.cols+1) = image.at<Vec3b>(image.rows-1,image.cols-1);
	//2 boundary rows
	for(int i = 1; i < image.cols + 1; i++)
		l_img.at<Vec3b>(0,i) = image.at<Vec3b>(0,i-1);
	for(int i = 1; i < image.cols + 1; i++)
		l_img.at<Vec3b>(image.rows+1,i) = image.at<Vec3b>(image.rows-1,i-1);
	//2 boundary cols
	for(int i = 1; i < image.rows + 1; i++)
		l_img.at<Vec3b>(i,0) = image.at<Vec3b>(i-1,0);
	for(int i = 1; i < image.rows + 1; i++)
		l_img.at<Vec3b>(i,image.cols+1) = image.at<Vec3b>(i-1,image.cols-1);
	//other pixels
	for(int i = 1; i < image.rows + 1; i++)
	{
		for(int j = 1; j < image.cols + 1; j++)
		{
			l_img.at<Vec3b>(i,j) = image.at<Vec3b>(i-1,j-1);
		}
	}
}

void blur(Mat& img)
{
	int gaussian[9] = {1,2,1,2,4,2,1,2,1};
	int dir_x[9] = {-1,-1,-1,0,0,0,1,1,1};
	int dir_y[9] = {-1,0,1,-1,0,1,-1,0,1};
	int x,y,count;
	int r,g,b;
	Vec3i tem;

	for(int i = 0; i < img.rows; i++)
	{
		for(int j = 0; j < img.cols; j++)
		{
			count = 0;
			r = g = b = 0;
			for(int k = 0; k < 9; k++)
			{
				x = i + dir_x[k];
				y = j + dir_y[k];
				if(x >= 0 && x < img.rows && y >= 0 && y < img.cols)
				{
					count += gaussian[k];
					r += img.at<Vec3b>(x,y)[0] * gaussian[k];
					g += img.at<Vec3b>(x,y)[1] * gaussian[k];
					b += img.at<Vec3b>(x,y)[2] * gaussian[k];
				}
			}
			img.at<Vec3b>(i,j)[0] = r / count;
			img.at<Vec3b>(i,j)[1] = g / count;
			img.at<Vec3b>(i,j)[2] = b / count;
		}
	}
	//cout<<"ok"<<endl;
}

void bigger(Mat& img)
{
	//upsampling
	Mat b_img(img.rows*2, img.cols*2, CV_8UC3, Scalar(0,0,0));
	for(int i = 0; i < b_img.rows; i += 2)
	{
		for(int j = 0; j < b_img.cols; j += 2)
		{
			b_img.at<Vec3b>(i,j) = img.at<Vec3b>(i/2,j/2);
		}
	}
	int r,g,b;
	for(int i = 0; i < b_img.rows; i += 2)
	{
		for(int j = 1; j < b_img.cols - 1; j += 2)
		{
			r = (b_img.at<Vec3b>(i,j-1)[0] + b_img.at<Vec3b>(i,j+1)[0])/2;
			g = (b_img.at<Vec3b>(i,j-1)[1] + b_img.at<Vec3b>(i,j+1)[1])/2;
			b = (b_img.at<Vec3b>(i,j-1)[2] + b_img.at<Vec3b>(i,j+1)[2])/2;
			b_img.at<Vec3b>(i,j) = Vec3b(r,g,b);
		}
	}
	for(int i = 0; i < b_img.rows; i += 2)
		b_img.at<Vec3b>(i,b_img.cols-1) = b_img.at<Vec3b>(i,b_img.cols-2);

	for(int i = 1; i < b_img.rows - 1; i += 2)
	{
		for(int j = 0; j < b_img.cols; j ++)
		{
			r = (b_img.at<Vec3b>(i-1,j)[0] + b_img.at<Vec3b>(i+1,j)[0])/2;
			g = (b_img.at<Vec3b>(i-1,j)[1] + b_img.at<Vec3b>(i+1,j)[1])/2;
			b = (b_img.at<Vec3b>(i-1,j)[2] + b_img.at<Vec3b>(i+1,j)[2])/2;
			b_img.at<Vec3b>(i,j) = Vec3b(r,g,b);
		}
	}
	for(int j = 0; j < b_img.cols; j++)
		b_img.at<Vec3b>(b_img.rows-1,j) = b_img.at<Vec3b>(b_img.rows-2,j);

	img = b_img;
}

void smaller(Mat& img)
{
	//we first blur the image to eliminate high frequency
	blur(img);
	//downsampling
	Mat s_img(img.rows/2, img.cols/2, CV_8UC3);
	for(int i = 0; i < img.rows - 1; i += 2)
	{
		for(int j = 0; j < img.cols - 1; j += 2)
		{
			s_img.at<Vec3b>(i/2,j/2) = img.at<Vec3b>(i,j);
		}
	}
	img = s_img;
}