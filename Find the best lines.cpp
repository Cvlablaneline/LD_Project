#include "Find the best lines.h"
#include "Vanishing Point.h"
#include <string>
#include <sstream>

#define M_PI 3.14159265358979323846

using namespace std;

#define Line_Group_Range 35
#define Line_Group_lenRange 100

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
    group_slope = 0.0;
    group_slope_angle = 0.0;
    GroupPoint = firstPoint;
    GroupScore = 1;
}

line_group::line_group(CvPoint firstPoint,double firstslope)
{
    angle = 0.0;
    group_line_slope = 0.0;
    group_slope = firstslope;
    group_slope_angle = atan(firstslope)*180/3.1415;
    GroupPoint = firstPoint;
    GroupScore = 1;
}

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
    //記錄點為最低點
    if ((*AllLineData)[LineNum].line_point_1->y > (*AllLineData)[LineNum].line_point_2->y)
    {
        GroupPoint.x += (*AllLineData)[LineNum].line_point_1->x;
        GroupPoint.y += (*AllLineData)[LineNum].line_point_1->y;
        GroupPoint_up.x += (*AllLineData)[LineNum].line_point_2->x;
        GroupPoint_up.y += (*AllLineData)[LineNum].line_point_2->y;
    }
    else
    {
        GroupPoint.x += (*AllLineData)[LineNum].line_point_2->x;
        GroupPoint.y += (*AllLineData)[LineNum].line_point_2->y;
        GroupPoint_up.x += (*AllLineData)[LineNum].line_point_1->x;
        GroupPoint_up.y += (*AllLineData)[LineNum].line_point_1->y;
    }
    GroupScore = 1;
}

void line_group::update(CvPoint newPoint)
{
    GroupPoint.x = (GroupPoint.x + newPoint.x) / 2;
    GroupPoint.y = (GroupPoint.y + newPoint.y) / 2;
    GroupScore++;
}

void line_group::update(CvPoint newPoint,double newslope)
{
    GroupPoint.x = (GroupPoint.x + newPoint.x) / 2;
    GroupPoint.y = (GroupPoint.y + newPoint.y) / 2;
    group_slope = (group_slope + newslope) /2.0;
    group_slope_angle = atan(group_slope)*180/3.1415;
    GroupScore++;
}

void line_group::update(int LineNum)
{
    LineNumTable.push_back(LineNum);
    GroupPoint.x = 0;
    GroupPoint.y = 0;
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
        group_slope += (*AllLineData)[*lnt_first].line_slope;
    }
    
    GroupPoint.x /= LineNumTable.size();
    GroupPoint.y /= LineNumTable.size();
    group_slope /= (float)LineNumTable.size();
    group_slope_angle = atan(group_slope)*180/3.1415;
	//R
	group_Normal_sum += countNormal(AllLineData, LineNum);
	group_Normal = group_Normal_sum / LineNumTable.size();

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
	CvPoint NewLineZeroPoint = cvPoint((-(*line)[LineNum].line_intercept) / (*line)[LineNum].line_slope,
		(*line)[LineNum].line_intercept);
	cout << "\t" << NewLineZeroPoint.x << "," << NewLineZeroPoint.y << endl;
	cout << (*line)[LineNum].line_point_1->x << "," << (*line)[LineNum].line_point_1->y << "\n" << (*line)[LineNum].line_point_2->x << "," << (*line)[LineNum].line_point_2->y << endl;
	double Angle = CountAngleForNormal(*(*line)[LineNum].line_point_1, *(*line)[LineNum].line_point_2);
	//double Angle = CountAngleForNormal(cvPoint(NewLineZeroPoint.x,0), cvPoint(0,NewLineZeroPoint.y));
	cout <<"angle"<< Angle << endl;

	double R = abs(NewLineZeroPoint.x*cos(Angle) + NewLineZeroPoint.y*sin(Angle));
	cout << R << endl;
	cvWaitKey();

	return R;
}

double CountAngleForNormal(CvPoint point1, CvPoint point2)
{
	double NewAngle;
	double line_slope = line_property(point1, point2).line_slope;
	if (line_slope == -1)
	{
		NewAngle = 90;
		return NewAngle;
	}
	NewAngle = atan(line_slope);
	return (M_PI/2 - NewAngle);
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
	if (!(abs(group_slope_angle - (atan((*AllLineData)[LineNum].line_slope) * 180 / 3.1415)) > 6))
	{
		if (abs(group_Normal - countNormal(AllLineData, LineNum)) <= 0.15) {
			return true;
		}
	}
	return false;
}

//計算長度
double line_group::countDistance (CvPoint line1,CvPoint line2)
{
    return sqrt((pow((line2.x-line1.x),2)+pow((line2.y-line1.y),2)));
}

//自訂排序演算法
bool sort_for_line_group_class(const line_group a, const line_group b)
{
    return a.GroupScore < b.GroupScore;
}

bool sort_for_line_group_class_ANGLE(const line_group a, const line_group b)
{
    return a.angle < b.angle;
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
CvPoint Calculation_extension(CvPoint Vanishing_Point,double slope,int y)
{
    return cvPoint(((Vanishing_Point.y - y) - (slope*Vanishing_Point.x)) / -(slope),y);
}
//算截距
double calculate_intercept(CvPoint point_1,double line_slope)
{
    return (double)(point_1.y - line_slope * point_1.x);
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


//找出最佳車道線
FTBL FindTheBestLines(IplImage* Ori_pic,vector<line_property> *AllHLineSlope, CvPoint vpfnpoint, int vp_range,vector<CvPoint> *vpRecoder,IplImage* showLineGroup_line)
{
#define drawGroupLine //是否畫出線群的線段
//#define NoGroup

    cout << "FTBL得到的霍夫轉換線斷數量：" << (*AllHLineSlope).size() << endl;
    
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
    /*顯示水平線Y軸資訊*/
//    cout << (*vpRecoder).size() << endl;
//    
//    for (int i = 0;i<(*vpRecoder).size();i++)
//    {
//        cout << (*vpRecoder)[i].x<<","<<(*vpRecoder)[i].y << endl;
//    }
    
    //計算水平線紀錄最高點(y最小)
    int topVP = WhoIsTop(vpRecoder);
    cout << "水平線y軸使用:" << (*vpRecoder)[topVP].y << endl;
    
    //文字顯示設定
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 0.5f, 0.5f, 0, 1, CV_AA);
    stringstream lg_score;
    
    for (int AHLSNum = 0; AHLSNum < (*AllHLineSlope).size(); AHLSNum++)
    {
		if ((*AllHLineSlope)[AHLSNum].line_slope == 0)
			continue;
        //左邊
        if ((*AllHLineSlope)[AHLSNum].line_slope < 0)
        {
            if (line_group_data_l.size() < 1)
            {
                line_group tmp(AllHLineSlope,AHLSNum);
                line_group_data_l.push_back(tmp);
            }
#ifdef NoGroup
            else if (1)
            {
                line_group tmp(AllHLineSlope,AHLSNum);
                line_group_data_l.push_back(tmp);
            }
#endif
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
        else //右邊
        {
            if (line_group_data_r.size() < 1)
            {
                line_group tmp(AllHLineSlope,AHLSNum);
                line_group_data_r.push_back(tmp);
            }
#ifdef NoGroup
            else if (1)
            {
                line_group tmp(AllHLineSlope,AHLSNum);
                line_group_data_r.push_back(tmp);
            }
#endif
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
        
//#define a
        
        //以角度排序
        sort(line_group_data_l.begin(), line_group_data_l.end(), sort_for_line_group_class_ANGLE);
#ifdef a
        maxLeft = Calculation_extension(vpfnpoint, line_group_data_l[0].group_line_slope, Ori_pic->height);
#else
        maxLeft = Calculation_extension(vpfnpoint, line_group_data_l[0].group_slope, Ori_pic->height);
#endif
        for (int i = 0; i<line_group_data_l.size();i++)
        {
            if (i == 0)
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_l[i].GroupScore;
                cvCircle(Ori_pic, line_group_data_l[i].GroupPoint,4,CV_RGB(255,0,0), 3);
                cvPutText(Ori_pic, lg_score.str().c_str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y) , &font ,CV_RGB(0,0,255));
            }
            else
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_l[i].GroupScore;
                cvCircle(Ori_pic, line_group_data_l[i].GroupPoint,4,CV_RGB(255,255,0), 3);
                cvPutText(Ori_pic, lg_score.str().c_str(), cvPoint(line_group_data_l[i].GroupPoint.x-2, line_group_data_l[i].GroupPoint.y) , &font ,CV_RGB(0,0,255));
            }
            //畫出線群的線段
#ifdef drawGroupLine
            cvCircle(showLineGroup_line, line_group_data_l[i].GroupPoint,4,CV_RGB(255,255,0), 3);
            cvCircle(showLineGroup_line, line_group_data_l[i].GroupPoint_up,4,CV_RGB(255,255,0), 3);
            cvLine(showLineGroup_line, line_group_data_l[i].GroupPoint, line_group_data_l[i].GroupPoint_up, CV_RGB(255, 0, 0), 2);
#endif
            
        }
        
        sort(line_group_data_r.begin(), line_group_data_r.end(), sort_for_line_group_class_ANGLE);
#ifdef a
        maxRight = Calculation_extension(vpfnpoint, line_group_data_r[line_group_data_r.size() - 1].group_line_slope, Ori_pic->height);
#else
        maxRight = Calculation_extension(vpfnpoint, line_group_data_r[line_group_data_r.size() - 1].group_slope, Ori_pic->height);
        
#endif
        for (int i = 0; i<line_group_data_r.size();i++)
        {
            if (i == line_group_data_r.size()-1)
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_r[i].GroupScore;
                cvCircle(Ori_pic, line_group_data_r[i].GroupPoint,4,CV_RGB(255,0,0), 3);
                cvPutText(Ori_pic, lg_score.str().c_str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), &font ,CV_RGB(0,255,0));
            }
            else
            {
                lg_score.str("");
                lg_score.clear();
                lg_score << line_group_data_r[i].GroupScore;
                cvCircle(Ori_pic, line_group_data_r[i].GroupPoint,4,CV_RGB(255,255,0), 3);
                cvPutText(Ori_pic, lg_score.str().c_str(), cvPoint(line_group_data_r[i].GroupPoint.x-2, line_group_data_r[i].GroupPoint.y), &font ,CV_RGB(0,255,0));
                
            }
            //畫出線群的線段
#ifdef drawGroupLine
            cvCircle(showLineGroup_line, line_group_data_r[i].GroupPoint,4,CV_RGB(255,255,255), 3);
            cvCircle(showLineGroup_line, line_group_data_r[i].GroupPoint_up,4,CV_RGB(255,255,255), 3);
            cvLine(showLineGroup_line, line_group_data_r[i].GroupPoint, line_group_data_r[i].GroupPoint_up, CV_RGB(255, 0, 0), 2);
#endif
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
    cvLine(Ori_pic, vpfnpoint, maxLeft, CV_RGB(0, 0, 255), 2);
    
    //右
    cvLine(Ori_pic, vpfnpoint, maxRight, CV_RGB(0, 0, 255), 2);
    
    cout << "刪除高於消失點的資料後的群總數:" << line_group_data_l.size()+line_group_data_r.size() << endl;
    cout << "左群有: " << line_group_data_l.size() << "\t右群有: " << line_group_data_r.size() << endl;
    cout << "左群最大點: " << maxLeft.x << "," << maxLeft.y << "\t右群最大點: " << maxRight.x << "," << maxRight.y << endl;

    //清除車道線斜率記錄vector
    /*vector <line_property> Nls, Nrs;
     newleftSlope.clear();
     newrightSlope.clear();
     newleftSlope.swap(Nls);
     newrightSlope.swap(Nrs);*/
    
    //清除線段分群記錄vector
    vector <line_group> Nllgd,Nlrgd;
    line_group_data_l.clear();
    line_group_data_l.swap(Nllgd);
    line_group_data_r.clear();
    line_group_data_r.swap(Nlrgd);
    
    return FTBL(maxLeft ,maxRight);
}