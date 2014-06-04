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
#include <math.h>
#include "Vanishing Point.h"

#ifndef __MacOpenCV__Find_the_best_lines__
#define __MacOpenCV__Find_the_best_lines__

//find_the_best_line回傳資料結構
class FTBL 
{
public:
	CvPoint FTBL_Point_L;
	CvPoint FTBL_Point_R;
	FTBL();
	FTBL(CvPoint Point_data_L, CvPoint Point_data_R);
};

//線段分群類別
class line_group 
{
public:
	CvPoint GroupPoint;
	int GroupScore;
	double angle;
	double group_line_slope;
	line_group(CvPoint firstPoint);
	void update(CvPoint newPoint);
	double calculate_the_angle(CvPoint Vanishing_Point);
};
//分群Vector sort
bool sort_for_line_group_class(const line_group a, const line_group b);
bool sort_for_line_group_class_ANGLE(const line_group a, const line_group b);

//畫出範圍框
void draw_VPoint(IplImage* img, int x, int y, int vp_range);

//(最佳車道線使用的canny,左線段vector儲存空間,右線段vector儲存空間）
void find_best_line_Hough(IplImage* mainCanny, vector<line_property> *leftSlope, vector<line_property> *rightSlope);

//找出最佳車道線 (原始彩圖,完整Canny,消失點位置,消失點周圍範圍）
FTBL FindTheBestLines(IplImage* Ori_pic, IplImage* FullCanny, CvPoint vpfnpoint, int vp_range);

//計算延伸線段座標
CvPoint Calculation_extension(CvPoint Vanishing_Point, double slope, int y);

#endif /* defined(__MacOpenCV__Find_the_best_lines__) */
