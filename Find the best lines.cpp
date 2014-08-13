//
//  Find the best lines.cpp
//
//  Created by Chienfu Huang on 2014/5/22.
//  Copyright (c) 2014年 Chienfu Huang. All rights reserved.
//
#include "stdafx.h"
#include "Find the best lines.h"
#include "Vanishing Point.h"

using namespace std;

#define Line_Group_Range 35 

//find_the_best_line回傳資料結構
FTBL::FTBL()
{
		FTBL_Point_L = cvPoint(0, 0);
		FTBL_Point_R = cvPoint(0, 0);
}
FTBL::FTBL(CvPoint Point_data_L, CvPoint Point_data_R) : FTBL_Point_L(Point_data_L), FTBL_Point_R(Point_data_R){}

//線段分群類別
line_group::line_group(CvPoint firstPoint)
{
	angle = 0.0;
	group_line_slope = 0.0;
	GroupPoint = firstPoint;
	GroupScore = 1;
}

void line_group::update(CvPoint newPoint)
{
	GroupPoint.x = (GroupPoint.x + newPoint.x) / 2;
	GroupPoint.y = (GroupPoint.y + newPoint.y) / 2;
	GroupScore++;
}

double line_group::calculate_the_angle(CvPoint Vanishing_Point)
{
	group_line_slope = line_property(Vanishing_Point, this->GroupPoint).line_slope;
	this->angle = atan(group_line_slope);
	//cout << "\t\t\t" << "angle is : " << angle << ","<< GroupScore <<endl;
	return angle;
}

bool sort_for_line_group_class(const line_group a, const line_group b)
{
	return a.GroupScore < b.GroupScore;
}

bool sort_for_line_group_class_ANGLE(const line_group a, const line_group b)
{
	return a.angle < b.angle;
}

//計算延伸線段座標
CvPoint Calculation_extension(CvPoint Vanishing_Point,double slope,int y)
{
	return cvPoint(((Vanishing_Point.y - y) - (slope*Vanishing_Point.x)) / -(slope),y);
}

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
FTBL FindTheBestLines(IplImage* Ori_pic, IplImage* FullCanny, CvPoint vpfnpoint, int vp_range)
{
	int vpx = vpfnpoint.x;
	int vpy = vpfnpoint.y;

	//分類記錄霍夫轉換後車道線斜率方向
	vector <line_property> newleftSlope, newrightSlope;

	//線段分群
	vector <line_group> line_group_data_l;
	vector <line_group> line_group_data_r;

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
				CvPoint left_low_point, right_low_point;
				//生成左右線段的最低點
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
				////cvCircle(Ori_pic, vPoint.LC_point, 4, CV_RGB(255, 0, 0), 1);
				//左
				////cvLine(Ori_pic, vPoint.LC_point, left_low_point, CV_RGB(255, 0, 255), 2);
				//右
				////cvLine(Ori_pic, vPoint.LC_point, right_low_point, CV_RGB(255, 0, 255), 2);
				if (line_group_data_l.size() < 1 || line_group_data_r.size() < 1)
				{
					line_group tmp(left_low_point);
					line_group_data_l.push_back(tmp);
					line_group tmp2(right_low_point);
					line_group_data_r.push_back(tmp2);
				}
				else
				{
					//左邊群
					bool isPass = false;
					for (int i = 0; i < line_group_data_l.size(); i++)
					{
						isPass = false;
						if (abs(line_group_data_l[i].GroupPoint.x - left_low_point.x) > Line_Group_Range || abs(line_group_data_l[i].GroupPoint.y - left_low_point.y) > Line_Group_Range)
						{
							continue;
						}
						else
						{
							line_group_data_l[i].update(left_low_point);
							isPass = true;
							break;
						}
					}
					if (isPass == false)
					{
						line_group tmp(left_low_point);
						line_group_data_l.push_back(tmp);
					}
					//右邊群
					isPass = false;
					for (int i = 0; i < line_group_data_r.size(); i++)
					{
						if (abs(line_group_data_r[i].GroupPoint.x - right_low_point.x) > Line_Group_Range || abs(line_group_data_r[i].GroupPoint.y - right_low_point.y) > Line_Group_Range)
						{
							continue;
						}
						else
						{
							line_group_data_r[i].update(right_low_point);
							isPass = true;
							break;
						}
					}
					if (isPass == false)
					{
						line_group tmp(right_low_point);
						line_group_data_r.push_back(tmp);
					}
				}
			}
		}
	}
	CvPoint maxLeft, maxRight;
	if (line_group_data_l.size() < 1 || line_group_data_r.size() < 1 )
	{
		cout << "沒有車道線對應資料!!" << endl;
		return FTBL(cvPoint(0,0),cvPoint(0,0));
	}
	else
	{
		//分群計算與消失點的個別角度
		vector<line_group>::iterator lgd_first = line_group_data_l.begin();
		for (lgd_first; lgd_first != line_group_data_l.end(); lgd_first++)
			lgd_first->calculate_the_angle(vpfnpoint);
		lgd_first = line_group_data_r.begin();
		for (lgd_first; lgd_first != line_group_data_r.end(); lgd_first++)
			lgd_first->calculate_the_angle(vpfnpoint);

		//以角度排序
		sort(line_group_data_l.begin(), line_group_data_l.end(), sort_for_line_group_class_ANGLE);
		//maxLeft = line_group_data_l[0].GroupPoint;
		maxLeft = Calculation_extension(vpfnpoint, line_group_data_l[0].group_line_slope, Ori_pic->height);

		sort(line_group_data_r.begin(), line_group_data_r.end(), sort_for_line_group_class_ANGLE);
		//maxRight = line_group_data_r[line_group_data_r.size()-1].GroupPoint;
		maxRight = Calculation_extension(vpfnpoint, line_group_data_r[line_group_data_r.size() - 1].group_line_slope, Ori_pic->height);

		/*
		//以分數排序
		sort(line_group_data_l.begin(), line_group_data_l.end(), sort_for_line_group_class);
		maxLeft = line_group_data_l[line_group_data_l.size() - 1].GroupPoint;
		
		sort(line_group_data_r.begin(), line_group_data_r.end(), sort_for_line_group_class);
		maxRight = line_group_data_r[line_group_data_r.size() - 1].GroupPoint;
		*/

		/*
		vector<line_group>::iterator psfirst = line_group_data_l.begin();
		for (psfirst; psfirst != line_group_data_l.end(); psfirst++)
			cout <<"左線分數" <<psfirst->GroupScore << endl;
		vector<line_group>::iterator psfirst2 = line_group_data_r.begin();
		for (psfirst2; psfirst2 != line_group_data_r.end(); psfirst2++)
			cout << "右線分數" << psfirst2->GroupScore << endl;
		*/
		

		//點
		cvCircle(Ori_pic, vpfnpoint, 4, CV_RGB(0, 0, 255), 1);
		//左
		cvLine(Ori_pic, vpfnpoint, maxLeft, CV_RGB(0, 0, 255), 2);
		//右
		cvLine(Ori_pic, vpfnpoint, maxRight, CV_RGB(0, 0, 255), 2);
	}

	cout << "左群有: " << line_group_data_l.size() << "\t右群有: " << line_group_data_r.size() << endl;
	cout << "左群最大點: " << maxLeft.x << "," << maxLeft.y << "\t右群最大點: " << maxRight.x << "," << maxRight.y << endl;
	//cvWaitKey();
	//清除車道線斜率記錄vector
	vector <line_property> Nls, Nrs;
	newleftSlope.clear();
	newrightSlope.clear();
	newleftSlope.swap(Nls);
	newrightSlope.swap(Nrs);

	//清除線段分群記錄vector
	vector <line_group> Nllgd,Nlrgd;
	line_group_data_l.clear();
	line_group_data_l.swap(Nllgd);
	line_group_data_r.clear();
	line_group_data_r.swap(Nlrgd);

	return FTBL(maxLeft ,maxRight);
}


