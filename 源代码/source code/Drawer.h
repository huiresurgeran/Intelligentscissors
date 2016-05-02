
#ifndef _DRAWER_H
#define _DRAWER_H

#include <stack>
#include <opencv2/opencv.hpp>
#include "NodeWrap.h"
#include "Scale.h"

using namespace std;
using namespace cv;

void draw_path_tree(Mat& tree_graph, Mat& cost_graph, int node_num, Node** nodes, Point seed, int row, int col, vector<Node*> node_list);
void draw_circle(Mat& image, Point x,double alpha, bool flag, Vec2d scale);
void draw_path(Mat& image, Mat& mask, Node** nodes, Point current, bool flag, double scale, bool is_debug = false);
void save_mask(Mat& mask, Mat& image, Vec2d scale);
void save_stack(Mat& image, stack<Mat>& s);
void save_scale(Vec2d& scale, stack<Vec2d>& s);
void save_node_list_stack(vector<Node*>& node_list, stack<vector<Node*>>& node_list_stack);
Node** undo(Mat& tem1, Mat& mask, stack<Node**>& map_stack, stack<Mat>& mask_stack, stack<Mat>& tem1_stack, stack<Vec2d>& scale_stack, Vec2d& scale, stack<vector<Node*>>& node_list_stack, vector<Node*>& node_list);
void stop(Mat& tem1, Mat& o_image, Mat& mask, double scale, stack<Node**>& map_stack, stack<Mat> tem1_stack, stack<Mat> mask_stack, Node** nodes, int row, int col, bool& close, int& seed_count, stack<Vec2d>& scale_stack, stack<vector<Node*>>& node_list_stack);

#endif