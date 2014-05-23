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
#include "Vanishing Point.h"

#ifndef __MacOpenCV__Find_the_best_lines__
#define __MacOpenCV__Find_the_best_lines__

//�e�X�d���
void draw_VPoint(IplImage* img, int x, int y, int vp_range);

//(�̨Ψ��D�u�ϥΪ�canny,���u�qvector�x�s�Ŷ�,�k�u�qvector�x�s�Ŷ��^
void find_best_line_Hough(IplImage* mainCanny, vector<line_property> *leftSlope, vector<line_property> *rightSlope);

//��X�̨Ψ��D�u (��l�m��,����Canny,�����I��m,�����I�P��d��^
void FindTheBestLines(IplImage* Ori_pic, IplImage* FullCanny, CvPoint vpfnpoint, int vp_range);

#endif /* defined(__MacOpenCV__Find_the_best_lines__) */
