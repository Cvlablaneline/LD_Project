#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <math.h>
#include "Vanishing Point.h"
#include "LD_LogSaveHelper.h"

#ifndef __MacOpenCV__Find_the_best_lines__
#define __MacOpenCV__Find_the_best_lines__

//find_the_best_line回傳資料結構
class FTBL
{
public:
    CvPoint FTBL_Point_L;
    CvPoint FTBL_Point_R;
    bool FTBL_isCouldNotDetermine;
    bool FTBL_stopPic;
    FTBL();
    FTBL(CvPoint Point_data_L, CvPoint Point_data_R);
    FTBL(CvPoint Point_data_L, CvPoint Point_data_R, bool _isCouldNotDetermine);
    FTBL(CvPoint Point_data_L, CvPoint Point_data_R, bool _isCouldNotDetermine, bool _stopPic);
};

//記錄消失點範圍區域
class VPZone
{
public:
    CvPoint L_up,L_down,R_up,R_down,vp;
    int top,bottom,left,right;
    VPZone(){};
    VPZone(CvPoint lup,CvPoint ldown,CvPoint rup,CvPoint rdown,CvPoint VP_Point);    
};
class IsMatchVPRange
{
public:
    bool left,top,btm,right;
    int count;
    IsMatchVPRange();
    int countAdd(int _top,int _btm,int _left,int _right);
};
//紀錄前後張選點資訊
class FTBLRecoder {
public:
    FTBLRecoder(){};
    CvPoint Org_1,Org_2;
    CvPoint up_Point,down_point;
    double slope;
    double confidence;
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
    int GroupLinePointNum;
    IsMatchVPRange MatchVPRange;
    double angle;
    double group_line_slope;
    double group_slope;
    double group_slope_angle;
    line_group(vector<line_property> *line,int LineNum);
    void update(int LineNum);
    double calculate_the_angle(CvPoint Vanishing_Point);
    //計算信心指數
    double confidence;
    double countConfidence();
    
    double confidence_groupHoughPoint;
    double groupHoughPointForConfidence(int AllThisSideGroupHoughPoints);
    
    double confidence_groupHoughPoint_2;
    double groupHoughPoint_2_ForConfidence(double avgHough);
    
    double confidence_avgGroupScore;
    double avgGroupScoreForConfidence(int sumGroupScore, int groupNum);
    
    double groupLineDistanceOfVP;
    CvPoint groupLineDistanceOfVPPoint;
    double countgroupLineDistanceOfVP(CvPoint VPoint);
    
    double confidence_groupLineDistanceOfVP;
    double confidence_groupLineDistanceOfVP_sigmoid;
    double groupLineDistanceOfVPForConfidence(double centerDOVPValue);
    
    //確認是否為該群線段
    bool CompareLine2Group (int LineNum, int range, double lenRange);
    double countDistance (CvPoint line1,CvPoint line2);
    //判斷線段是否通過VPZone
    bool CompareGroupLineInVPZone(VPZone *vpzone);
};

//Sigmoid Function
double Sigmoid_Function(double x);
double Sigmoid_Function(double x, double a, double b);

//計算法線
double CountAngleForNormal(CvPoint point1, CvPoint point2);
double countNormal(vector<line_property> *line, int LineNum);

//分群Vector sort
bool sort_for_line_group_class(const line_group a, const line_group b);
bool sort_for_line_group_class_ANGLE(const line_group a, const line_group b);

//畫出範圍框
VPZone draw_VPoint(IplImage* img, int x, int y, int vp_range);

//找出最佳車道線 (原始彩圖,完整Canny,消失點位置,消失點周圍範圍）
FTBL FindTheBestLines(IplImage* Ori_pic,vector<line_property> *AllHLineSlope, CvPoint vpfnpoint, int vp_range,vector<CvPoint> *vpRecoder,IplImage* showLineGroup_line,int pictureNumber, GroupPointLogs* gplogs, deque<FTBLRecoder> *FTBLRecoder_L, deque<FTBLRecoder> *FTBLRecoder_R);

//計算延伸線段座標
CvPoint Calculation_extension(CvPoint Vanishing_Point, double slope, int y);
CvPoint Calculation_extension_2(CvPoint _Point,double slope,int x);
//算截距
double calculate_intercept(CvPoint point_1,double line_slope);

//比對車道線正確性
int CompareTrueLD (CvPoint FPoint_Lup, CvPoint FPoint_Ldown,CvPoint FPoint_Rup, CvPoint FPoint_Rdown, string DB_URL, int pic_Number);
bool doCompareTrueLD (CvPoint FPoint_up, CvPoint FPoint_down,CvPoint trueLDPoint_up,CvPoint trueLDPoint_down);
double CountAngleForCompareTrueLD(CvPoint point1, CvPoint point2);
double countNormalForCompareTrueLD(line_property *_line_property);
#endif /* defined(__MacOpenCV__Find_the_best_lines__) */
