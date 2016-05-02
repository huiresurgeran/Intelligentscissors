#ifndef MY_DRAW_H
#define MY_DRAW_H

//---------------------------------------------------------
// This header file includes the functions for drawing 
// the seed points and path from seed to current position
//---------------------------------------------------------

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include"node.h"
#include"scale.h"
#include<stack>
using namespace cv;


void DrawPathTree(Mat& tree_graph, Mat& cost_graph, int node_num, node** cost_map, Vec2i seed, int row, int col, vector<node*> node_list);
void draw_circle(Mat& image, Point x,double alpha, bool flag, Vec2d scale);
void draw_path(Mat& image, Mat& mask, node** cost_map, Vec2i current, bool flag, double scale, bool is_debug = false);
void save_mask(Mat& mask, Mat& image, Vec2d scale);
void save_stack(Mat& image, stack<Mat>& s);
void save_scale(Vec2d& scale, stack<Vec2d>& s);
void save_node_list_stack(vector<node*>& node_list, stack<vector<node*>>& node_list_stack);
node** undo(Mat& tem1, Mat& mask, stack<node**>& map_stack, stack<Mat>& mask_stack, stack<Mat>& tem1_stack, stack<Vec2d>& scale_stack, Vec2d& scale, stack<vector<node*>>& node_list_stack, vector<node*>& node_list);
void stop(Mat& tem1, Mat& o_image, Mat& mask, double scale, stack<node**>& map_stack, stack<Mat> tem1_stack, stack<Mat> mask_stack, node** cost_map, int row, int col, bool& close, int& seed_count, stack<Vec2d>& scale_stack, stack<vector<node*>>& node_list_stack);

#endif