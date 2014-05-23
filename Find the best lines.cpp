//
//  Find the best lines.cpp
//
//  Created by Chienfu Huang on 2014/5/22.
//  Copyright (c) 2014年 Chienfu Huang. All rights reserved.
//
#include "stdafx.h"
#include "Find the best lines.h"
#include "Vanishing Point.h"

//畫出範圍框
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

//（主程式使用的canny,左線段vector儲存空間,右線段vector儲存空間）
void find_best_line_Hough(IplImage* mainCanny, vector<line_property> *leftSlope, vector<line_property> *rightSlope)
{
	unsigned char* mainCannyData = (unsigned char*)mainCanny->imageData;

	//遮照上半部50%
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

	//遮照下半部10%
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

	// 霍夫線轉換
	CvMemStorage* storage_Hough = cvCreateMemStorage(0);
	CvSeq* Hough_Lines = cvHoughLines2(mainCanny, storage_Hough, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 59, 10);

	for (int i = 0; i < Hough_Lines->total; i++)
	{
		CvPoint* ptThis = (CvPoint*)cvGetSeqElem(Hough_Lines, i);
		if (ptThis[0].x - ptThis[1].x == 0)
			continue;

		//建立線段資訊
		line_property line_info_temp(ptThis[0], ptThis[1]);

		//跳過斜率不符合的線段
		if (abs(line_info_temp.line_slope) < 0.27)
			continue;
		//分類車道線斜率方向
		if (line_info_temp.line_slope < 0)
			(*leftSlope).push_back(line_info_temp);
		else
			(*rightSlope).push_back(line_info_temp);
	}
	cout << "左線資料數量: " << (*leftSlope).size() << "\t右線資料數量: " << (*rightSlope).size() << endl;

}

//找出最佳車道線
void FindTheBestLines(IplImage* Ori_pic, IplImage* FullCanny, CvPoint vpfnpoint, int vp_range)
{
	int vpx = vpfnpoint.x;
	int vpy = vpfnpoint.y;
	//分類記錄霍夫轉換後車道線斜率方向
	vector <line_property> newleftSlope, newrightSlope;

	find_best_line_Hough(FullCanny, &newleftSlope, &newrightSlope);

	//配對與計算出每個交點
	for (int i = 0; i < newleftSlope.size(); i++)
	{
		for (int j = 0; j < newrightSlope.size(); j++)
		{
			//避開斜率相同之線段
			if (abs(newleftSlope[i].line_slope - newrightSlope[j].line_slope) == 0)
				continue;
			//呼叫計算
			line_crosspoint vPoint(newleftSlope[i].line_slope, newleftSlope[i].line_intercept, newrightSlope[j].line_slope, newrightSlope[j].line_intercept);


			if (vPoint.LC_point.x > vpx + vp_range || vPoint.LC_point.x < vpx - vp_range || vPoint.LC_point.y > vpy + vp_range || vPoint.LC_point.y < vpy - vp_range)
				continue;
			else
			{
				//生成左右線段的最低點
				CvPoint left_low_point, right_low_point;
				if (newleftSlope[i].line_point_1->y > newleftSlope[i].line_point_2->y)
					left_low_point = *newleftSlope[i].line_point_1;
				else
					left_low_point = *newleftSlope[i].line_point_2;

				if (newrightSlope[j].line_point_1->y > newrightSlope[j].line_point_2->y)
					right_low_point = *newrightSlope[j].line_point_1;
				else
					right_low_point = *newrightSlope[j].line_point_2;
				//畫出消失點與對應的線段
				//點
				cvCircle(Ori_pic, vPoint.LC_point, 4, CV_RGB(255, 0, 0), 1);
				//左
				cvLine(Ori_pic, vPoint.LC_point, left_low_point, CV_RGB(255, 0, 255), 2);
				//右
				cvLine(Ori_pic, vPoint.LC_point, right_low_point, CV_RGB(255, 0, 255), 2);
			}
		}

	}
	//清除車道線斜率記錄vector
	vector <line_property> Nls, Nrs;
	newleftSlope.clear();
	newrightSlope.clear();
	newleftSlope.swap(Nls);
	newrightSlope.swap(Nrs);
}






