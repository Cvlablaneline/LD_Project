//
//  Find the best lines.cpp
//
//  Created by Chienfu Huang on 2014/5/22.
//  Copyright (c) 2014�~ Chienfu Huang. All rights reserved.
//
#include "stdafx.h"
#include "Find the best lines.h"
#include "Vanishing Point.h"

//�e�X�d���
void draw_VPoint(IplImage* img, int x, int y, int vp_range)
{
	CvPoint VPoint, vline[4];
	VPoint.x = x;
	VPoint.y = y;
	cvCircle(img, VPoint, 4, CV_RGB(0, 255, 255), 1);
	vline[0].x = x - vp_range;
	vline[0].y = y - vp_range;
	vline[1].x = x + vp_range;
	vline[1].y = y - vp_range;
	vline[2].x = x - vp_range;
	vline[2].y = y + vp_range;
	vline[3].x = x + vp_range;
	vline[3].y = y + vp_range;
	cvLine(img, vline[0], vline[1], CV_RGB(255, 0, 255), 2);
	cvLine(img, vline[1], vline[3], CV_RGB(255, 0, 255), 2);
	cvLine(img, vline[0], vline[2], CV_RGB(255, 0, 255), 2);
	cvLine(img, vline[2], vline[3], CV_RGB(255, 0, 255), 2);
}

//�]�D�{���ϥΪ�canny,���u�qvector�x�s�Ŷ�,�k�u�qvector�x�s�Ŷ��^
void find_best_line_Hough(IplImage* mainCanny, vector<line_property> *leftSlope, vector<line_property> *rightSlope)
{
	unsigned char* mainCannyData = (unsigned char*)mainCanny->imageData;

	//�B�ӤW�b��50%
	for (int h = 0; h < mainCanny->height * 50 / 100; h++)
	{
		for (int w = 0; w < mainCanny->width; w++)
		{
			if (mainCannyData[mainCanny->width * h + w] == 255)
			{
				mainCannyData[mainCanny->width * h + w] = 0;
			}
		}
	}

	//�B�ӤU�b��10%
	for (int h = mainCanny->height * 90 / 100; h < mainCanny->height; h++)
	{
		for (int w = 0; w < mainCanny->width; w++)
		{
			if (mainCannyData[mainCanny->width * h + w] == 255)
			{
				mainCannyData[mainCanny->width * h + w] = 0;
			}
		}
	}

	// �N�ҽu�ഫ
	CvMemStorage* storage_Hough = cvCreateMemStorage(0);
	CvSeq* Hough_Lines = cvHoughLines2(mainCanny, storage_Hough, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 59, 10);

	for (int i = 0; i < Hough_Lines->total; i++)
	{
		CvPoint* ptThis = (CvPoint*)cvGetSeqElem(Hough_Lines, i);
		if (ptThis[0].x - ptThis[1].x == 0)
			continue;

		//�إ߽u�q��T
		line_property line_info_temp(ptThis[0], ptThis[1]);

		//���L�ײv���ŦX���u�q
		if (abs(line_info_temp.line_slope) < 0.27)
			continue;
		//�������D�u�ײv��V
		if (line_info_temp.line_slope < 0)
			(*leftSlope).push_back(line_info_temp);
		else
			(*rightSlope).push_back(line_info_temp);
	}
	cout << "���u��Ƽƶq: " << (*leftSlope).size() << "\t�k�u��Ƽƶq: " << (*rightSlope).size() << endl;

}

//��X�̨Ψ��D�u
void FindTheBestLines(IplImage* Ori_pic, IplImage* FullCanny, CvPoint vpfnpoint, int vp_range)
{
	int vpx = vpfnpoint.x;
	int vpy = vpfnpoint.y;
	//�����O���N���ഫ�ᨮ�D�u�ײv��V
	vector <line_property> newleftSlope, newrightSlope;

	find_best_line_Hough(FullCanny, &newleftSlope, &newrightSlope);

	//�t��P�p��X�C�ӥ��I
	for (int i = 0; i < newleftSlope.size(); i++)
	{
		for (int j = 0; j < newrightSlope.size(); j++)
		{
			//�׶}�ײv�ۦP���u�q
			if (abs(newleftSlope[i].line_slope - newrightSlope[j].line_slope) == 0)
				continue;
			//�I�s�p��
			line_crosspoint vPoint(newleftSlope[i].line_slope, newleftSlope[i].line_intercept, newrightSlope[j].line_slope, newrightSlope[j].line_intercept);


			if (vPoint.LC_point.x > vpx + vp_range || vPoint.LC_point.x < vpx - vp_range || vPoint.LC_point.y > vpy + vp_range || vPoint.LC_point.y < vpy - vp_range)
				continue;
			else
			{
				//�ͦ����k�u�q���̧C�I
				CvPoint left_low_point, right_low_point;
				if (newleftSlope[i].line_point_1->y > newleftSlope[i].line_point_2->y)
					left_low_point = *newleftSlope[i].line_point_1;
				else
					left_low_point = *newleftSlope[i].line_point_2;

				if (newrightSlope[j].line_point_1->y > newrightSlope[j].line_point_2->y)
					right_low_point = *newrightSlope[j].line_point_1;
				else
					right_low_point = *newrightSlope[j].line_point_2;
				//�e�X�����I�P�������u�q
				//�I
				cvCircle(Ori_pic, vPoint.LC_point, 4, CV_RGB(255, 0, 0), 1);
				//��
				cvLine(Ori_pic, vPoint.LC_point, left_low_point, CV_RGB(255, 0, 255), 2);
				//�k
				cvLine(Ori_pic, vPoint.LC_point, right_low_point, CV_RGB(255, 0, 255), 2);
			}
		}

	}
	//�M�����D�u�ײv�O��vector
	vector <line_property> Nls, Nrs;
	newleftSlope.clear();
	newrightSlope.clear();
	newleftSlope.swap(Nls);
	newrightSlope.swap(Nrs);
}






