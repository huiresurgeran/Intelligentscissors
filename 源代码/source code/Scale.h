
#ifndef _SCALE_H
#define _SCALE_H

#include <opencv2/opencv.hpp>
#include "NodeWrap.h"

using namespace std;
using namespace cv;

void larger(Mat& image, Mat& l_img);
void blur(Mat& img);
void bigger(Mat& img);
void smaller(Mat& img);
void create_pixel_and_cost_graph (Mat& image, Mat& pixel_graph, Mat& cost_graph, Node** nodes);

#endif