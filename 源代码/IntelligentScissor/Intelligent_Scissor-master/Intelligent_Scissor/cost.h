#ifndef COST_H
#define COST_H

//-----------------------------------------------------------------------
// This header file includes the functions which are used to calculate the 
// cost graph, also some help functions, such as larger
//-----------------------------------------------------------------------

#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<math.h>
#include<queue>
#include"node.h"

using namespace cv;
using namespace std;

void cal_cost_map(node** cost_map, Mat& l_img);
void generate_tree(node** cost_map, Vec2i seed, int row, int col, vector<node*>& node_list);
node** new_tree(node** cost_map, int row, int col);

#endif