//
//  Find the best lines.h
//
//  Created by Chienfu Huang on 2014/5/22.
//  Copyright (c) 2014�~ Chienfu Huang. All rights reserved.
//
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <math.h>
#include "Vanishing Point.h"

#ifndef __MacOpenCV__Find_the_best_lines__
#define __MacOpenCV__Find_the_best_lines__

//find_the_best_line�^�Ǹ�Ƶ��c
class FTBL 
{
public:
	CvPoint FTBL_Point_L;
	CvPoint FTBL_Point_R;
	FTBL();
	FTBL(CvPoint Point_data_L, CvPoint Point_data_R);
};

//�u�q���s���O
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
//���sVector sort
bool sort_for_line_group_class(const line_group a, const line_group b);
bool sort_for_line_group_class_ANGLE(const line_group a, const line_group b);

//�e�X�d���
void draw_VPoint(IplImage* img, int x, int y, int vp_range);

//(�̨Ψ��D�u�ϥΪ�canny,���u�qvector�x�s�Ŷ�,�k�u�qvector�x�s�Ŷ��^
void find_best_line_Hough(IplImage* mainCanny, vector<line_property> *leftSlope, vector<line_property> *rightSlope);

//��X�̨Ψ��D�u (��l�m��,����Canny,�����I��m,�����I�P��d��^
FTBL FindTheBestLines(IplImage* Ori_pic, IplImage* FullCanny, CvPoint vpfnpoint, int vp_range);

//�p�⩵���u�q�y��
CvPoint Calculation_extension(CvPoint Vanishing_Point, double slope, int y);

#endif /* defined(__MacOpenCV__Find_the_best_lines__) */
