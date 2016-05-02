
#ifndef _COST_H
#define _COST_H

#include <cmath>
#include <queue>
#include <opencv2/opencv.hpp>
#include "NodeWrap.h"

using namespace cv;
using namespace std;

void cal_local_cost(Node** nodes, Mat& l_img);
void gen_path_tree(Node** nodes, Point seed, int row, int col, vector<Node*>& node_list);
Node ** gen_new_tree(Node** nodes, int row, int col);

#endif