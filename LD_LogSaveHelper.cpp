//
//  LD_LogSaveHelper.cpp
//  LD_Project_xcode
//
//  Created by Chienfu Huang on 2015/9/7.
//  Copyright (c) 2015年 ___HCF___. All rights reserved.
//

#include "LD_LogSaveHelper.h"

//-----------------save_text_for_group_hough_point-----------------
save_text_for_group_hough_point::save_text_for_group_hough_point()
{
    cout << "error, no data" <<endl;
    exit(1);
}
save_text_for_group_hough_point::save_text_for_group_hough_point(int picNum)
{
    if(pNum == -1)
    {
        if (picNum >= 0)
            pNum = picNum;
        else
            cout << "error picNum" <<endl;
    }
    else
    {
        cout << "error" <<endl;
    }
}
save_text_for_group_hough_point::save_text_for_group_hough_point(int picNum,string Wrinkle)
{
    if(pNum == -1 && wrinkle.size() == 0)
    {
        if (picNum >= 0)
            pNum = picNum;
        else
            cout << "error picNum" <<endl;
        if (Wrinkle != "")
        {
            wrinkle = Wrinkle;
        }
        else
        {
            cout << "error Wrinkle"<<endl;
        }
    }
    else
    {
        cout << "error!!" <<endl;
    }

}

bool save_text_for_group_hough_point::newData (string DataInput)
{
    SData.push_back(DataInput);
    return true;
}
bool save_text_for_group_hough_point::WriteData(string Path)
{
    if (pNum < 0)
    {
        cout << "error"<<endl;
        return false;
    }
    fstream file;
    stringstream URL;
    URL << Path;
    if (wrinkle.size() != 0 || wrinkle != "")
    {
        URL << wrinkle;
    }
    URL << pNum;
    URL << ".txt";
    cout << "save log :" << URL.str()<<endl;
    file.open(URL.str().c_str(), ios::out);
    if (!file)
    {
        cout << "file create error"<<endl;
        return false;
    }
    file << "picture : "<<pNum<<endl;
    file << "-----------------------"<<endl;
    for (int i = 0; i < SData.size();i++)
    {
        file << SData[i]<<endl;
    }
    file.close();
    cout << "save log finish" << endl;
    return true;
}
//-----------------------------------------------------------------
//-------------------------GroupPointLogs--------------------------
GroupPointLogs::GroupPointLogs(IplImage* Ori_pic)
{
    Origin_img = Ori_pic;
    HoughLog_img1 = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    HoughLog_img2 = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    HoughLog_img3 = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    GLDistanceOfVP_img =cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    DrawAllHoughLine_img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    confidence_img= cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    
    cvResize(Ori_pic, HoughLog_img1, CV_INTER_LINEAR);
    cvResize(Ori_pic, HoughLog_img2, CV_INTER_LINEAR);
    cvResize(Ori_pic, HoughLog_img3, CV_INTER_LINEAR);
    cvResize(Ori_pic, GLDistanceOfVP_img, CV_INTER_LINEAR);
    cvResize(Ori_pic, DrawAllHoughLine_img,CV_INTER_LINEAR);
    cvResize(Ori_pic, confidence_img,CV_INTER_LINEAR);
    
}
bool GroupPointLogs::drawGroupPoint_Red(CvPoint point)
{
    cvCircle(Origin_img, point,4,CV_RGB(255,0,0), 3);
    cvCircle(HoughLog_img1, point,4,CV_RGB(255,0,0), 3);
    cvCircle(HoughLog_img2, point,4,CV_RGB(255,0,0), 3);
    cvCircle(HoughLog_img3, point,4,CV_RGB(255,0,0), 3);
    cvCircle(GLDistanceOfVP_img, point,4,CV_RGB(255,0,0), 3);
    cvCircle(confidence_img, point,4,CV_RGB(255,0,0), 3);
    return true;
}
bool GroupPointLogs::drawGroupPoint_Yellow(CvPoint point)
{
    cvCircle(Origin_img, point,4,CV_RGB(255,255,0), 3);
    cvCircle(HoughLog_img1, point,4,CV_RGB(255,255,0), 3);
    cvCircle(HoughLog_img2, point,4,CV_RGB(255,255,0), 3);
    cvCircle(HoughLog_img3, point,4,CV_RGB(255,255,0), 3);
    cvCircle(GLDistanceOfVP_img, point,4,CV_RGB(255,255,0), 3);
    cvCircle(confidence_img, point,4,CV_RGB(255,255,0), 3);
    return true;
}
bool GroupPointLogs::putText (int ImgSelect,string str,CvPoint point,CvScalar color)
{
    //font
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 0.5f, 0.5f, 0, 1, CV_AA);
    
    if (ImgSelect == 0)
    {
        cvPutText(Origin_img, str.c_str(), point, &font, color);
    }
    else if (ImgSelect == 1)
    {
        cvPutText(HoughLog_img1, str.c_str(), point, &font, color);
    }
    else if (ImgSelect == 2)
    {
        cvPutText(HoughLog_img2, str.c_str(), point, &font, color);
    }
    else if (ImgSelect == 3)
    {
        cvPutText(HoughLog_img3, str.c_str(), point, &font, color);
    }
    else if (ImgSelect == 4)
    {
        cvPutText(GLDistanceOfVP_img, str.c_str(), point, &font, color);
    }
    else if (ImgSelect == 5)
    {
        cvPutText(confidence_img, str.c_str(), point, &font, color);
    }
    else
    {
        return false;
    }
    return true;
}

bool GroupPointLogs::putOtherPointColor (int ImgSelect,CvPoint point,CvScalar color)
{
    if (ImgSelect == 0)
    {
        cvCircle(Origin_img, point, 4, color, 3);
    }
    else if (ImgSelect == 1)
    {
        cvCircle(HoughLog_img1, point, 4, color, 3);
    }
    else if (ImgSelect == 2)
    {
        cvCircle(HoughLog_img2, point, 4, color, 3);
    }
    else if (ImgSelect == 3)
    {
        cvCircle(HoughLog_img3, point, 4, color, 3);
    }
    else if (ImgSelect == 4)
    {
        cvCircle(GLDistanceOfVP_img, point, 4, color, 3);
    }
    else if (ImgSelect == 5)
    {
        cvCircle(confidence_img, point, 4, color, 3);
    }
    else
    {
        return false;
    }
    return true;
}
bool GroupPointLogs::putLineToImg (int ImgSelect,CvPoint point1,CvPoint point2,CvScalar color)
{
    if (ImgSelect == 0)
    {
        cvLine(Origin_img, point1, point2, color, 2);
    }
    else if (ImgSelect == 1)
    {
        cvLine(HoughLog_img1, point1, point2, color, 2);
    }
    else if (ImgSelect == 2)
    {
        cvLine(HoughLog_img2, point1, point2, color, 2);
    }
    else if (ImgSelect == 3)
    {
        cvLine(HoughLog_img3, point1, point2, color, 2);
    }
    else if (ImgSelect == 4)
    {
        cvLine(GLDistanceOfVP_img, point1, point2, color, 2);
    }
    else if (ImgSelect == 5)
    {
        cvLine(confidence_img, point1, point2, color, 2);
    }
    else
    {
        return false;
    }
    return true;
}
bool GroupPointLogs::drawAllHoughLines(vector<line_property> *AllHLineSlope)
{
    for (int i = 0; i < AllHLineSlope->size(); i++)
    {
        cvLine(DrawAllHoughLine_img, *(*AllHLineSlope)[i].line_point_1, *(*AllHLineSlope)[i].line_point_2, CV_RGB(255, 30, 120), 2);
    }
    return true;
}
bool GroupPointLogs::show()
{
    cvShowImage("GroupPoingLogs_hough1", HoughLog_img1);
    cvShowImage("GroupPoingLogs_hough2", HoughLog_img2);
    cvShowImage("confidence_avgGroupScore", HoughLog_img3);
    cvShowImage("GroupLine Distance Of VP", GLDistanceOfVP_img);
    cvShowImage("Draw All Hoguh Lines", DrawAllHoughLine_img);
    cvShowImage("Confidence Img", confidence_img);
    return true;
}
GroupPointLogs::~GroupPointLogs()
{
    cvReleaseImage(&HoughLog_img1);
    cvReleaseImage(&HoughLog_img2);
    cvReleaseImage(&HoughLog_img3);
    cvReleaseImage(&GLDistanceOfVP_img);
    cvReleaseImage(&DrawAllHoughLine_img);
    cvReleaseImage(&confidence_img);
    cout << "GroupPointLogs Images Was Released"<<endl;
}