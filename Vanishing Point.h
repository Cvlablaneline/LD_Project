#ifndef VANISHING_POINT
#define VANISHING_POINT
#include <vector>
using namespace std;
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

CvPoint check_point(CvPoint newPoint, vector<CvPoint> *pointSave);

#endif
