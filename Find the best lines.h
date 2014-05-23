//
//  Find the best lines.h
//
//  Created by Chienfu Huang on 2014/5/22.
//  Copyright (c) 2014年 Chienfu Huang. All rights reserved.
//
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include "Vanishing Point.h"

#ifndef __MacOpenCV__Find_the_best_lines__
#define __MacOpenCV__Find_the_best_lines__

//畫出範圍框
void draw_VPoint(IplImage* img, int x, int y, int vp_range);

//(最佳車道線使用的canny,左線段vector儲存空間,右線段vector儲存空間）
void find_best_line_Hough(IplImage* mainCanny, vector<line_property> *leftSlope, vector<line_property> *rightSlope);

//找出最佳車道線 (原始彩圖,完整Canny,消失點位置,消失點周圍範圍）
void FindTheBestLines(IplImage* Ori_pic, IplImage* FullCanny, CvPoint vpfnpoint, int vp_range);

#endif /* defined(__MacOpenCV__Find_the_best_lines__) */
