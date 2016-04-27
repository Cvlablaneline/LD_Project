//
//  Experimental_tools.cpp
//  LD_Project_xcode
//
//  Created by Chien-Fu Huang on 2016/1/22.
//  Copyright © 2016年 ___HCF___. All rights reserved.
//

#include "Experimental_tools.hpp"

LaneLocationRecord::LaneLocationRecord()
{
    cout << "沒有任何輸入，初始化失敗"<< endl;
    classInit = false;
    exit(-1);
}

LaneLocationRecord::LaneLocationRecord(string _fileName, string _pathURL)
{
    fileName = _fileName;
    pathURL = _pathURL;
    if (pathURL[pathURL.length()-1] != '/' && pathURL[pathURL.length()-1] != '\\')
    {
        for ( string::iterator it=pathURL.begin(); it!=pathURL.end(); ++it)
        {
            if (*it == '\\')
            {
                pathURL.replace(it, it+1, "/");
            }
        }
        pathURL.append("/");
    }
    cout << "資料庫要存放的路徑為 " << pathURL << "之下"<< endl;
    cout << "影片名稱為: " << fileName << endl;
    DB_URL = pathURL + fileName +".db";
    cout << "資料庫路徑與名稱為:"<<DB_URL << endl;
    
    //Open sqlite3 DB
    int db_open_error = sqlite3_open(DB_URL.c_str(), &db);
    if(db_open_error)
    {
        cout << "資料庫創建或開啟失敗！！"<<endl;
        exit(-1);
    }
    else
    {
        cout << "資料庫創建或開啟完成！！"<<endl;
    }
    
    //create table
    const string sqlCreateTable = "CREATE TABLE LaneLocationRecord("  \
                                "fileNum INT PRIMARY KEY     NOT NULL," \
                                "fileName           TEXT    NOT NULL," \
                                "point_LUP_X        INT     NOT NULL," \
                                "point_LUP_Y        INT     NOT NULL," \
                                "point_LDOWN_X      INT     NOT NULL," \
                                "point_LDOWN_Y      INT     NOT NULL," \
                                "point_RUP_X        INT     NOT NULL," \
                                "point_RUP_Y        INT     NOT NULL," \
                                "point_RDOWN_X      INT     NOT NULL," \
                                "point_RDOWN_Y      INT     NOT NULL," \
                                "CouldNotDetermine  INT     NOT NULL);";
    
    bool reTry = false;
    do {
        char *szErrMsg = 0;
        int rc = sqlite3_exec(db, sqlCreateTable.c_str(), CallBackFuncForSqlite3LLR, 0, &szErrMsg);
        
        if( rc != SQLITE_OK )
        {
            bool isContinue = false;
            string rcErrorMsg = szErrMsg;
            sqlite3_free(szErrMsg);
            cout << "資料表建立錯誤 SQL error: " << rcErrorMsg << endl;
            if (rcErrorMsg == "table LaneLocationRecord already exists")
            {
                cout << "資料表已經存在，輸入 Y-確定繼續使用 R-刪除之前的資料表 N-結束\n請輸入(Y/R/N): ";
                string inputRQ = "";
                cin >> inputRQ;
                if (inputRQ[0] == 'Y' || inputRQ[0] == 'y')
                {
                    cout << "繼續使用先前創立的資料表!!" << endl;
                    isContinue = true;
                    reTry = false;
                }
                else if (inputRQ[0] == 'R' || inputRQ[0] == 'r')
                {
                    isContinue = true;
                    reTry = true;
                    sqlite3_exec(db, "DROP TABLE LaneLocationRecord", CallBackFuncForSqlite3LLR, 0, &szErrMsg);
                    cout << "刪除之前的資料表並重新建立!!" << endl;
                }
                else
                {
                    isContinue = false;
                }
            }
            if (!isContinue)
            {
                reTry = false;
                exit(-1);
            }
        }else
        {
            reTry = false;
            cout << "資料表新建立完成" << endl;
        }
    } while (reTry);
    clickNum = 0;
    this->pointReset();
    fileNum = -1;
    classInit = true;
    //font
    cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 0.5f, 0.5f, 0, 1, CV_AA);
    cout << "初始化完成，可以開始使用車道線位置記錄器了!!" <<endl;
}

LaneLocationRecord::~LaneLocationRecord()
{
    if (classInit)
    {
        sqlite3_close(db);
    }
    cout << "車道線位置記錄器關閉！！" << endl;
}

bool LaneLocationRecord::start(IplImage* Ori_pic,int _fileNum)
{
    if (!classInit)
    {
        cout << "類別初始化異常" << endl;
        return -1;
    }
    pic = cvCloneImage(Ori_pic);
    pic_bk = cvCloneImage(Ori_pic);
    fileNum = _fileNum;
    cout << "影像編號----- " << fileNum << endl;
    
    cvShowImage("LaneLocationRecord", pic);
    cv::setMouseCallback("LaneLocationRecord", CallBackFuncForOpenCVMouseEvent, this);
    
    this->clickNum = 0;
    this->pointReset();
    
    while (this->clickNum < 4) {
        cout << "點擊左鍵記錄車道線左上座標!!(按q鍵可以標示此圖無法判斷車道線)" << endl;
        char waitKeyInput = cvWaitKey();
        if (waitKeyInput == 'q')
        {
            cout << "您按了q鍵，是否將此張圖標示無法判斷車道線？(Y:是 N:否)" << endl;
            string check = "";
            cin >> check;
            if (check[0] == 'Y' || check[0] == 'y')
            {
                isCouldNotDetermine = 1;
                this->clickNum = 10;
            }
        }
        if (this->clickNum < 4)
        {
            cout << "您沒有點選車道線位置，或是點選總數量小於4個點!!" << endl;
            restart();
            cout << "請重新點選!!" << endl;
        }
    }
    
    stringstream sqlstr;
    
    sqlstr << "INSERT INTO LaneLocationRecord (fileNum, fileName, point_LUP_X, point_LUP_Y, point_LDOWN_X,point_LDOWN_Y, point_RUP_X, point_RUP_Y, point_RDOWN_X, point_RDOWN_Y, CouldNotDetermine) "
    << "VALUES ("
    << fileNum << ",'"
    << fileName << "',"
    << point_LUP.x << ","
    << point_LUP.y << ","
    << point_LDOWN.x << ","
    << point_LDOWN.y << ","
    << point_RUP.x << ","
    << point_RUP.y << ","
    << point_RDOWN.x << ","
    << point_RDOWN.y << ","
    << isCouldNotDetermine << ");";
    
    bool reTry = false;
    do {
        char *szErrMsg = 0;
        int rc = sqlite3_exec(db, sqlstr.str().c_str(), CallBackFuncForSqlite3LLR, 0, &szErrMsg);
        if( rc != SQLITE_OK ){
            bool isContinue = false;
            string rcErrorMsg = szErrMsg;
            sqlite3_free(szErrMsg);
            cout << "資料寫入資料庫錯誤 SQL error: " << rcErrorMsg << endl;
            if (rcErrorMsg == "UNIQUE constraint failed: LaneLocationRecord.fileNum" || rc == 19)
            {
                cout << "已經有此圖片編號的紀錄"<< endl;
                cout << "輸入 Y-略過不寫入 R-覆蓋原資料 N-結束\n請輸入(Y/R/N): ";
                string inputRQ = "";
                cin >> inputRQ;
                if (inputRQ[0] == 'Y' || inputRQ[0] == 'y')
                {
                    cout << "繼續使用先前創立的資料表!!" << endl;
                    isContinue = true;
                    reTry = false;
                }
                else if (inputRQ[0] == 'R' || inputRQ[0] == 'r')
                {
                    isContinue = true;
                    reTry = true;
                    stringstream sqlQueryTmp;
                    sqlQueryTmp << "DELETE from LaneLocationRecord where fileNum=" << fileNum << ";";
                    sqlite3_exec(db, sqlQueryTmp.str().c_str(), CallBackFuncForSqlite3LLR, 0, &szErrMsg);
                    cout << "刪除之前的資料表並重新建立!!" << endl;
                }
                else
                {
                    isContinue = false;
                }

            }
            if (!isContinue)
            {
                reTry = false;
                exit(-1);
            }
        }else{
            reTry = false;
            cout << "資料座標寫入資料庫完成!!" << endl;
        }
    } while (reTry);
    
    cvReleaseImage(&pic);
    cvReleaseImage(&pic_bk);
    return true;
}

bool LaneLocationRecord::restart()
{
    clickNum = 0;
    this->pointReset();
    pic = cvCloneImage(pic_bk);
    cvShowImage("LaneLocationRecord", pic);
    return true;
}

bool LaneLocationRecord::pointReset()
{
    point_LUP = cvPoint(-1, -1);
    point_LDOWN = cvPoint(-1, -1);
    point_RUP = cvPoint(-1, -1);
    point_RDOWN = cvPoint(-1, -1);
    isCouldNotDetermine = 0;
    return true;
}

void LaneLocationRecord::CallBackFuncForOpenCVMouseEvent(int event, int x, int y, int flags, void* userdata)
{
    LaneLocationRecord *LLR = (LaneLocationRecord *) userdata;
    //左鍵
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
        cout << "點擊左鍵位置..(" << x << ", " << y << ")" << endl;
        if ((*LLR).clickNum == 0)
        {
            (*LLR).point_LUP = cvPoint(x, y);
            cvCircle((*LLR).pic, cvPoint(x, y),2,CV_RGB(255,0,0), 3);
            cvPutText((*LLR).pic, "Top left", cvPoint(x+5, y), &(*LLR).font, CV_RGB(0,255,0));
            cout << "點擊左鍵記錄車道線左下座標!!" << endl;
            (*LLR).clickNum++;
        }
        else if ((*LLR).clickNum == 1)
        {
            (*LLR).point_LDOWN = cvPoint(x, y);
            cvCircle((*LLR).pic, cvPoint(x, y),2,CV_RGB(255,0,0), 3);
            cvPutText((*LLR).pic, "bottom left", cvPoint(x+5, y), &(*LLR).font, CV_RGB(0,255,0));
            cout << "點擊左鍵記錄車道線右上座標!!" << endl;
            (*LLR).clickNum++;
        }
        else if ((*LLR).clickNum == 2)
        {
            (*LLR).point_RUP = cvPoint(x, y);
            cvCircle((*LLR).pic, cvPoint(x, y),2,CV_RGB(255,0,0), 3);
            cvPutText((*LLR).pic, "top right", cvPoint(x+5, y), &(*LLR).font, CV_RGB(0,255,0));
            cout << "點擊左鍵記錄車道線右下座標!!" << endl;
            (*LLR).clickNum++;
        }
        else if((*LLR).clickNum == 3)
        {
            (*LLR).point_RDOWN = cvPoint(x, y);
            cvCircle((*LLR).pic, cvPoint(x, y),2,CV_RGB(255,0,0), 3);
            cvPutText((*LLR).pic, "bottom right", cvPoint(x+5, y), &(*LLR).font, CV_RGB(0,255,0));
            cout << "此張圖車道線座標記錄完成!!" << endl;
            (*LLR).clickNum++;
            cout << "確認無誤請按鍵盤的return(enter)鍵，如果要重新點擊請點選滑鼠右鍵!!" <<endl;
        }
        else if((*LLR).clickNum == 4)
        {
            cout << "您已經完成此張圖的點擊，" << endl;
            cout << "確認無誤請按鍵盤的return(enter)鍵，\n如果要重新點擊請點選滑鼠右鍵!!" <<endl;
        }
        cvShowImage("LaneLocationRecord", (*LLR).pic);
    }
    //右鍵
    else if  ( event == cv::EVENT_RBUTTONDOWN )
    {
        (*LLR).restart();
        cout << "您可以重新定位車道線位置!!" << endl;
        cout << "點擊左鍵記錄車道線左上座標!!" << endl;
    }
    else if  ( event == cv::EVENT_MBUTTONDOWN )
    {
        if ((*LLR).clickNum == 4)
        {
            cout << "畫出定位的左右車道線段！" << endl;
            cvLine((*LLR).pic, (*LLR).point_LUP, (*LLR).point_LDOWN, CV_RGB(0, 0, 255), 2);
            cvLine((*LLR).pic, (*LLR).point_RUP, (*LLR).point_RDOWN, CV_RGB(0, 0, 255), 2);
            cvShowImage("LaneLocationRecord", (*LLR).pic);
        }
        else
        {
            cout << "尚未定位車道線，無法畫出定位的車道線段！" << endl;
        }
    }
}

int LaneLocationRecord::CallBackFuncForSqlite3LLR(void *userdata, int argc, char **argv, char **szColName)
{
    return 0;
}

//查詢
int SearchLaneLocation::CallBackFuncForSqlite3SearchLL(void *userdata, int argc, char **argv, char **szColName)
{
    SearchLaneLocation *SLL = (SearchLaneLocation *) userdata;
    //------dataTemp-----
    int tFileNum = -1;
    string tFileName = "null";
    int tpoint_LUP_X = -1;
    int tpoint_LUP_Y = -1;
    int tpoint_LDOWN_X = -1;
    int tpoint_LDOWN_Y = -1;
    int tpoint_RUP_X = -1;
    int tpoint_RUP_Y = -1;
    int tpoint_RDOWN_X = -1;
    int tpoint_RDOWN_Y = -1;
    int tisCouldNotDetermine = 0;
    //-------------------
    for(int i=0; i < argc; i++){
        string ColName = szColName[i];
        stringstream tmp;
        if (argv[i])
        {
            tmp << argv[i];
        }
        else
        {
            tmp << "-1";
        }
        
        if (ColName == "fileNum")
        {
            tmp >> tFileNum;
        }
        if (ColName == "fileName")
        {
            tmp >> tFileName;
        }
        if (ColName == "point_LUP_X")
        {
            tmp >> tpoint_LUP_X;
        }
        if (ColName == "point_LUP_Y")
        {
            tmp >> tpoint_LUP_Y;
        }
        if (ColName == "point_LDOWN_X")
        {
            tmp >> tpoint_LDOWN_X;
        }
        if (ColName == "point_LDOWN_Y")
        {
            tmp >> tpoint_LDOWN_Y;
        }
        if (ColName == "point_RUP_X")
        {
            tmp >> tpoint_RUP_X;
        }
        if (ColName == "point_RUP_Y")
        {
            tmp >> tpoint_RUP_Y;
        }
        if (ColName == "point_RDOWN_X")
        {
            tmp >> tpoint_RDOWN_X;
        }
        if (ColName == "point_RDOWN_Y")
        {
            tmp >> tpoint_RDOWN_Y;
        }
        if (ColName == "CouldNotDetermine")
        {
            tmp >> tisCouldNotDetermine;
        }
    }
    //-------------寫入查詢到的資料-------------
    if (tFileNum == (*SLL).fileNum)
    {
        (*SLL).point_LUP = cvPoint(tpoint_LUP_X, tpoint_LUP_Y);
        (*SLL).point_LDOWN = cvPoint(tpoint_LDOWN_X, tpoint_LDOWN_Y);
        (*SLL).point_RUP = cvPoint(tpoint_RUP_X, tpoint_RUP_Y);
        (*SLL).point_RDOWN = cvPoint(tpoint_RDOWN_X, tpoint_RDOWN_Y);
        (*SLL).fileName = tFileName;
        (*SLL).isCouldNotDetermine = tisCouldNotDetermine;
    }
    else
    {
        cout << "查詢編號異常，停止寫入！！" << endl;
    }
    
    return 0;
}
SearchLaneLocation::SearchLaneLocation()
{
    cout << "沒有任何輸入，查詢系統初始化失敗"<< endl;
    classInit = false;
    mode = 0;
    exit(-1);
}

SearchLaneLocation::SearchLaneLocation(sqlite3 *_db, string _tableName)
{
    db = _db;
    tableName = _tableName;
    cout << "外部載入資料庫查詢，模式:3" << endl;
    mode = 3;
    classInit = true;
    this->pointReset();
}

SearchLaneLocation::SearchLaneLocation(string _DB_URL, string _tableName)
{
    //Open sqlite3 DB
    int db_open_error = sqlite3_open(_DB_URL.c_str(), &db);
    if(db_open_error)
    {
        cout << "資料庫開啟失敗！！"<<endl;
        exit(-1);
    }
    else
    {
        cout << "資料庫創建或開啟完成！！"<<endl;
    }
    tableName = _tableName;
    cout << "模式:2" << endl;
    mode = 2;
    classInit = true;
    this->pointReset();
}

bool SearchLaneLocation::doSearch(int picNum)
{
    this->pointReset();
    if (!classInit)
    {
        cout << "類別尚未完成初始化！" << endl;
        return false;
    }
    fileNum = picNum;
    stringstream ss;
    ss << "select * from LaneLocationRecord where fileNum = ";
    ss << fileNum << " ;";
    
    char *szErrMsg = 0;
    int rc = sqlite3_exec(db, ss.str().c_str(), CallBackFuncForSqlite3SearchLL, this, &szErrMsg);
    if( rc != SQLITE_OK )
    {
        cout << "SQL error: " << szErrMsg << endl;
        sqlite3_free(szErrMsg);
    }
    else
    {
        cout << "查詢完成!!" << endl;
    }
    
    return true;
}

bool SearchLaneLocation::pointReset()
{
    point_LUP = cvPoint(-1, -1);
    point_LDOWN = cvPoint(-1, -1);
    point_RUP = cvPoint(-1, -1);
    point_RDOWN = cvPoint(-1, -1);
    isCouldNotDetermine = 0;
    return true;
}

CvPoint SearchLaneLocation::Point_LUP()
{
    return this->point_LUP;
}

CvPoint SearchLaneLocation::Point_LDOWN()
{
    return this->point_LDOWN;
}

CvPoint SearchLaneLocation::Point_RUP()
{
    return this->point_RUP;
}

CvPoint SearchLaneLocation::Point_RDOWN()
{
    return this->point_RDOWN;
}

bool SearchLaneLocation::IsCouldNotDetermine()
{
    if (isCouldNotDetermine == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

SearchLaneLocation::~SearchLaneLocation()
{
    if (classInit)
    {
        sqlite3_close(db);
    }
    cout << "關閉查詢資料庫！！" << endl;
}


