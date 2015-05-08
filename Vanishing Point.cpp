#define _USE_MATH_DEFINES
#include <cv.h>
#include <highgui.h>
#include <vector>
#include "Vanishing Point.h"
#include <iostream>
#include "VP_hough_source.h"
#include <cmath>
using namespace std;
//class of line_property
line_property::line_property(CvPoint& point_1, CvPoint& point_2) :line_point_1(&point_1), line_point_2(&point_2)
{
    //斜率
    if ((point_1.x - point_2.x) == 0)
        line_slope = -M_PI_2;
    else
        line_slope = (double)(point_1.y - point_2.y) / (point_1.x - point_2.x);
    
    //截距
    line_intercept = (double)(point_1.y - line_slope * point_1.x);
    
}

//class of line_crosspoint
line_crosspoint::line_crosspoint(double slope1, double intercept1, double slope2, double intercept2)
{
    //兩條線解聯立方程式，找出交點P(ptX,ptY)：
    LC_point.x = -(intercept1 - intercept2) / (slope1 - slope2);
    LC_point.y = intercept1 + (slope1 * LC_point.x);
    LC_score = 0;
}

CvPoint find_Vanishing_Point(IplImage* Canny, IplImage* Ori_pic, vector<line_property>* all_Slope, CvMemStorage* storage_DThrSmo)
{
    //vector<line_property> all_Slope =  vector<line_property>(); //線資料;
    vector<line_crosspoint> all_point;
    
    //宣告vector(存線密度資料)近去或夫拿資料
    vector<int> get_num_line_point;
    CvSeq* lines_DThrSmo = LD_cvHoughLines2(Canny, storage_DThrSmo, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 30, 10, &get_num_line_point);
    //輸入,儲存,變換方法,距離精度rho,角度精度theta,臨界值(線點數?),最小長度,最大間隔(線間隔)gap
    
#ifdef DRAW_LINE_DATA
    int truelinecount = 0;//算第幾條線
#endif
    int count = 0;
    for (int i = 0; i < lines_DThrSmo->total; i++)
    {
        
#ifdef DRAW_LINE_DATA
        cvNamedWindow("line_debug_pic");		//原圖
        IplImage* simg2 = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3); //顯示小圖結果
#endif
        
        CvPoint* ptThis = (CvPoint*)cvGetSeqElem(lines_DThrSmo, i);
        //過濾完全水平、垂直
        if (ptThis[0].x - ptThis[1].x == 0 && ptThis[0].y - ptThis[1].y == 0)
            continue;
        
        //建立線段資訊
        line_property line_info_temp(ptThis[0], ptThis[1]);
        count++;
        line_info_temp.num_line_point = get_num_line_point.at(get_num_line_point.size() - 1);
        if (get_num_line_point.size() != 0)
            get_num_line_point.pop_back();
        //bool ok = true;
        
#ifdef DRAW_ALL_BEFORE_LINE
        cvLine(Ori_pic, ptThis[0], ptThis[1], CV_RGB(125, 0, 10), 3, 8);
#endif
        /////////2015_4_3:過濾近水平、相似線暫時不考慮
        //(*my_all_Slope).push_back(line_info_temp);
        ////過濾相似線
        //if (!(abs(line_info_temp.line_slope) < 0.18)) //0.18 ~= tan 10度
        //{
        //	if (i != 0)//第一條線不做
        //	{
        //		for (int q = 0; q < (all_Slope).size(); q++)//比對之前所有斜率
        //		{
        //			//相同不收集
        //			if (abs(line_info_temp.line_slope - (all_Slope)[q].line_slope) < 0.01 && abs(line_info_temp.line_intercept - (all_Slope)[q].line_intercept) < 5)//0.01 1
        //			{
        //				ok = false;
        //				break;
        //			}
        //		}
        //	}
        //}
        ////過濾近水平線
        //else
        //{
        //	(*my_all_Slope).push_back(line_info_temp);
        //	ok = true;
        //}
        
        //全部儲存
        /*if (ok == true)
         {*/
        
#ifdef DRAW_ALL_AFTER_LINE
        cvLine(Ori_pic, ptThis[0], ptThis[1], CV_RGB(15, 0, 125), 3, 8);
#endif
#ifdef DRAW_LINE_DATA
        truelinecount++;
        cout << "第" << truelinecount << "線的截距" << line_info_temp.line_intercept << endl;
        cout << "第" << truelinecount << "線   " << "斜率M= " << line_info_temp.line_slope << endl;
        cout << "第" << truelinecount << "線點的數量:" << line_info_temp.num_line_point << endl;
        cvLine(Ori_pic, ptThis[0], ptThis[1], CV_RGB(15, 0, 125), 3, 8);
        cvResize(Ori_pic, simg2);
        cvShowImage("line_debug_pic", simg2);
        cvWaitKey(0);
        
        cvReleaseImage(&simg2);
#endif
        
        (*all_Slope).push_back(line_info_temp);
        
        //}
        //else
        //	continue;
        
    }//line total for
    
#ifdef DRAW_POINT_DATA
    int pointcount = 0;
#endif
    for (int f = 0; f < (*all_Slope).size(); f++)
    {
        for (int s = f + 1; s < (*all_Slope).size(); s++)
        {
            
            line_crosspoint LC_Temp((double)(*all_Slope)[f].line_slope, (double)(*all_Slope)[f].line_intercept, (double)(*all_Slope)[s].line_slope, (double)(*all_Slope)[s].line_intercept);
            if (LC_Temp.LC_point.x >= Canny->width || LC_Temp.LC_point.y >= Canny->height || LC_Temp.LC_point.x < 0 || LC_Temp.LC_point.y < 0)
                continue;
            else if ((LC_Temp.LC_point.y - (*all_Slope)[f].line_point_1[0].y) > 0 &&//2015_3_26新增排除路標交點
                     (LC_Temp.LC_point.y - (*all_Slope)[f].line_point_1[1].y) > 0 && //排除線在交點上的，並且角度大於80度的
                     !(abs((*all_Slope)[f].line_slope) < 5.6))
            {
#ifdef DRAW_APPROACH_VERTICAL
                cvLine(Ori_pic, cvPoint(LC_Temp.LC_point.x - 100, LC_Temp.LC_point.y - 100), cvPoint(LC_Temp.LC_point.x + 100, LC_Temp.LC_point.y + 100), CV_RGB(255, 255, 255), 1, 18);
                cvLine(Ori_pic, cvPoint(LC_Temp.LC_point.x - 100, LC_Temp.LC_point.y + 100), cvPoint(LC_Temp.LC_point.x + 100, LC_Temp.LC_point.y - 100), CV_RGB(255, 255, 255), 1, 18);
#endif
                continue;
            }
            else if ((LC_Temp.LC_point.y - (*all_Slope)[s].line_point_1[0].y) > 0 &&
                     (LC_Temp.LC_point.y - (*all_Slope)[s].line_point_1[1].y) > 0 &&
                     !(abs((*all_Slope)[s].line_slope) < 5.6))
            {
#ifdef DRAW_APPROACH_VERTICAL
                cvLine(Ori_pic, cvPoint(LC_Temp.LC_point.x - 100, LC_Temp.LC_point.y - 100), cvPoint(LC_Temp.LC_point.x + 100, LC_Temp.LC_point.y + 100), CV_RGB(255, 255, 255), 1, 18);
                cvLine(Ori_pic, cvPoint(LC_Temp.LC_point.x - 100, LC_Temp.LC_point.y + 100), cvPoint(LC_Temp.LC_point.x + 100, LC_Temp.LC_point.y - 100), CV_RGB(255, 255, 255), 1, 18);
#endif
                continue;
            }
            
#ifdef DRAW_POINT_DATA
            cvNamedWindow("point_debug_pic");		//原圖
            IplImage* simg3 = cvCreateImage(cvSize(1024, 720), 8, 3); //顯示小圖結果
            cout << "第" << pointcount << "個交點是由 第" << f + 1 << "號線 和 第" << s + 1 << "號線 構成" << endl;
            cout << "第" << pointcount << "個交點座標:   (" << LC_Temp.LC_point.x << "," << LC_Temp.LC_point.y << ")" << endl;
            cvLine(Ori_pic, LC_Temp.LC_point, (*all_Slope)[f].line_point_1[0], CV_RGB(0, 255, 0), 1, 8);
            cvLine(Ori_pic, LC_Temp.LC_point, (*all_Slope)[s].line_point_1[0], CV_RGB(0, 255, 0), 1, 8);
            //X
            cvLine(Ori_pic, cvPoint(LC_Temp.LC_point.x - 10, LC_Temp.LC_point.y - 10), cvPoint(LC_Temp.LC_point.x + 10, LC_Temp.LC_point.y + 10), CV_RGB(255, 0, 0), 1, 8);
            cvLine(Ori_pic, cvPoint(LC_Temp.LC_point.x - 10, LC_Temp.LC_point.y + 10), cvPoint(LC_Temp.LC_point.x + 10, LC_Temp.LC_point.y - 10), CV_RGB(255, 0, 0), 1, 8);
            cvResize(Ori_pic, simg3);
            cvShowImage("point_debug_pic", simg3);
            cvWaitKey(0);
            
            cvReleaseImage(&simg3);
            pointcount++;
#endif
            all_point.push_back(LC_Temp);
        }
    }
    //沒有交點
    if (all_point.size() < 1)
    {
        CvPoint no_p;
        no_p.x = 0;
        no_p.y = 0;
        return no_p;
    }
    
    //將交點的資訊寫入圖像中
    IplImage* IntegralImg = cvCreateImage(cvSize(Canny->width, Canny->height), IPL_DEPTH_8U, 1);
    cvZero(IntegralImg);
    unsigned char* IntegralImgdata = (unsigned char*)IntegralImg->imageData;
#ifdef CROSSPOINT_AREA
    IplImage* crossImg = cvCreateImage(cvSize(Canny->width, Canny->height), IPL_DEPTH_32F, 3);
    cvZero(crossImg);
#endif
    for (int i = 0; i < all_point.size(); i++)
    {
        int temp = all_point[i].LC_point.y * Canny->width + all_point[i].LC_point.x;
        IntegralImgdata[temp] += 1;
#ifdef CROSSPOINT_AREA
        CvPoint tmp = cvPoint(all_point[i].LC_point.x, all_point[i].LC_point.y);
        cvLine(crossImg, tmp, tmp, cvScalar(255, 255, 255), 1, 8);
#endif
    }
    
    
    
    
    
    
    CvMat *sumMat;
    //建立儲存影像積分的陣列，格式可為32-bit整數或64F浮點數
    sumMat = cvCreateMat(Canny->height + 1, Canny->width + 1, CV_64FC1);
    
    //計算積分影像
    cvIntegral(IntegralImg, sumMat);
    //積分影像資訊類別
    class max_info {
    public:
        float max;
        float first_x;
        float first_y;
        int sec_x;
        int sec_y;
        float score;
        
        max_info() {
            max = 0.0;
            first_x = 0.0;
            first_y = 0.0;
            sec_x = 0.0;
            sec_y = 0.0;
            score = 0.0;
        }
        void clear_score()
        {
            max = 0.0;
            score = 0.0;
        }
    };
    
    max_info Integral_info;
#ifdef INTEGRAL_SCORE
    int Integralscore[600][440] = { 0 };
#endif
    
    for (int y = 0; y < Canny->height - 40; y++)
    {
        for (int x = 0; x < Canny->width - 40; x++)
        {
            Integral_info.score = cvmGet(sumMat, y + 40, x + 40) + cvmGet(sumMat, y, x) - cvmGet(sumMat, y, x + 40) - cvmGet(sumMat, y + 40, x);
#ifdef INTEGRAL_SCORE
            Integralscore[x][y] = Integral_info.score;
#endif
            if (Integral_info.score > Integral_info.max)
            {
                Integral_info.max = Integral_info.score;
                Integral_info.first_x = x;
                Integral_info.first_y = y;
            }
        }
    }
    /*#ifdef INTEGRAL_SCORE
     for (int y = 0; y < Canny->height - 40; y++)
     {
     for (int x = 0; x < Canny->width - 40; x++)
     {
     cout<<"("<<x<<"."<<y<<") = "<<Integralscore[x][y]<<"  ";
     }
     }
     #endif*/
    cout << "找到之積分框數量最大值~~~~~~~~~~~~" << Integral_info.max << endl;
    //Integral_info.sec_x = Integral_info.first_x +20;
    //Integral_info.sec_y = Integral_info.first_y +20;
    Integral_info.clear_score();
    for (int i = Integral_info.first_y; i < Integral_info.first_y + 40 - 2; i++)
    {
        for (int j = Integral_info.first_x; j < Integral_info.first_x + 40 - 2; j++)
        {
            Integral_info.score = cvmGet(sumMat, i + 2, j + 2) + cvmGet(sumMat, i, j) - cvmGet(sumMat, i, j + 2) - cvmGet(sumMat, i + 2, j);
            if (Integral_info.score > Integral_info.max)
            {
                Integral_info.max = Integral_info.score;
                Integral_info.sec_x = j - 1;
                Integral_info.sec_y = i - 1;
            }
        }
    }
    
    
    CvPoint VSPoint = cvPoint(Integral_info.sec_x, Integral_info.sec_y);
    cvLine(Ori_pic, VSPoint, VSPoint, CV_RGB(255, 255, 255), 20, 8);//前
#ifdef CROSSPOINT_AREA
    cvNamedWindow("test");
    cvLine(crossImg, VSPoint, VSPoint, CV_RGB(255, 0, 0), 10, 8);
    cvShowImage("test", crossImg);
    cvWaitKey(0);
    
    cvReleaseImage(&crossImg);
#endif
    Check_VPoint(VSPoint.x, VSPoint.y);
    cvLine(Ori_pic, VSPoint, VSPoint, CV_RGB(50, 125, 255), 20, 8);//後//***
    
    //釋放IntegralImg
    cvReleaseImage(&IntegralImg);
    //CvMat刪除、釋放
    //if (sumMat != NULL)
    //cvDecRefData(sumMat);
    cvReleaseMat(&sumMat);
    return cvPoint(Integral_info.sec_x, Integral_info.sec_y);
}

/*
 char filename1[5000];
 char output[5000];
 const int NUM_FRAME = 5000;
 //#define	SEQ_NAME	"C:\\Users\\user\\Desktop\\LLSample\\output\\Video-%03d.jpg"//舊圖(1-832)
 //#define	SEQ_NAME	"C:\\Users\\user\\Desktop\\lane_pic\\output\\Out2\\Video-%03d.jpg"//隧道圖(1-213)
 #define	SEQ_NAME	"C:\\Users\\user\\Desktop\\lane_pic\\夜間車道線\\goodmothing\\PICTURE\\Video-%03d.jpg"//(1-1962)夜間公路
 //#define	SEQ_NAME	"C:\\Users\\user\\Desktop\\123\\Video-%03d.jpg"//test num_line_point
 //#define	SEQ_NAME   "C:\\Users\\user\\Desktop\\lane_pic\\夜間車道線\\1\\Video-%01d.jpg"
 //#define	SEQ_NAME   "C:\\Users\\user\\Desktop\\lane_pic\\NewPicout\\morning\\3\\vedio-%d.jpg"//下高速公路
 //#define	SEQ_NAME   "C:\Users\user\Desktop\lane_pic\NewPicout\morning\out"
 int CheckXY[5][2]={0};   //排序XY
 int AddressXY[5][2]={0}; //基礎XY
 int CPround;
 int _tmain(int argc, _TCHAR* argv[])
 {
 cvNamedWindow("Final_pic");		//原圖
 int nWinPosOffset=20;
 cvMoveWindow("Final_pic", nWinPosOffset+640, 0);
 
 CPround=0; //最剛開始初始化消失點紀錄
 
 for (int k = 232; k < 600; k++)
 {
 sprintf(filename1, SEQ_NAME, k);
 printf("%s\n", filename1);
 IplImage* Ori_pic = NULL;
 Ori_pic = cvLoadImage(filename1, 3);					//讀入原始圖片
 if (Ori_pic == NULL)
 continue;
 IplImage* Ori_pic_gray = cvLoadImage(filename1, 0);	//圖片變成灰階
 IplImage* Smo_pic =cvCreateImage(cvSize(Ori_pic_gray->width,Ori_pic_gray->height),Ori_pic_gray->depth,Ori_pic_gray->nChannels);
 IplImage* dst_DThrSmo =cvCreateImage(cvSize(Ori_pic_gray->width,Ori_pic_gray->height),Ori_pic_gray->depth,Ori_pic_gray->nChannels);
 cvCopy(Ori_pic_gray ,Smo_pic);
 cvSmooth(Ori_pic_gray, Smo_pic, CV_BLUR);
 cvCanny(Smo_pic, dst_DThrSmo, 30, 200, 3);				// 邊緣檢測(可能要改)
 #ifdef LOOK_CVCANNY
 cvNamedWindow("Edge_pic");
 cvMoveWindow("Edge_pic",nWinPosOffset,0);
 IplImage* canimg = cvCreateImage(cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),8,1);
 cvResize(dst_DThrSmo,canimg);
 cvShowImage("Edge_pic",canimg);
 #endif
 
 CvMemStorage* storage_DThrSmo = cvCreateMemStorage(0);		// 霍夫線轉換
 vector<line_property> all_Slope =  vector<line_property>(); //線資料
 
 CvPoint myPoint = find_Vanishing_Point(dst_DThrSmo,Ori_pic,&all_Slope,storage_DThrSmo);
 IplImage* simg = cvCreateImage(cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),8,3); //顯示小圖結果
 cvResize(Ori_pic,simg);
 cvShowImage("Final_pic", simg);
 cvReleaseImage(&simg);
 cvWaitKey(0);
 #ifdef LOOK_CVCANNY
 cvReleaseImage(&canimg);
 #endif
 cvReleaseImage(&Ori_pic);
 cvReleaseImage(&dst_DThrSmo);
 cvReleaseImage(&Ori_pic_gray);
 cvReleaseImage(&Smo_pic);
 _CrtDumpMemoryLeaks(); // 結束前輸出
 }//for()
 
 return 0;
 }*/
//====================================
//==============檢查消失點正確性======
void Check_VPoint(int &VPx, int &VPy)
{
    static int CheckXY[5][2] = { 0 };   //排序XY
    static int AddressXY[5][2] = { 0 }; //基礎XY
    static int CPround;
    
    int oldx = VPx; int oldy = VPy;
    if (CPround<CPIndex) //前五張丟陣列
    {
        AddressXY[CPround][0] = CheckXY[CPround][0] = VPx; //save X
        AddressXY[CPround][1] = CheckXY[CPround][1] = VPy; //save Y
    }
    else
    {
        for (int i = 0; i<CPIndex; i++) //CheckXY排序
            for (int j = 0; j<CPIndex - 1; j++)
            {
                if (CheckXY[j][0]>CheckXY[j + 1][0]){ //X 排序
                    int resx = CheckXY[j][0];
                    CheckXY[j][0] = CheckXY[j + 1][0];
                    CheckXY[j + 1][0] = resx;
                }
                if (CheckXY[j][1]>CheckXY[j + 1][1]){ //Y 排序
                    int resy = CheckXY[j][1];
                    CheckXY[j][1] = CheckXY[j + 1][1];
                    CheckXY[j + 1][1] = resy;
                }
            }
        //===================
        //cout << CheckXY[0][0] << "  "<< CheckXY[1][0] << "  "<< CheckXY[2][0] << "  "<< CheckXY[3][0] << "  "<< CheckXY[4][0]  <<  endl;
        //cout << AddressXY[0][0] << "  "<< AddressXY[1][0] << "  "<< AddressXY[2][0] << "  "<< AddressXY[3][0] << "  "<< AddressXY[4][0]  <<  endl;
        int midx = CheckXY[2][0]; //中間的X
        int midy = CheckXY[2][1];//中間的Y
        
        VPx = midx;
        VPy = midy;
        //if(abs(VPx-midx)>30){  VPx=midx; cout <<endl<< "修正X ★★★★★★ "<< midx <<endl; }//誤差太大(20) 修正
        //if(abs(VPy-midy)>30){  VPy=midy; cout <<endl<< "修正Y ☆☆☆☆☆☆ "<<  midy <<endl; }//誤差太大(20) 修正
        
        for (int i = 1; i<CPIndex; i++) {
            AddressXY[i - 1][0] = AddressXY[i][0]; //基礎搬移X
            AddressXY[i - 1][1] = AddressXY[i][1]; //基礎搬移Y
            CheckXY[i - 1][0] = AddressXY[i - 1][0];
            CheckXY[i - 1][1] = AddressXY[i - 1][1];
        }
        AddressXY[CPIndex - 1][0] = CheckXY[CPIndex - 1][0] = oldx; //save X
        AddressXY[CPIndex - 1][1] = CheckXY[CPIndex - 1][1] = oldy; //save Y
        
    }
    
    if (CPround <= CPIndex)
        CPround++;
    //if(CPround ==15) CPround=0;
    //cout << AddressXY[0][0] << "  "<< AddressXY[1][0] << "  "<< AddressXY[2][0] << "  "<< AddressXY[3][0] << "  "<< AddressXY[4][0]  <<  endl;
}

