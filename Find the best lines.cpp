#define _USE_MATH_DEFINES

#include "Find the best lines.h"
#include "Vanishing Point.h"
#include <string>
#include <sstream>
#include <cmath>
#include "LD_LogSaveHelper.h"
#include "Experimental_tools.hpp"

using namespace std;

#define Line_Group_Range 35
#define Line_Group_lenRange 100

//find_the_best_line回傳資料結構
FTBL::FTBL()
{
    FTBL_Point_L = cvPoint(0, 0);
    FTBL_Point_R = cvPoint(0, 0);
    FTBL_isCouldNotDetermine = false;
}
FTBL::FTBL(CvPoint Point_data_L, CvPoint Point_data_R) : FTBL_Point_L(Point_data_L), FTBL_Point_R(Point_data_R){}
FTBL::FTBL(CvPoint Point_data_L, CvPoint Point_data_R, bool _isCouldNotDetermine) : FTBL_Point_L(Point_data_L), FTBL_Point_R(Point_data_R), FTBL_isCouldNotDetermine(_isCouldNotDetermine){}
FTBL::FTBL(CvPoint Point_data_L, CvPoint Point_data_R, bool _isCouldNotDetermine, bool _stopPic) : FTBL_Point_L(Point_data_L), FTBL_Point_R(Point_data_R), FTBL_isCouldNotDetermine(_isCouldNotDetermine), FTBL_stopPic(_stopPic){}

//記錄消失點範圍區域
VPZone::VPZone(CvPoint lup,CvPoint ldown,CvPoint rup,CvPoint rdown,CvPoint VP_Point)
{
    L_up = lup;
    L_down = ldown;
    R_up = rup;
    R_down = rdown;
    vp = VP_Point;
    top = L_up.y;
    bottom = L_down.y;
    left = L_up.x;
    right = R_up.x;
}
IsMatchVPRange::IsMatchVPRange()
{
    this->top = 0;
    this->btm = 0;
    this->right = 0;
    this->left = 0;
    this->count = 0;
}
int IsMatchVPRange::countAdd(int _top,int _btm,int _left,int _right)
{
    if (_top == 1)
    {
        if(this->top == 0)
        {
            this->top = 1;
            this->count++;
        }
    }
    if (_btm == 1)
    {
        if(this->btm == 0)
        {
            this->btm = 1;
            this->count++;
        }
    }
    if (_left == 1)
    {
        if(this->left == 0)
        {
            this->left = 1;
            this->count++;
        }
    }
    if(_right == 1)
    {
        if(this->right == 0)
        {
            this->right = 1;
            this->count++;
        }
    }
    return count;
}

//線段分群類別
line_group::line_group(vector<line_property> *line,int LineNum)
{
    LineNumTable.push_back(LineNum);
    AllLineData = line;
    angle = 0.0;
    group_line_slope = 0.0;
    group_slope = (*AllLineData)[LineNum].line_slope;
    group_slope_angle = atan(group_slope)*180/3.1415;
    //R
    group_Normal = countNormal(AllLineData, LineNum);
    group_Normal_sum = group_Normal;
    //初始化上下點座標為0
    GroupPoint.x = 0;
    GroupPoint.y = 0;
    GroupPoint_up.x = 0;
    GroupPoint_up.y = 0;
    //記錄點為最低點
    if ((*AllLineData)[LineNum].line_point_1->y > (*AllLineData)[LineNum].line_point_2->y)
    {
        GroupPoint.x = (*AllLineData)[LineNum].line_point_1->x;
        GroupPoint.y = (*AllLineData)[LineNum].line_point_1->y;
        GroupPoint_up.x = (*AllLineData)[LineNum].line_point_2->x;
        GroupPoint_up.y = (*AllLineData)[LineNum].line_point_2->y;
    }
    else
    {
        GroupPoint.x = (*AllLineData)[LineNum].line_point_2->x;
        GroupPoint.y = (*AllLineData)[LineNum].line_point_2->y;
        GroupPoint_up.x = (*AllLineData)[LineNum].line_point_1->x;
        GroupPoint_up.y = (*AllLineData)[LineNum].line_point_1->y;
    }
    //霍夫點數量
    GroupLinePointNum = (*AllLineData)[LineNum].num_line_point;
    //群分數(合併進來的線段個數）
    GroupScore = 1;
}

void line_group::update(int LineNum)
{
    LineNumTable.push_back(LineNum);
    GroupPoint.x = 0;
    GroupPoint.y = 0;
    GroupPoint_up.x = 0;
    GroupPoint_up.y = 0;
    group_slope = 0.0;
    
    for (vector<int>::iterator lnt_first = LineNumTable.begin();
         lnt_first != LineNumTable.end();
         lnt_first++)
    {
        //記錄點為最低點
        if ((*AllLineData)[*lnt_first].line_point_1->y > (*AllLineData)[*lnt_first].line_point_2->y)
        {
            GroupPoint.x += (*AllLineData)[*lnt_first].line_point_1->x;
            GroupPoint.y += (*AllLineData)[*lnt_first].line_point_1->y;
            GroupPoint_up.x += (*AllLineData)[*lnt_first].line_point_2->x;
            GroupPoint_up.y += (*AllLineData)[*lnt_first].line_point_2->y;
        }
        else
        {
            GroupPoint.x += (*AllLineData)[*lnt_first].line_point_2->x;
            GroupPoint.y += (*AllLineData)[*lnt_first].line_point_2->y;
            GroupPoint_up.x += (*AllLineData)[*lnt_first].line_point_1->x;
            GroupPoint_up.y += (*AllLineData)[*lnt_first].line_point_1->y;
        }
//        group_slope += (*AllLineData)[*lnt_first].line_slope;
    }
    
    GroupPoint.x /= LineNumTable.size();
    GroupPoint.y /= LineNumTable.size();
    GroupPoint_up.x /= LineNumTable.size();
    GroupPoint_up.y /= LineNumTable.size();
    
//    group_slope /= (float)LineNumTable.size();
    
    //直接使用Group的新聲成端點重新計算斜率
    group_slope = line_property(GroupPoint,GroupPoint_up).line_slope;
    
    group_slope_angle = atan(group_slope)*180/3.1415;
    //R
    group_Normal_sum += countNormal(AllLineData, LineNum);
    group_Normal = group_Normal_sum / LineNumTable.size();
    //霍夫點數量
    GroupLinePointNum += (*AllLineData)[LineNum].num_line_point;
    //群分數(合併進來的線段個數）
    GroupScore++;
}

double line_group::calculate_the_angle(CvPoint Vanishing_Point)
{
    group_line_slope = line_property(Vanishing_Point, this->GroupPoint).line_slope;
    if (group_line_slope == -1)
    {
        angle = 90;
        return angle;
    }
    this->angle = atan(group_line_slope)*180/3.1415;
    //cout << "\t\t\t" << "angle is : " << angle*180/3.1415 << ","<< GroupScore <<endl;
    return angle;
}

double countNormal(vector<line_property> *line, int LineNum)
{
    //計算(x,0) & (0,Y)
    CvPoint NewLineZeroPoint = cvPoint((-(*line)[LineNum].line_intercept) / (*line)[LineNum].line_slope,(*line)[LineNum].line_intercept);
    
    double Angle = CountAngleForNormal(*(*line)[LineNum].line_point_1, *(*line)[LineNum].line_point_2);
    //double Angle = CountAngleForNormal(cvPoint(NewLineZeroPoint.x,0), cvPoint(0,NewLineZeroPoint.y));
    
    double R = NewLineZeroPoint.x*cos(Angle) + 0 * sin(Angle);
    return R;
}

double CountAngleForNormal(CvPoint point1, CvPoint point2)
{
    double NewAngle;
    double line_slope = line_property(point1, point2).line_slope;
    NewAngle = atan(line_slope);
    return (M_PI_2 + NewAngle);
}

//確認是否為該群線段
/*old
 bool line_group::CompareLine2Group (int LineNum, int range, double lenRange)
 {
 if ((*AllLineData)[LineNum].line_point_1->y > (*AllLineData)[LineNum].line_point_2->y)
 {
 //比較點位置(line_point_1在下)
 if(abs(GroupPoint.x - (*AllLineData)[LineNum].line_point_1->x) <= range
 && abs(GroupPoint.y - (*AllLineData)[LineNum].line_point_1->y) <= range
 && abs(GroupPoint_up.x - (*AllLineData)[LineNum].line_point_2->x) <= range
 && abs(GroupPoint_up.y - (*AllLineData)[LineNum].line_point_2->y) <= range
 )
 {
 if(!(abs(group_slope_angle - (atan((*AllLineData)[LineNum].line_slope)*180/3.1415)) > 6))
 {
 double GroupLineDistance = countDistance(GroupPoint, GroupPoint_up);
 double NewLineDeatance = countDistance(*(*AllLineData)[LineNum].line_point_1, *(*AllLineData)[LineNum].line_point_2);
 if (abs(GroupLineDistance - NewLineDeatance) <= lenRange) {
 return true;
 }
 }
 }
 return false;
 }
 else
 {
 //比較點位置(line_point_2在下)
 if(abs(GroupPoint.x - (*AllLineData)[LineNum].line_point_2->x) <= range
 && abs(GroupPoint.y - (*AllLineData)[LineNum].line_point_2->y) <= range
 && abs(GroupPoint_up.x - (*AllLineData)[LineNum].line_point_1->x) <= range
 && abs(GroupPoint_up.y - (*AllLineData)[LineNum].line_point_1->y) <= range
 )
 {
 if(!(abs(group_slope_angle - (atan((*AllLineData)[LineNum].line_slope)*180/3.1415)) > 6))
 {
 double GroupLineDistance = countDistance(GroupPoint, GroupPoint_up);
 double NewLineDeatance = countDistance(*(*AllLineData)[LineNum].line_point_1, *(*AllLineData)[LineNum].line_point_2);
 if (abs(GroupLineDistance - NewLineDeatance) <= lenRange) {
 return true;
 }
 }
 }
 return false;
 }
 }
 */
bool line_group::CompareLine2Group(int LineNum, int range, double lenRange)
{
    if (!(abs(group_slope_angle - (atan((*AllLineData)[LineNum].line_slope) * 180 / 3.1415)) > 20))
    {
        if (abs(group_Normal - countNormal(AllLineData, LineNum)) <= 30) {
            return true;
        }
    }
    return false;
}
bool line_group::CompareGroupLineInVPZone(VPZone *vpzone)
{
    CvPoint VPZone_Gtop = Calculation_extension(this->GroupPoint,this->group_slope,((*vpzone).top + 1));
    CvPoint VPZone_Gbtm = Calculation_extension(this->GroupPoint,this->group_slope,((*vpzone).bottom - 1));
    CvPoint VPZone_Gleft = Calculation_extension_2(this->GroupPoint, this->group_slope, ((*vpzone).left+1));
    CvPoint VPZone_Gright = Calculation_extension_2(this->GroupPoint, this->group_slope, ((*vpzone).right-1));
                                                   
    if (VPZone_Gtop.x > (*vpzone).L_up.x && VPZone_Gtop.x < (*vpzone).R_up.x)
    {
        this->MatchVPRange.countAdd(1, 0, 0, 0);
    }
    if (VPZone_Gbtm.x > (*vpzone).L_down.x && VPZone_Gbtm.x < (*vpzone).R_down.x)
    {
        this->MatchVPRange.countAdd(0, 1, 0, 0);
    }
    if (VPZone_Gleft.y > (*vpzone).top && VPZone_Gleft.y < (*vpzone).bottom)
    {
        this->MatchVPRange.countAdd(0, 0, 1, 0);
    }
    if (VPZone_Gright.y > (*vpzone).top && VPZone_Gright.y < (*vpzone).bottom)
    {
        this->MatchVPRange.countAdd(0, 0, 0, 1);
    }
    if(this->MatchVPRange.count > 1)
        return true;
    else
        return false;
}
//計算信心指數
double line_group::countConfidence()
{
    confidence = 0.0;
    confidence = confidence_groupHoughPoint*confidence_groupHoughPoint_2*confidence_avgGroupScore*confidence_groupLineDistanceOfVP_sigmoid;
    return confidence;
}
double line_group::groupHoughPointForConfidence(int AllThisSideGroupHoughPoints)
{
    if (AllThisSideGroupHoughPoints <= 0)
    {
        cout << "AllThisSideGroupHoughPoints ERROR!" << endl;
        return -1;
    }
    confidence_groupHoughPoint = (double)GroupLinePointNum / (double)AllThisSideGroupHoughPoints;
    return confidence_groupHoughPoint;
}
double line_group::groupHoughPoint_2_ForConfidence(double avgHough)
{
    confidence_groupHoughPoint_2 = (double)GroupLinePointNum - avgHough;
    confidence_groupHoughPoint_2 = Sigmoid_Function(confidence_groupHoughPoint_2);
    return confidence_groupHoughPoint_2;
}

double line_group::avgGroupScoreForConfidence(int sumGroupScore, int groupNum)
{
    confidence_avgGroupScore = (double)GroupScore - (double)(sumGroupScore/groupNum);
    confidence_avgGroupScore = Sigmoid_Function(confidence_avgGroupScore);
    return confidence_avgGroupScore;
}

double line_group::countgroupLineDistanceOfVP(CvPoint VPoint)
{
    double M1,M2;
    M2 = this->group_slope;
    M1 = (-1.0/M2);
    
    double x,y;
    x = ((VPoint.x * M1 - GroupPoint.x * M2) - VPoint.y + GroupPoint.y)/(M1-M2);
    y = (x - VPoint.x) * M1 + VPoint.y;
    groupLineDistanceOfVPPoint = cvPoint(x, y);
    groupLineDistanceOfVP = countDistance(VPoint, groupLineDistanceOfVPPoint);
    confidence_groupLineDistanceOfVP = 150.0 - groupLineDistanceOfVP;
    return groupLineDistanceOfVP;
}

double line_group::groupLineDistanceOfVPForConfidence(double centerDOVPValue)
{
    confidence_groupLineDistanceOfVP -= centerDOVPValue;
    confidence_groupLineDistanceOfVP_sigmoid = Sigmoid_Function(confidence_groupLineDistanceOfVP);
    return confidence_groupLineDistanceOfVP_sigmoid;
}

//計算長度
double line_group::countDistance (CvPoint line1,CvPoint line2)
{
    return sqrt((pow((line2.x-line1.x),2)+pow((line2.y-line1.y),2)));
}

//自訂排序演算法
bool sort_for_line_group_class_GLDOVP_MinF(const line_group a, const line_group b)
{
    return a.groupLineDistanceOfVP < b.groupLineDistanceOfVP;
}

bool sort_for_line_group_class_GLDOVP_MinF_confidence(const line_group a, const line_group b)
{
    return a.confidence_groupLineDistanceOfVP < b.confidence_groupLineDistanceOfVP;
}

bool sort_for_line_group_class_Confidence(const line_group a, const line_group b)
{
    return a.confidence > b.confidence;
}

bool sort_for_line_group_class_GroupScore(const line_group a, const line_group b)
{
    return a.GroupScore > b.GroupScore;
    
    //以下為測試用
//    return a.confidence > b.confidence;
//    return a.confidence_groupHoughPoint > b.confidence_groupHoughPoint;
//    return a.confidence_groupHoughPoint_2 > b.confidence_groupHoughPoint_2;
//    return a.confidence_avgGroupScore > b.confidence_avgGroupScore;
//    return a.confidence_groupLineDistanceOfVP_sigmoid > b.confidence_groupLineDistanceOfVP_sigmoid;
}

bool sort_for_line_group_class_ANGLE_L(const line_group a, const line_group b)
{
    return a.angle < b.angle;
}
bool sort_for_line_group_class_ANGLE_R(const line_group a, const line_group b)
{
    return a.angle > b.angle;
}

//計算哪個是最高點
int WhoIsTop(vector<CvPoint> *vpRecoder)
{
    if ((*vpRecoder).size() <= 1)//若資料只有1筆直接回傳0
        return 0;
    
    int top = 0;
    int temp = (*vpRecoder)[0].y;
    for (int i = 1; i < (*vpRecoder).size(); i++)
    {
        if (temp > (*vpRecoder)[i].y)
        {
            top = i;
            temp = (*vpRecoder)[i].y;
        }
    }
    return top;
}
//計算延伸線段座標
//計算(X0,固定y)
CvPoint Calculation_extension(CvPoint _Point,double slope,int y)
{
    return cvPoint(((_Point.y - y) - (slope*_Point.x)) / -(slope),y);
}
//計算(固定x,Y0)
CvPoint Calculation_extension_2(CvPoint _Point,double slope,int x)
{
    return cvPoint(x, (slope * (x - _Point.x)) + _Point.y);
}

//算截距
double calculate_intercept(CvPoint point_1,double line_slope)
{
    return (double)(point_1.y - line_slope * point_1.x);
}
//畫出範圍框
VPZone draw_VPoint(IplImage* img, int x, int y, int vp_range)
{
    CvPoint VPoint, vline[4];
    VPoint.x = x;
    VPoint.y = y;
    cvCircle(img, VPoint, 4, CV_RGB(0, 255, 255), 1);
    //L_UP
    vline[0].x = x - vp_range;
    vline[0].y = y - vp_range;
    //R_UP
    vline[1].x = x + vp_range;
    vline[1].y = y - vp_range;
    //L_DOWN
    vline[2].x = x - vp_range;
    vline[2].y = y + vp_range;
    //R_DOWN
    vline[3].x = x + vp_range;
    vline[3].y = y + vp_range;
    cvLine(img, vline[0], vline[1], CV_RGB(255, 0, 255), 2);
    cvLine(img, vline[1], vline[3], CV_RGB(255, 0, 255), 2);
    cvLine(img, vline[0], vline[2], CV_RGB(255, 0, 255), 2);
    cvLine(img, vline[2], vline[3], CV_RGB(255, 0, 255), 2);
    
    return VPZone(vline[0],vline[2],vline[1],vline[3],VPoint);
}

//Sigmoid Function
double Sigmoid_Function(double x)
{
    return 1.0/(1.0+exp((-x)));
}
double Sigmoid_Function(double x, double a, double b)
{
    return 1.0/(1.0+exp((-a*x)+(a*b)));
}

//找出最佳車道線
FTBL FindTheBestLines(IplImage* Ori_pic,vector<line_property> *AllHLineSlope, CvPoint vpfnpoint, int vp_range,vector<CvPoint> *vpRecoder,IplImage* showLineGroup_line,int pictureNumber, GroupPointLogs* gplogs, deque<FTBLRecoder> *FTBLRecoder_L, deque<FTBLRecoder> *FTBLRecoder_R)
{
    //畫出所有Hough線段
    gplogs->drawAllHoughLines(AllHLineSlope);
    
#define drawGroupLine //是否畫出線群的線段
    
    cout << "FTBL得到的霍夫轉換線斷數量：" << (*AllHLineSlope).size() << endl;
    
    //記錄消失點範圍＆畫出消失點範圍框
    VPZone vpzone;
    if (vpfnpoint.x != 0.0 || vpfnpoint.y != 0.0)
        vpzone = draw_VPoint(Ori_pic, vpfnpoint.x, vpfnpoint.y, vp_range);
    
    //線段分群
    vector <line_group> line_group_data_l;
    vector <line_group> line_group_data_r;
    
    //消失點記錄（水平線紀錄）
    if ((*vpRecoder).size() < 3)
    {
        (*vpRecoder).push_back(vpfnpoint);
    }
    else
    {
        (*vpRecoder).push_back(vpfnpoint);
        (*vpRecoder).erase((*vpRecoder).begin());
    }
    
    //計算水平線紀錄最高點(y最小)
    int topVP = WhoIsTop(vpRecoder);
    cout << "水平線y軸使用:" << (*vpRecoder)[topVP].y << endl;
    
    //文字顯示設定
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 0.5f, 0.5f, 0, 1, CV_AA);
    stringstream lg_score;
    
    double left_hough_lineG_point = 0;
    double right_hough_lineG_point = 0;
    
    for (int AHLSNum = 0; AHLSNum < (*AllHLineSlope).size(); AHLSNum++)
    {
        if ((*AllHLineSlope)[AHLSNum].line_slope == 0)
            continue;
        //左邊
        if ((*AllHLineSlope)[AHLSNum].line_slope < 0 && (*AllHLineSlope)[AHLSNum].line_point_1->x <= vpfnpoint.x && (*AllHLineSlope)[AHLSNum].line_point_2->x <= vpfnpoint.x)
        {
            left_hough_lineG_point += (*AllHLineSlope)[AHLSNum].num_line_point;
            if (line_group_data_l.size() < 1)
            {
                line_group tmp(AllHLineSlope,AHLSNum);
                line_group_data_l.push_back(tmp);
            }
            else
            {
                bool isPass = false;
                for (int i = 0; i < line_group_data_l.size(); i++)
                {
                    isPass = line_group_data_l[i].CompareLine2Group(AHLSNum, Line_Group_Range,Line_Group_lenRange);
                    if (isPass)
                    {
                        line_group_data_l[i].update(AHLSNum);
                        break;
                    }
                }
                if (isPass == false)
                {
                    line_group tmp(AllHLineSlope,AHLSNum);
                    line_group_data_l.push_back(tmp);
                }
            }
            
        }
        else if ((*AllHLineSlope)[AHLSNum].line_slope > 0 && (*AllHLineSlope)[AHLSNum].line_point_1->x >= vpfnpoint.x && (*AllHLineSlope)[AHLSNum].line_point_2->x >= vpfnpoint.x) //右邊
        {
            right_hough_lineG_point += (*AllHLineSlope)[AHLSNum].num_line_point;
            if (line_group_data_r.size() < 1)
            {
                line_group tmp(AllHLineSlope,AHLSNum);
                line_group_data_r.push_back(tmp);
            }
            else
            {
                bool isPass = false;
                for (int i = 0; i < line_group_data_r.size(); i++)
                {
                    isPass = line_group_data_r[i].CompareLine2Group(AHLSNum, Line_Group_Range,Line_Group_lenRange);
                    if (isPass)
                    {
                        line_group_data_r[i].update(AHLSNum);
                        break;
                    }
                }
                if (isPass == false)
                {
                    line_group tmp(AllHLineSlope,AHLSNum);
                    line_group_data_r.push_back(tmp);
                }
            }
            
        }
        
    }
    cout << "群總數:" << line_group_data_l.size()+line_group_data_r.size() << endl;
    CvPoint maxLeft_down,maxLeft_up, maxRight_down, maxRight_up;
    if (line_group_data_l.size() < 1 || line_group_data_r.size() < 1 )
    {
        cout << "沒有車道線對應資料!!" << endl;
        return FTBL(cvPoint(0,0),cvPoint(0,0));
    }
    else
    {
        //分群計算與消失點的個別角度
        vector<line_group>::iterator lgd_first = line_group_data_l.begin();
        for (lgd_first; lgd_first != line_group_data_l.end();)
        {
            //刪除高於消失點的資料
            if (lgd_first->GroupPoint.y <= (*vpRecoder)[topVP].y)
            {
                lgd_first = line_group_data_l.erase(lgd_first);
            }
            else
            {
                lgd_first->calculate_the_angle(vpfnpoint);
                ++lgd_first;
            }
            
        }
        lgd_first = line_group_data_r.begin();
        for (lgd_first; lgd_first != line_group_data_r.end(); )
        {
            //刪除高於消失點的資料
            if (lgd_first->GroupPoint.y <= (*vpRecoder)[topVP].y)
            {
                lgd_first = line_group_data_r.erase(lgd_first);
            }
            else
            {
                lgd_first->calculate_the_angle(vpfnpoint);
                ++lgd_first;
            }
        }
        
        if (line_group_data_l.size() < 1 || line_group_data_r.size() < 1 )
        {
            cout << "刪除高於消失點的群後，沒有車道線對應資料!!" << endl;
            return FTBL(cvPoint(0,0),cvPoint(0,0));
        }
/*-------CompareGroupLineInVPZone(記錄群線段是否有通過ＶＰ周圍範圍之內)------*/
        for(int p = 0; p < line_group_data_l.size(); p++)
        {
            line_group_data_l[p].CompareGroupLineInVPZone(&vpzone);
        }
        for(int p = 0; p < line_group_data_r.size(); p++)
        {
            line_group_data_r[p].CompareGroupLineInVPZone(&vpzone);
        }
        
/*-------------------------------------------------------------------------------------------*/
/*計算信心指數*/
        int allHoughPointNum = left_hough_lineG_point+right_hough_lineG_point;
        double AvgHough = (double)left_hough_lineG_point / line_group_data_l.size();
        vector<double> confidence_groupLineDistanceOfVP_tmpArray_L,confidence_groupLineDistanceOfVP_tmpArray_R;
        
        //取得左右群的總分數
        int leftGroupScoreSum = 0;
        int rightGroupScoreSum = 0;
        for (int iCount = 0; iCount < line_group_data_l.size(); iCount++)
        {
            leftGroupScoreSum += line_group_data_l[iCount].GroupScore;
        }
        
        for (int iCount = 0; iCount < line_group_data_r.size(); iCount++)
        {
            rightGroupScoreSum += line_group_data_r[iCount].GroupScore;
        }
        
        //L
        for (int FCount = 0; FCount < line_group_data_l.size(); FCount++)
        {
            line_group_data_l[FCount].countgroupLineDistanceOfVP(vpfnpoint);
            confidence_groupLineDistanceOfVP_tmpArray_L.push_back(line_group_data_l[FCount].confidence_groupLineDistanceOfVP);
            
            //            /*==============GroupLine Distance Of VP Logs==============*/
            //            stringstream tmpSS;
            //            tmpSS.str("");
            //            tmpSS.clear();
            //            tmpSS << line_group_data_l[vpDCount].groupLineDistanceOfVP;
            //            (*gplogs).putText(4, tmpSS.str(), cvPoint(line_group_data_l[vpDCount].GroupPoint.x-2, line_group_data_l[vpDCount].GroupPoint.y), CV_RGB(255,255,255));
            //            /*=========================================================*/
            
            line_group_data_l[FCount].groupHoughPointForConfidence(left_hough_lineG_point);
            line_group_data_l[FCount].groupHoughPoint_2_ForConfidence(AvgHough);
            line_group_data_l[FCount].avgGroupScoreForConfidence(leftGroupScoreSum, (int)line_group_data_l.size());
        }
        sort(confidence_groupLineDistanceOfVP_tmpArray_L.begin(),confidence_groupLineDistanceOfVP_tmpArray_L.end());
        
        int CenterDOVPValue = 0;
        if (confidence_groupLineDistanceOfVP_tmpArray_L.size()%2 == 0)
        {
            int tmpv = confidence_groupLineDistanceOfVP_tmpArray_L.size() / 2;
            CenterDOVPValue = (confidence_groupLineDistanceOfVP_tmpArray_L[tmpv] + confidence_groupLineDistanceOfVP_tmpArray_L[tmpv-1])/2;
        }
        else
        {
            int tmpv = confidence_groupLineDistanceOfVP_tmpArray_L.size() / 2;
            CenterDOVPValue = confidence_groupLineDistanceOfVP_tmpArray_L[tmpv];
        }
        
        for (int FCount = 0; FCount < line_group_data_l.size(); FCount++)
        {
            //計算左邊群與消失點距離之信心指數
            line_group_data_l[FCount].groupLineDistanceOfVPForConfidence(CenterDOVPValue);
            
            //計算左邊群的總信心指數
            line_group_data_l[FCount].countConfidence();
        }
        
        //R
        AvgHough = (double)right_hough_lineG_point / line_group_data_r.size();
        for (int FCount = 0; FCount < line_group_data_r.size(); FCount++)
        {
            line_group_data_r[FCount].countgroupLineDistanceOfVP(vpfnpoint);
            confidence_groupLineDistanceOfVP_tmpArray_R.push_back(line_group_data_r[FCount].confidence_groupLineDistanceOfVP);
            
            //            /*==============GroupLine Distance Of VP Logs==============*/
            //            stringstream tmpSS;
            //            tmpSS.str("");
            //            tmpSS.clear();
            //            tmpSS << line_group_data_r[vpDCount].groupLineDistanceOfVP;
            //            (*gplogs).putText(4, tmpSS.str(), cvPoint(line_group_data_r[vpDCount].GroupPoint.x-2, line_group_data_r[vpDCount].GroupPoint.y), CV_RGB(200,128,128));
            //            /*=========================================================*/
            
            line_group_data_r[FCount].groupHoughPointForConfidence(right_hough_lineG_point);
            line_group_data_r[FCount].groupHoughPoint_2_ForConfidence(AvgHough);
            line_group_data_r[FCount].avgGroupScoreForConfidence(rightGroupScoreSum, (int)line_group_data_r.size());
            
            
        }
        sort(confidence_groupLineDistanceOfVP_tmpArray_R.begin(),confidence_groupLineDistanceOfVP_tmpArray_R.end());
        
        CenterDOVPValue = 0;
        if (confidence_groupLineDistanceOfVP_tmpArray_R.size()%2 == 0)
        {
            int tmpv = confidence_groupLineDistanceOfVP_tmpArray_R.size() / 2;
            CenterDOVPValue = (confidence_groupLineDistanceOfVP_tmpArray_R[tmpv] + confidence_groupLineDistanceOfVP_tmpArray_R[tmpv-1])/2;
        }
        else
        {
            int tmpv = confidence_groupLineDistanceOfVP_tmpArray_R.size() / 2;
            CenterDOVPValue = confidence_groupLineDistanceOfVP_tmpArray_R[tmpv];
        }
        
        for (int FCount = 0; FCount < line_group_data_r.size(); FCount++)
        {
            //計算右邊群與消失點距離之信心指數
            line_group_data_r[FCount].groupLineDistanceOfVPForConfidence(CenterDOVPValue);
            
            //計算右邊群的總信心指數
            line_group_data_r[FCount].countConfidence();
        }
        //釋放暫存用vector
        confidence_groupLineDistanceOfVP_tmpArray_L.clear();
        confidence_groupLineDistanceOfVP_tmpArray_R.clear();
/*--------------------------------排序&找出最佳群點----------------------------------*/
        /*---L---*/

//        sort(line_group_data_l.begin(),line_group_data_l.end(),sort_for_line_group_class_GLDOVP_MinF);
//        double tmpNumber = round(line_group_data_l.size()/2.0);
//        
//        if (tmpNumber < 4 && line_group_data_l.size() >= 4)
//        {
//            tmpNumber = 4;
//        }
//        else if (tmpNumber < 4)
//        {
//            tmpNumber = line_group_data_l.size();
//        }
//        if (line_group_data_l.size() > tmpNumber)
//        {
//            for(; line_group_data_l.size() > tmpNumber;)
//            {
//                line_group_data_l.pop_back();
//            }
//        }
//        
//        for (vector<line_group>::iterator c = line_group_data_l.begin(); c != line_group_data_l.end(); )
//        {
//            if ((*c).groupLineDistanceOfVP > 10)
//            {
//                c = line_group_data_l.erase(c);
//            }
//            else
//            {
//                c++;
//            }
//        }
        
/*群的分數排序*/
//#define a
        //以群分數大小排序
        sort(line_group_data_l.begin(),line_group_data_l.end(),sort_for_line_group_class_GroupScore);
//        sort(line_group_data_l.begin(),line_group_data_l.end(),sort_for_line_group_class_Confidence);
#ifdef a
        maxLeft = Calculation_extension(vpfnpoint, line_group_data_l[0].group_line_slope, Ori_pic->height);
#else
        maxLeft_down = Calculation_extension(line_group_data_l[0].GroupPoint_up, line_group_data_l[0].group_slope, Ori_pic->height);
        maxLeft_up = Calculation_extension(line_group_data_l[0].GroupPoint_up, line_group_data_l[0].group_slope, vpfnpoint.y+20);
#endif
        for (int i = 0; i<line_group_data_l.size();i++)
        {
            if (i == 0)
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_l[i].GroupScore;
                (*gplogs).drawGroupPoint_Red(line_group_data_l[i].GroupPoint);
                (*gplogs).putText(0, lg_score.str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y), CV_RGB(0,0,255));
                //補齊整條Hough線段資訊（上點+線)
                (*gplogs).putLineToImg(4, line_group_data_l[i].GroupPoint, line_group_data_l[i].GroupPoint_up, CV_RGB(255, 0, 0));
                (*gplogs).putOtherPointColor(4, line_group_data_l[i].GroupPoint_up, CV_RGB(255, 0, 0));
            }
            else
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_l[i].GroupScore;
                (*gplogs).drawGroupPoint_Yellow(line_group_data_l[i].GroupPoint);
                (*gplogs).putText(0, lg_score.str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y), CV_RGB(0,0,255));
                //補齊整條Hough線段資訊（上點+線)
                (*gplogs).putLineToImg(4, line_group_data_l[i].GroupPoint, line_group_data_l[i].GroupPoint_up, CV_RGB(0, 255, 0));
                (*gplogs).putOtherPointColor(4, line_group_data_l[i].GroupPoint_up, CV_RGB(255, 255, 0));
            }
            
            /*===============HoughLogs==============*/
            stringstream tmpSS;
            
            tmpSS << line_group_data_l[i].confidence_groupHoughPoint;
            (*gplogs).putText(1, tmpSS.str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y), CV_RGB(0,0,255));
            
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_l[i].confidence_groupHoughPoint_2;
            (*gplogs).putText(2, tmpSS.str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y), CV_RGB(0,0,255));
            
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_l[i].confidence_avgGroupScore;
            (*gplogs).putText(3, tmpSS.str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y), CV_RGB(0,0,255));
            /*=====================================*/
            
            
            /*==============GroupLine Distance Of VP Logs==============*/
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_l[i].confidence_groupLineDistanceOfVP_sigmoid;
            //(*gplogs).putOtherPointColor(4, line_group_data_l[i].groupLineDistanceOfVPPoint, CV_RGB(0, 255, 0));
            (*gplogs).putText(4, tmpSS.str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y), CV_RGB(0,0,255));
            /*=========================================================*/
            
            /*==============信心指數 Logs==============*/
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_l[i].confidence;
            (*gplogs).putText(5, tmpSS.str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y), CV_RGB(0,0,255));
            /*=========================================================*/
            
            //畫出線群的線段
#ifdef drawGroupLine
            cvCircle(showLineGroup_line, line_group_data_l[i].GroupPoint,4,CV_RGB(255,255,0), 3);
            cvCircle(showLineGroup_line, line_group_data_l[i].GroupPoint_up,4,CV_RGB(255,255,0), 3);
            cvLine(showLineGroup_line, line_group_data_l[i].GroupPoint, line_group_data_l[i].GroupPoint_up, CV_RGB(255, 0, 0), 2);
#endif
            
        }
        /*---R---*/

//        sort(line_group_data_r.begin(),line_group_data_r.end(),sort_for_line_group_class_GLDOVP_MinF);
//        tmpNumber = round(line_group_data_r.size()/2.0);
//        
//        if (tmpNumber < 4 && line_group_data_r.size() >= 4)
//        {
//            tmpNumber = 4;
//        }
//        else if (tmpNumber < 4)
//        {
//            tmpNumber = line_group_data_r.size();
//        }
//        if (line_group_data_r.size() > tmpNumber)
//        {
//            for(; line_group_data_r.size() > tmpNumber;)
//            {
//                line_group_data_r.pop_back();
//            }
//        }
//        
//        for (vector<line_group>::iterator c = line_group_data_r.begin(); c != line_group_data_r.end(); )
//        {
//            if ((*c).groupLineDistanceOfVP > 10)
//            {
//                c = line_group_data_r.erase(c);
//            }
//            else
//            {
//                c++;
//            }
//        }
        
/*群的分數排序*/
        //以群分數大小排序
        sort(line_group_data_r.begin(), line_group_data_r.end(), sort_for_line_group_class_GroupScore);
//        sort(line_group_data_r.begin(), line_group_data_r.end(), sort_for_line_group_class_Confidence);
        
#ifdef a
        maxRight = Calculation_extension(vpfnpoint, line_group_data_r[0].group_line_slope, Ori_pic->height);
#else
        maxRight_down = Calculation_extension(line_group_data_r[0].GroupPoint_up, line_group_data_r[0].group_slope, Ori_pic->height);
        maxRight_up = Calculation_extension(line_group_data_r[0].GroupPoint_up, line_group_data_r[0].group_slope, vpfnpoint.y+20);
        
#endif
        for (int i = 0; i<line_group_data_r.size();i++)
        {
            if (i == 0)
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_r[i].GroupScore;
                (*gplogs).drawGroupPoint_Red(line_group_data_r[i].GroupPoint);
                (*gplogs).putText(0, lg_score.str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), CV_RGB(0,255,0));
                //補齊整條Hough線段資訊（上點+線)
                (*gplogs).putLineToImg(4, line_group_data_r[i].GroupPoint, line_group_data_r[i].GroupPoint_up, CV_RGB(255, 0, 255));
                (*gplogs).putOtherPointColor(4, line_group_data_r[i].GroupPoint_up, CV_RGB(255, 0, 0));
            }
            else
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_r[i].GroupScore;
                (*gplogs).drawGroupPoint_Yellow(line_group_data_r[i].GroupPoint);
                (*gplogs).putText(0, lg_score.str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), CV_RGB(0,255,0));
                //補齊整條Hough線段資訊（上點+線)
                (*gplogs).putLineToImg(4, line_group_data_r[i].GroupPoint, line_group_data_r[i].GroupPoint_up, CV_RGB(0, 255, 255));
                (*gplogs).putOtherPointColor(4, line_group_data_r[i].GroupPoint_up, CV_RGB(255, 255, 0));
                
            }
            
            /*===============HoughLogs==============*/
            stringstream tmpSS;
            
            tmpSS << line_group_data_r[i].confidence_groupHoughPoint;
            (*gplogs).putText(1, tmpSS.str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), CV_RGB(0,255,0));
            
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_r[i].confidence_groupHoughPoint_2;
            (*gplogs).putText(2, tmpSS.str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), CV_RGB(0,255,0));
            
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_r[i].confidence_avgGroupScore;
            (*gplogs).putText(3, tmpSS.str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), CV_RGB(0,255,0));
            /*=====================================*/
            
            /*==============GroupLine Distance Of VP Logs==============*/
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_r[i].confidence_groupLineDistanceOfVP_sigmoid;
            //(*gplogs).putOtherPointColor(4, line_group_data_r[i].groupLineDistanceOfVPPoint, CV_RGB(0, 0, 255));
            (*gplogs).putText(4, tmpSS.str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), CV_RGB(0,255,0));
            /*=========================================================*/
            
            /*==============信心指數 Logs==============*/
            tmpSS.str("");
            tmpSS.clear();
            tmpSS << line_group_data_r[i].confidence;
            (*gplogs).putText(5, tmpSS.str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), CV_RGB(0,255,0));
            /*=========================================================*/
            
            //畫出線群的線段
#ifdef drawGroupLine
            cvCircle(showLineGroup_line, line_group_data_r[i].GroupPoint,4,CV_RGB(255,255,255), 3);
            cvCircle(showLineGroup_line, line_group_data_r[i].GroupPoint_up,4,CV_RGB(255,255,255), 3);
            cvLine(showLineGroup_line, line_group_data_r[i].GroupPoint, line_group_data_r[i].GroupPoint_up, CV_RGB(255, 0, 0), 2);
#endif
/*--------------------------------------------------------------------------*/
            
        }
    }
    
    /*=======================線段配對==============================*/
    //計算交點
    /*line_crosspoint vPoint(line_group_data_l[0].group_slope,
     calculate_intercept(line_group_data_l[0].GroupPoint,
     line_group_data_l[0].group_slope),
     line_group_data_r[line_group_data_r.size() - 1].group_slope,
     calculate_intercept(line_group_data_r[line_group_data_r.size() - 1].GroupPoint,
     line_group_data_r[line_group_data_r.size() - 1].group_slope));
     cvCircle(Ori_pic, vPoint.LC_point,4,CV_RGB(150,0,255), 10);
     */
    //cvWaitKey();
    
    /*=======================畫出車道線============================*/
    //點
    cvCircle(Ori_pic, vpfnpoint, 4, CV_RGB(255, 0, 255), 2);
    //左
    if (line_group_data_l.size() != 0)
    cvLine(Ori_pic, maxLeft_up, maxLeft_down, CV_RGB(0, 0, 255), 2);
    
    //右
    if (line_group_data_r.size() != 0)
        cvLine(Ori_pic, maxRight_up, maxRight_down, CV_RGB(0, 0, 255), 2);
    
    cout << "刪除高於消失點的資料後的群總數:" << line_group_data_l.size()+line_group_data_r.size() << endl;
    cout << "左群有: " << line_group_data_l.size() << "\t右群有: " << line_group_data_r.size() << endl;
    cout << "左群最大點: " << maxLeft_down.x << "," << maxLeft_down.y << "\t右群最大點: " << maxRight_down.x << "," << maxRight_down.y << endl;

    //cvWaitKey();
    
    //FTBL點記錄(記錄最後三個)
    //L
    FTBLRecoder tmpL;
    tmpL.confidence = line_group_data_l[0].confidence;
    tmpL.Org_1 = line_group_data_l[0].GroupPoint;
    tmpL.Org_2 = line_group_data_l[0].GroupPoint_up;
    tmpL.slope = line_group_data_l[0].group_slope;
    tmpL.up_Point = maxLeft_up;
    tmpL.down_point = maxLeft_down;
    if ((*FTBLRecoder_L).size() < 3)
    {
        
        (*FTBLRecoder_L).push_back(tmpL);
    }
    else
    {
        (*FTBLRecoder_L).push_back(tmpL);
        (*FTBLRecoder_L).pop_front();
    }
    //R
    FTBLRecoder tmpR;
    tmpR.confidence = line_group_data_r[0].confidence;
    tmpR.Org_1 = line_group_data_r[0].GroupPoint;
    tmpR.Org_2 = line_group_data_r[0].GroupPoint_up;
    tmpR.slope = line_group_data_r[0].group_slope;
    tmpR.up_Point = maxRight_up;
    tmpR.down_point = maxRight_down;
    if ((*FTBLRecoder_R).size() < 3)
    {
        (*FTBLRecoder_R).push_back(tmpR);
    }
    else
    {
        (*FTBLRecoder_R).push_back(tmpR);
        (*FTBLRecoder_R).pop_front();
    }
    for (int i = 0; i < (*FTBLRecoder_L).size(); i++)
    {
        cout << "L"<<i+1<<" 信心指數:" << (*FTBLRecoder_L)[i].confidence <<endl;
    }
    for (int i = 0; i < (*FTBLRecoder_R).size(); i++)
    {
        cout << "R"<<i+1<<"信心指數:" << (*FTBLRecoder_R)[i].confidence <<endl;
    }

    //清除線段分群記錄vector
    vector <line_group> Nllgd,Nlrgd;
    line_group_data_l.clear();
    line_group_data_l.swap(Nllgd);
    line_group_data_r.clear();
    line_group_data_r.swap(Nlrgd);
    
//    char outTitle[200];
//    sprintf(outTitle, "/Users/chienfu/Desktop/savepic_M1/DrawAllHoughLine_img_M_%d.jpg",pictureNumber);
//    cvSaveImage(outTitle, (*gplogs).DrawAllHoughLine_img);
    
    (*gplogs).show();
//    int CIsPass =1;
    int CIsPass = CompareTrueLD(maxLeft_up,
                  maxLeft_down,
                  maxRight_up,
                  maxRight_down,
                  "/Users/chienfu/Desktop/高速公路1.db",
                  pictureNumber);
    static int trueFind = 0, FalseFind =0 ,CouldNotDetermineNum = 0;
    bool isCouldNotDetermineNum = false;
    bool doStopPic = false;
    if (CIsPass == 1)
    {
        cout << "(O)車道線找對了!!"<<endl;
        trueFind++;
    }
    else if (CIsPass == 2)
    {
        cout << "(X)車道線找錯了!!" << endl;
        FalseFind++;
        doStopPic = true;
    }
    else if (CIsPass == 3)
    {
        cout << "(N)Could Not Determine" << endl;
        CouldNotDetermineNum++;
        isCouldNotDetermineNum = true;
    }
    cout << "T:" << trueFind <<"\tF:" << FalseFind << "\tN:" << CouldNotDetermineNum <<endl;
    return FTBL(maxLeft_down, maxRight_down, isCouldNotDetermineNum,doStopPic);
}

//比對車道線正確性
int CompareTrueLD (CvPoint FPoint_Lup, CvPoint FPoint_Ldown,CvPoint FPoint_Rup, CvPoint FPoint_Rdown, string DB_URL, int pic_Number)
{
    //查詢資料
    SearchLaneLocation SLL(DB_URL, "LaneLocationRecord");
    SLL.doSearch(pic_Number);
    CvPoint trueLDPoint_Lup, trueLDPoint_Ldown,trueLDPoint_Rup, trueLDPoint_Rdown;
    trueLDPoint_Lup = SLL.Point_LUP();
    trueLDPoint_Ldown = SLL.Point_LDOWN();
    trueLDPoint_Rup = SLL.Point_RUP();
    trueLDPoint_Rdown = SLL.Point_RDOWN();
    bool isCouldNotDetermineNum = SLL.IsCouldNotDetermine();
    
    bool CLDLift = doCompareTrueLD(FPoint_Lup, FPoint_Ldown, trueLDPoint_Lup, trueLDPoint_Ldown);
    bool CLDRight = doCompareTrueLD(FPoint_Rup, FPoint_Rdown, trueLDPoint_Rup, trueLDPoint_Rdown);
    
    if (isCouldNotDetermineNum == true)
    {
        return 3;
    }
    if (CLDLift && CLDRight)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

bool doCompareTrueLD (CvPoint FPoint_up, CvPoint FPoint_down,CvPoint trueLDPoint_up,CvPoint trueLDPoint_down)
{
    line_property F_line_property(FPoint_up,FPoint_down);
    line_property True_line_property(trueLDPoint_up,trueLDPoint_down);
    
    double Fslope = F_line_property.line_slope;
    double Fslope_angle = atan(Fslope)*180/3.1415;
    
    double Trueslope = True_line_property.line_slope;
    double Trueslope_angle = atan(Trueslope)*180/3.1415;
    //R
    double F_Normal = countNormalForCompareTrueLD(&F_line_property);
    
    if (!(abs(Fslope_angle -Trueslope_angle) > 20))
    {
        cout << "xxxx1" << endl;
        if (abs(F_Normal - countNormalForCompareTrueLD(&True_line_property)) <= 60) {
            return true;
        }
    }
    return false;
}
double CountAngleForCompareTrueLD(CvPoint point1, CvPoint point2)
{
    double NewAngle;
    double line_slope = line_property(point1, point2).line_slope;
    NewAngle = atan(line_slope);
    return (M_PI_2 + NewAngle);
}
double countNormalForCompareTrueLD(line_property *_line_property)
{
    //計算(x,0) & (0,Y)
    CvPoint NewLineZeroPoint = cvPoint((_line_property->line_intercept) / _line_property->line_slope,_line_property->line_intercept);
    double Angle = CountAngleForCompareTrueLD(*_line_property->line_point_1,*_line_property->line_point_2);
    double R = NewLineZeroPoint.x*cos(Angle) + 0 * sin(Angle);
    
    return R;
}

