#ifndef SCALE_H
#define SCALE_H

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include"node.h"

using namespace cv;

void larger(Mat& image, Mat& l_img);
void blur(Mat& img);
void bigger(Mat& img);
void smaller(Mat& img);

void CreatePixelAndCostGraph (Mat& image, Mat& pixel_graph, Mat& cost_graph, node** cost_map);
#endif