﻿#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <cxcore.h>
#include <cassert>
#include "Vanishing Point.h"
#include "Canny Line.h"
#include "Find the best lines.h"




#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#include <windows.h>
//#pragma comment(lib, "Winmm.lib")

/********************
*****Commit7/7*******
********************/

using namespace std;
using namespace cv;

//圖像檔案位置
//#define	SEQ_NAME	"/Users/chienfu/Desktop/車道線偵測/demo/Video- (%d).jpg"
//#define	SEQ_NAME	"D:\\車道線photo\\demo\\Video- (%d).jpg"
//#define	SEQ_NAME	"D:\\車道線photo\\Maskout\\Video-%d.jpg"
//#define	SEQ_NAME	"D:\\車道線photo\\downtown\\Video-%d.jpg"
//#define	SEQ_NAME	"D:\\車道線photo\\night\\Vidio- (%d).jpg"
//#define	SEQ_NAME	"D:\\車道線photo\\freeway2\\Vidoi- (%d).jpg"


//畫出消失點位置與周圍感應區
//設定偵測消失點的上下左右多少範圍
#define vp_range 50
#define DBGflag 1 //##debug 資訊開關

void ImageMerge(IplImage*& pImageRes);



int main(int argc, char *argv[])
{
	char FileName[200], FileName2[200], FileName3[200], maskout[200];
	int oldXX=0,oldYY=0;
	//第一張出現的圖片編號
	int  FristPic =30;  
	
	for (int i = FristPic; i < 3000; i += 1){

		
		//===========圖片brbrbr
		
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\meeting Video\\市區15s\\rainday\\Video-%d.jpg",i);    //★雨天
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\meeting Video\\市區15s\\rainday2\\Video-%d.jpg",i);    //★雨天2
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\meeting Video\\市區15s\\市區day\\Video-%d.jpg",i);    //★市區
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\meeting Video\\市區15s\\市區night\\Video-%d.jpg",i);    //★市區夜間
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i); //★日間高速(sing)
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\夜間車道線\\CIMG3461 (2014-4-20 下午 10-15-45)\\Video-%d.jpg",i); //★夜間高速
		//sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i); //
		//-------------------new pic----------------------------------
		sprintf(FileName2, "C:\\Users\\user\\Desktop\\NewPicout\\morning\\3\\vedio-%d.jpg",i);


		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\Xian開會影片\\out\\Video-%d.jpg",i); //
		//sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);//★日間高速
		////sprintf(FileName3, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+2);

		//Mask 初始化 (在第一張 只做一次)
		if(i==FristPic)
		{
		Mask_Init(FileName2); 
		}
		else
		{

		//===================讀進圖片(重設大小)==================
		//=======================================================
		
		IplImage *src2 = cvLoadImage(FileName2, 0);//讀進原圖
		IplImage *src3 = cvLoadImage(FileName2, 1);//讀取彩圖
		
		if(src2==NULL||src3==NULL) break;

		
		CvSize pImgB_size;  
		CvSize pImgColor_size; //新的pic大小


		
		pImgB_size.width = src2->width* (640.0 / src2->width); //重設pImgB大小
		pImgB_size.height = src2->height* (480.0 / src2->height);
		//cout << pImgA_size.width << "   " << pImgA_size.height << "         "<<endl;  //DBG用
		
		
		IplImage *pImgColor = cvCreateImage(cvSize(src2->width* (640.0 / src2->width), src2->height* (480.0 / src2->height)), src3->depth, src3->nChannels);
		
		
		cvCopy(pImgB, pImgA); //img1 copy to imgout
		//改變大小
		cvResize(src2, pImgB, CV_INTER_LINEAR);
		cvResize(src3, pImgColor, CV_INTER_LINEAR);


		//cvReleaseImage(&src1);
		cvReleaseImage(&src2);
		cvReleaseImage(&src3);

		//前置處理======
		IplImage *pImgMar = cvCreateImage(cvSize(pImgColor->width*2, pImgColor->height*2), pImgA->depth ,pImgA->nChannels);  ///##Debug   
		cvZero(pImgMar);
		cvCopy(pImgA, pImgC); //img1 copy to imgout
		
		//====================================
		//===========疊圖=====================
		//疊圖(input1,input2, *output)

			pImgC = xxhh(pImgA, pImgB, pImgC,pImgColor);  
			//cvShowImage("canny",pImgC);

		//============canny===================
			//pImgCanny = canny(pImgGrayC, pImgBuffer);
			cvCopy(pImgGrayC,pImgCanny);
			
			
		//===========對比線(點)===============
			//pImgC = drawline(pImgC, oldXX, oldYY); //drawline (輸入圖片,消失點X,消失點Y)//劃出對比點(取得)
           // cout << "======END DRAWLINE========="<<endl;
		 
		//============遮罩canny圖 (如果是夜間模式請取消)=======================
			/***if(i>FristPic+5){
			cvSet(pImgFilter,cvScalar(0,0,0));
			Filter_Init(oldXX,oldYY);//VanishingPoint.x 
			//cvShowImage("canny",pImgCanny);
			}***/

			//cvSet(pImgFilter,cvScalar(0,0,0));
			//Filter_Init(oldXX,oldYY);//VanishingPoint.x 

			//============消失點function===========
            //記錄霍夫線段資訊
            static vector<line_property> AllVPHoughLineSlopeRecorder;
            //for vp
            CvMemStorage* storage_DThrSmo = cvCreateMemStorage(0);
            //call vanishing point function and get the vanishing point to VanishingPoint
            CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny, pImgC, &AllVPHoughLineSlopeRecorder, storage_DThrSmo);
            cvLine( pImgColor, VanishingPoint, VanishingPoint, CV_RGB(0,255,0), 7);
            
			//檢查消失點正確性
            Check_VPoint(VanishingPoint.x,VanishingPoint.y);
            //cvLine(pImgColor,VanishingPoint,VanishingPoint,CV_RGB(225,0,0),20,4); //劃出消失點
            
            cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl << endl;// 產生消失點(debug)
            //把第一個消失點放進 對比點陣列(100是上限)
            RLpoint[0][0] = VanishingPoint.x; RLpoint[0][1] = VanishingPoint.y;
			oldXX=VanishingPoint.x;oldYY=VanishingPoint.y;


           
            //==============================
            //draw the vanishing point range
            if (VanishingPoint.x != 0.0 || VanishingPoint.y != 0.0)
                draw_VPoint(pImgColor, VanishingPoint.x, VanishingPoint.y, vp_range);
            //Find the best lines (Original picture,Full Canny,Vanishing point(CvPoint),Vanishing point range(vp_range)）
            FTBL ftblData = FindTheBestLines(pImgColor, &AllVPHoughLineSlopeRecorder,  VanishingPoint, vp_range);
            //清除車道線斜率記錄vector
            vector <line_property> Nhl;
            AllVPHoughLineSlopeRecorder.clear();
            AllVPHoughLineSlopeRecorder.swap(Nhl);
            
            
			//release storage_DThrSmo
			cvReleaseMemStorage(&storage_DThrSmo);

		//車道偏移
		//if( Lane_Offset(VanishingPoint, ftblData.FTBL_Point_L.x, ftblData.FTBL_Point_R.x)==true){
		//	CvFont font;
		//	// PlaySound(TEXT("C:\\Users\\user\\Desktop\\AudioJoiner140604213842.wav"),NULL,SND_FILENAME | SND_SYNC );
		//	cvInitFont(&font,CV_FONT_HERSHEY_TRIPLEX  ,4.0f,1.0f,0,3,CV_AA);
		//	cvPutText(pImgColor,"Warning!" , Point(640/4,480/2),& font ,CV_RGB(255,0,0));
		//}
		//else if (Lane_Offset(VanishingPoint, ftblData.FTBL_Point_L.x, ftblData.FTBL_Point_R.x) == false){
		//}
		
		//=========DBG開關=======
		if(DBGflag==1){
			//====
			char ss[100];
			sprintf(ss, "%d",i);
			CvFont font;
			// PlaySound(TEXT("C:\\Users\\user\\Desktop\\AudioJoiner140604213842.wav"),NULL,SND_FILENAME | SND_SYNC );
			cvInitFont(&font,CV_FONT_HERSHEY_TRIPLEX  ,2.0f,1.0f,0,2,CV_AA);
			cvPutText(pImgColor,ss , Point(30,30),& font ,CV_RGB(255,0,0));
			//==

			cvCvtColor(pImgColor,pImgGrayC,	CV_RGB2GRAY);
			ImageMerge(pImgMar);
			cvNamedWindow("ImageMerge", CV_WINDOW_NORMAL);
			//設定Window屬性為全螢幕,所以就不會有最上面的Title Bar
			//cvSetWindowProperty("ImageMerge", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
			cvShowImage("ImageMerge", pImgMar);
		};


		//sprintf(maskout, "C:\\Users\\user\\Desktop\\Xian開會影片\\10_15_meeting\\市區day\\output-%d.jpg",i);
		//cvSaveImage(maskout,pImgColor);

		// Create Windows
		cvShowImage("pImgColor", pImgColor);
		//cvShowImage("pImgCanny", pImgCanny);

		//cvReleaseImage(&pImgDCanny); 

		 cvReleaseImage(&pImgColor);
		 cvReleaseImage(&pImgMar);
		//if(i==400) waitKey(0);
		waitKey(1);
		
		//_CrtDumpMemoryLeaks();		

	}
	}
	cvReleaseImage(&pImgA);
	cvReleaseImage(&pImgB);
	//cvReleaseImage(&pImgBlack); 
	cvReleaseImage(&pImgCanny);  
	//cvReleaseImage(&pImgFilter);
	cvReleaseImage(&pImgC); //釋放記憶體
	cvReleaseImage(&pImgGrayC);
	//cvReleaseImage(&pImgHouf);
	
	return 0;
}


void ImageMerge(IplImage*& pImageRes){

	//載入Canny圖到目標圖像
    cvSetImageROI(pImageRes, cvRect(0, pImgCanny->height-50,  pImgCanny->width,pImgCanny->height));
    cvCopy(pImgCanny, pImageRes);
    cvResetImageROI(pImageRes);

	////載入Houf圖到目標圖像 pImgHouf
    cvSetImageROI(pImageRes, cvRect(pImgHouf->width, pImgHouf->height-50, pImgHouf->width, pImgHouf->height));
    cvCopy(pImgHouf, pImageRes);
    cvResetImageROI(pImageRes);

	////載入灰階結果圖到目標圖像
    cvSetImageROI(pImageRes, cvRect(0, 0, pImgGrayC->width, pImgGrayC->height));
    cvCopy(pImgGrayC, pImageRes);
    cvResetImageROI(pImageRes);
	/*cvSetImageROI(pImageRes, cvRect(0, 0, pImgFilter->width, pImgFilter->height));
    cvCopy(pImgFilter, pImageRes);
    cvResetImageROI(pImageRes);*/
    
	////載入對比點圖到目標圖像
    cvSetImageROI(pImageRes, cvRect(pImgC->width, 0, pImgC->width, pImgC->height));
    cvCopy(pImgC, pImageRes);
    cvResetImageROI(pImageRes);

	

	

	//cvReleaseImage(&pImageA);
	//cvReleaseImage(&pImageB);
}

