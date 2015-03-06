#ifndef VANISHING_POINT
#define VANISHING_POINT
#include <vector>
using namespace std;

//DEGUB開關
//顯示輸入灰階圖。
#define LOOK_CVCANNY

//顯示所有原始線(咖啡)。
#define DRAW_ALL_BEFORE_LINE

//顯示所有算交點線(藍色)。
#define DRAW_ALL_AFTER_LINE

//一條條顯示交點線(藍色)與斜率截距值。
//#define DRAW_LINE_DATA

//顯示交點資訊(紅叉)與座標。
//#define DRAW_POINT_DATA
///////////////////////////////////////////////////


#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define CPIndex 5

class line_property {
public:
    CvPoint *line_point_1, *line_point_2;
    double line_slope;
    double line_intercept;
public:
    line_property();
    line_property(CvPoint& point_1, CvPoint& point_2);
    ~line_property() {};
};

class line_crosspoint
{
public:
    CvPoint LC_point;
    int LC_score;
public:
    line_crosspoint();
    line_crosspoint(double slope1, double intercept1, double slope2, double intercept2);
    ~line_crosspoint(){};
};
CvPoint find_Vanishing_Point(IplImage* Canny, IplImage* Ori_pic, vector<line_property>* all_Slope,CvMemStorage* storage_DThrSmo);

void Check_VPoint(int &VPx,int &VPy);

#endif
