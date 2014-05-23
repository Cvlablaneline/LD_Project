#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <vector>
#include "Vanishing Point.h"
using namespace std;

//class of line_property
line_property::line_property(CvPoint& point_1, CvPoint& point_2) :line_point_1(&point_1), line_point_2(&point_2)
{
	//斜率
	line_slope = (double)(point_1.y - point_2.y) / (point_1.x - point_2.x);

	//截距
	line_intercept = (double)(point_1.y - line_slope * point_1.x);

}

//class of line_crosspoint
line_crosspoint::line_crosspoint(double slope1, double intercept1, double slope2, double intercept2)
{
	//兩條線解聯立方程式，找出交點P(ptX,ptY)：
	LC_point.x = -(intercept1 - intercept2) / (slope1 - slope2);
	LC_point.y = intercept1 + (slope1 * LC_point.x);
	LC_score = 0;
}

CvPoint find_Vanishing_Point(IplImage* Canny, IplImage* Ori_pic, vector<CvPoint> *pointSave)
{
	vector<line_property> all_Slope;
	vector<line_crosspoint> all_point;
	CvMemStorage* storage_DThrSmo = cvCreateMemStorage(0);		// 霍夫線轉換
	CvSeq* lines_DThrSmo = cvHoughLines2(Canny, storage_DThrSmo, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 50, 10);
	//輸入,儲存,變換方法,距離精度,角度精度,臨界值,最小長度,最大間隔

	for (int i = 0; i < lines_DThrSmo->total; i++)
	{
		CvPoint* ptThis = (CvPoint*)cvGetSeqElem(lines_DThrSmo, i);
		if (ptThis[0].x - ptThis[1].x == 0)
			continue;

		line_property line_info_temp(ptThis[0], ptThis[1]);
		bool ok = true;

		//過濾水平線
		if (!(abs(line_info_temp.line_slope) < 0.18)) //0.18 ~= tan 10度
		{
			if (i != 0)
			{
				for (int q = 0; q < all_Slope.size(); q++)
				{
					if (abs(line_info_temp.line_slope - all_Slope[q].line_slope) < 0.01 && abs(line_info_temp.line_intercept - all_Slope[q].line_intercept) < 1)
					{
						ok = false;
						break;
					}
				}
			}
		}
		else
		{
			ok = false;
		}

		//全部儲存
		if (ok == true)
		{
			all_Slope.push_back(line_info_temp);
		}
		else
			continue;
	}

	int count = 0;
	for (int f = 0; f < all_Slope.size(); f++)
	{
		for (int s = f + 1; s < all_Slope.size(); s++)
		{
			//假如兩線斜率差異不大，就不算交點了
			if (abs(all_Slope[f].line_slope - all_Slope[s].line_slope)<0.01)
				continue;
			//line_crosspoint(double slope1, double intercept1, double slope2, double intercept2)
			line_crosspoint LC_Temp((double)all_Slope[f].line_slope, (double)all_Slope[f].line_intercept, (double)all_Slope[s].line_slope, (double)all_Slope[s].line_intercept);
			if (LC_Temp.LC_point.x >= Canny->width || LC_Temp.LC_point.y >= Canny->height || LC_Temp.LC_point.x < 0 || LC_Temp.LC_point.y < 0)
				continue;
			all_point.push_back(LC_Temp);
			count++;
		}
	}

	if (all_point.size() < 1)
	{
		CvPoint no_p;
		no_p.x = NULL;
		no_p.y = NULL;
		return no_p;
	}

	//將交點的資訊寫入圖像中
	IplImage* IntegralImg = cvCreateImage(cvSize(Canny->width, Canny->height), IPL_DEPTH_8U, 1);
	unsigned char* IntegralImgdata = (unsigned char*)IntegralImg->imageData;

	for (int i = 0; i < all_point.size(); i++)
	{
		int temp = all_point[i].LC_point.y * Canny->width + all_point[i].LC_point.x;
		if ( all_point[i].LC_point.x < 0 || all_point[i].LC_point.y < 0 ||
			 all_point[i].LC_point.x >= Canny->width ||
			 all_point[i].LC_point.y >= Canny->height ) {
			int a=0;
			a++;
		}
		IntegralImgdata[temp] += 20;
	}




	CvMat *sumMat;
	//建立儲存影像積分的陣列，格式可為32-bit整數或64F浮點數
	sumMat = cvCreateMat(Canny->height + 1, Canny->width + 1, CV_64FC1);

	//計算積分影像
	cvIntegral(IntegralImg, sumMat);
	//積分影像資訊類別
	class max_info {
	public:
		float max;
		float first_x;
		float first_y;
		float sec_x;
		float sec_y;
		float score;

		max_info() {
			max = 0.0;
			first_x = 0.0;
			first_y = 0.0;
			sec_x = 0.0;
			sec_y = 0.0;
			score = 0.0;
		}
		void clear_score()
		{
			max = 0.0;
			score = 0.0;
		}
	};

	max_info Integral_info;

	for (int y = 0; y < Canny->height - 40; y++)
	{
		for (int x = 0; x < Canny->width - 40; x++)
		{
			Integral_info.score = cvmGet(sumMat, y + 40, x + 40) + cvmGet(sumMat, y, x) - cvmGet(sumMat, y, x + 40) - cvmGet(sumMat, y + 40, x);
			if (Integral_info.score > Integral_info.max)
			{
				Integral_info.max = Integral_info.score;
				Integral_info.first_x = x;
				Integral_info.first_y = y;
			}
		}
	}

	Integral_info.clear_score();
	for (int i = Integral_info.first_y; i < Integral_info.first_y + 40 - 2; i++)
	{
		for (int j = Integral_info.first_x; j < Integral_info.first_x + 40 - 2; j++)
		{
			Integral_info.score = cvmGet(sumMat, i + 2, j + 2) + cvmGet(sumMat, i, j) - cvmGet(sumMat, i, j + 2) - cvmGet(sumMat, i + 2, j);
			if (Integral_info.score > Integral_info.max)
			{
				Integral_info.max = Integral_info.score;
				Integral_info.sec_x = j - 1;
				Integral_info.sec_y = i - 1;
			}
		}
	}

	//釋放IntegralImg
	cvReleaseImage(&IntegralImg);
	//CvMat刪除、釋放
	//if (sumMat != NULL)
	//cvDecRefData(sumMat);
	cvReleaseMat(&sumMat);

	return cvPoint(Integral_info.sec_x, Integral_info.sec_y);
	//CvPoint foundPoint = check_point(cvPoint(Integral_info.sec_x, Integral_info.sec_y), pointSave);
	//cout << "儲存的消失點數量:" << (*pointSave).size() << endl;

	//return foundPoint;
}

//check point
CvPoint check_point(CvPoint newPoint, vector<CvPoint> *pointSave)
{
	static int error_count = 0;
	cout << "error_count: " << error_count << endl;

	class count_check_point
	{
	public:
		CvPoint save_point;
		int score;
		count_check_point(CvPoint Point)
		{
			score = 1;
			save_point = Point;
		}
		void hit_it(CvPoint hit_point)
		{
			save_point.x = (save_point.x + hit_point.x) / 2;
			save_point.y = (save_point.y + hit_point.y) / 2;
			score++;
		}
	};
	//記錄10張圖片的消失點坐標
	if ((*pointSave).size() < 10)
	{
		(*pointSave).push_back(newPoint);
		return newPoint;
	}
	else
	{
		vector<count_check_point> ckpoint;
		bool isPass = false;
		for (int i = 0; i < (*pointSave).size(); i++)
		{
			isPass = false;
			if (ckpoint.size() < 1)
			{
				count_check_point tmp((*pointSave)[i]);
				ckpoint.push_back(tmp);
				continue;
			}
			for (int j = 0; j < ckpoint.size(); j++)
			{
				if (abs(ckpoint[j].save_point.x - (*pointSave)[i].x) > 40 || abs(ckpoint[j].save_point.y - (*pointSave)[i].y) > 40)
				{
					continue;
				}
				else
				{
					ckpoint[j].score++;
					ckpoint[j].hit_it((*pointSave)[i]);
					isPass = true;
					break;
				}
			}
			if (isPass == false)
			{
				count_check_point tmp((*pointSave)[i]);
				ckpoint.push_back(tmp);
			}
		}
		cout << "組別有: " << ckpoint.size() << "組" << endl;

		cvWaitKey();

		CvPoint MaxPoint;
		int Max = 0;
		for (int i = 0; i < ckpoint.size(); i++)
		{
			if (ckpoint[i].score > Max)
			{
				MaxPoint = ckpoint[i].save_point;
			}
		}
		vector<CvPoint>::iterator psfirst = (*pointSave).begin();
		(*pointSave).erase(psfirst);
		(*pointSave).push_back(newPoint);
		cout << "差距:" << abs(newPoint.x - MaxPoint.x) << "," << abs(newPoint.y - MaxPoint.y) << endl;
		if (abs(newPoint.x - MaxPoint.x) > 40 || abs(newPoint.y - MaxPoint.y) > 40)
		{
			error_count++;
			if (error_count + 2 >= (*pointSave).size())
			{
				error_count = 0;
				(*pointSave).clear();
				return newPoint;
			}
			cout << "old:" << (*pointSave).size() - 2 - (error_count - 1) << endl;
			return (*pointSave)[(*pointSave).size() - 2 - (error_count - 1)];
		}
		else
		{
			error_count = 0;
			cout << "new" << endl;
			return newPoint;
		}
	}

}




