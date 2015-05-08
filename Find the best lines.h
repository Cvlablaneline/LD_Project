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
protected:
    vector<int> LineNumTable;
    vector<line_property> *AllLineData;
    double group_Normal_sum;
    double group_Normal;
public:
    CvPoint GroupPoint;
    CvPoint GroupPoint_up;
    int GroupScore;
    double angle;
    double group_line_slope;
    double group_slope;
    double group_slope_angle;
    line_group(CvPoint firstPoint);
    line_group(CvPoint firstPoint,double firstslope);
    line_group(vector<line_property> *line,int LineNum);
    void update(CvPoint newPoint);
    void update(CvPoint newPoint,double newslope);
    void update(int LineNum);
    double calculate_the_angle(CvPoint Vanishing_Point);
    
    //確認是否為該群線段
    bool CompareLine2Group (int LineNum, int range, double lenRange);
    double countDistance (CvPoint line1,CvPoint line2);
};

//計算法線
double CountAngleForNormal(CvPoint point1, CvPoint point2);
double countNormal(vector<line_property> *line, int LineNum);

//分群Vector sort
bool sort_for_line_group_class(const line_group a, const line_group b);
bool sort_for_line_group_class_ANGLE(const line_group a, const line_group b);

//畫出範圍框
void draw_VPoint(IplImage* img, int x, int y, int vp_range);

//找出最佳車道線 (原始彩圖,完整Canny,消失點位置,消失點周圍範圍）
FTBL FindTheBestLines(IplImage* Ori_pic,vector<line_property> *AllHLineSlope, CvPoint vpfnpoint, int vp_range,vector<CvPoint> *vpRecoder,IplImage* showLineGroup_line);

//計算延伸線段座標
CvPoint Calculation_extension(CvPoint Vanishing_Point, double slope, int y);
//算截距
double calculate_intercept(CvPoint point_1,double line_slope);


#endif /* defined(__MacOpenCV__Find_the_best_lines__) */
