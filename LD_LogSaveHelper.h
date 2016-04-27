//
//  LD_LogSaveHelper.h
//  LD_Project_xcode
//
//  Created by Chienfu Huang on 2015/9/7.
//  Copyright (c) 2015年 ___HCF___. All rights reserved.
//

#ifndef __LD_Project_xcode__LD_LogSaveHelper__
#define __LD_Project_xcode__LD_LogSaveHelper__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cv.h>
#include <highgui.h>
#include "Vanishing Point.h"
using namespace std;

class save_text_for_group_hough_point {
private:
    int pNum = -1;
    string wrinkle;
    vector<string> SData;
public:
    save_text_for_group_hough_point();
    save_text_for_group_hough_point(int picNum);
    save_text_for_group_hough_point(int picNum,string Wrinkle);
    bool newData (string DataInput);
    bool WriteData(string Path);
};

class GroupPointLogs {
public:
    IplImage *Origin_img;
    IplImage *HoughLog_img1;
    IplImage *HoughLog_img2;
    IplImage *HoughLog_img3;
    IplImage *GLDistanceOfVP_img;
    
    IplImage *DrawAllHoughLine_img;
    
    IplImage *confidence_img;
    
    GroupPointLogs(){};
    GroupPointLogs(IplImage* Ori_pic);
    bool drawGroupPoint_Red(CvPoint point);
    bool drawGroupPoint_Yellow(CvPoint point);
    bool putText (int ImgSelect,string str,CvPoint point,CvScalar color);
    bool putOtherPointColor (int ImgSelect,CvPoint point,CvScalar color);
    bool putLineToImg (int ImgSelect,CvPoint point1,CvPoint point2,CvScalar color);
    bool show();
    bool drawAllHoughLines(vector<line_property> *AllHLineSlope);
    ~GroupPointLogs();
};

class CompareCTools {
public:
    double Hough1_Max,Hough2_Max,SFHough2_Max;
    int Score_Max;
    
    vector<int> Hough1_Group_Num,
                Hough2_Group_Num,
                SFHough2_Group_Num,
                Score_Group_Num;
    
    CompareCTools()
    {
        Hough1_Max = Hough2_Max = SFHough2_Max = 0.0;
        Score_Max = 0;
        Hough1_Group_Num.push_back(0);
        Hough2_Group_Num.push_back(0);
        SFHough2_Group_Num.push_back(0);
        Score_Group_Num.push_back(0);
        cout <<"比較一致性工具初始化完成!!"<<endl;
    }
    
    int compare_Hough1 (double Value, int ArrayNum)
    {
        if (Hough1_Max < Value)
        {
            Hough1_Max = Value;
            if (Hough1_Group_Num.size() == 1)
            {
                Hough1_Group_Num[0] = ArrayNum;
            }
            else
            {
                Hough1_Group_Num.clear();
                Hough1_Group_Num.push_back(ArrayNum);
            }
            return 1;
        }
        if (Hough1_Max > Value)
        {
            return 2;
        }
        if(Hough1_Max == Value)
        {
            Hough1_Group_Num.push_back(ArrayNum);
            return 3;
        }
        return 0;
    }
};
#endif /* defined(__LD_Project_xcode__LD_LogSaveHelper__) */
