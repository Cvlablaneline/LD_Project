//
//  Experimental_tools.hpp
//  LD_Project_xcode
//
//  Created by Chien-Fu Huang on 2016/1/22.
//  Copyright © 2016年 ___HCF___. All rights reserved.
//

#ifndef Experimental_tools_hpp
#define Experimental_tools_hpp

#include <iostream>
#include <highgui.h>
#include <sstream>
#include <sqlite3.h>
using namespace std;



//車道線定位記錄器
class LaneLocationRecord {
private:
    sqlite3 *db;
    bool classInit;
    IplImage *pic,*pic_bk;
    int clickNum;
    CvPoint point_LUP,point_LDOWN,point_RUP,point_RDOWN;
    string fileName,pathURL,DB_URL;
    int fileNum;
    int isCouldNotDetermine;
    //OpenCV MouseCallback
    static void CallBackFuncForOpenCVMouseEvent(int event, int x, int y, int flags, void* userdata);
    //sqlite3CallBack
    static int CallBackFuncForSqlite3LLR(void *userdata, int argc, char **argv, char **szColName);
    //重置func
    bool restart();
    bool pointReset();
    //font
    CvFont font;
public:
    LaneLocationRecord();
    //建構元(檔案(資料庫)名稱, 資料庫要放置的目錄)
    LaneLocationRecord(string _fileName, string _pathURL);
    ~LaneLocationRecord();
    //開始定位車道線(原圖,圖片編號)
    bool start(IplImage* Ori_pic,int _fileNum);
};

//查詢車道線定位紀錄
class SearchLaneLocation
{
private:
    sqlite3 *db;
    bool classInit;
    int mode;
    CvPoint point_LUP,point_LDOWN,point_RUP,point_RDOWN;
    string tableName;
    string fileName;
    int fileNum;
    static int CallBackFuncForSqlite3SearchLL(void *userdata, int argc, char **argv, char **szColName);
    bool pointReset();
    int isCouldNotDetermine;
public:
    SearchLaneLocation();
    SearchLaneLocation(string _DB_URL, string _tableName);
    SearchLaneLocation(sqlite3 *_db, string _tableName);
    ~SearchLaneLocation();
    bool doSearch(int picNum);
    CvPoint Point_LUP();
    CvPoint Point_LDOWN();
    CvPoint Point_RUP();
    CvPoint Point_RDOWN();
    bool IsCouldNotDetermine();
};

#endif /* Experimental_tools_hpp */

