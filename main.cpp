#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include "Vanishing Point.h"
#include "Canny Line.h"
#include "Find the best lines.h"

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

int main(int argc, char *argv[])
{
	char FileName[200], FileName2[200], FileName3[200], maskout[200];
	
	//第一張出現的圖片編號
	int  FristPic = 1;  
	//Mask 初始化
	Mask_Init(); 
	for (int i = FristPic; i < 3000; i += 1){


		//===========圖片brbrbr
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\夜間車道線\\goodmothing\\PICTURE\\Video-%d.jpg", i); //★夜間高速
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\夜間車道線\\goodmothing\\PICTURE\\Video-%d.jpg", i + 9); //★夜間高速
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\日間車道線\\\good0002\\PICTURE\\video-%d.jpg",i);  //★日間街道
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\日間車道線\\\good0002\\PICTURE\\video-%d.jpg",i+9);//★日間街道
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\日間車道線\\VIDEO0003 (2014-4-20 下午 10-10-12)\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\日間車道線\\VIDEO0003 (2014-4-20 下午 10-10-12)\\Video-%d.jpg",i+1);
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\夜間車道線\\CIMG3461 (2014-4-20 下午 10-15-45)\\Video-%d.jpg", i);
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\夜間車道線\\CIMG3461 (2014-4-20 下午 10-15-45)\\Video-%d.jpg", i + 1);
		sprintf(FileName, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i);    //★日間高速
		sprintf(FileName2, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i+1); //★日間高速
		//sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+1);
		////sprintf(FileName3, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+2);

		//===================讀進圖片(重設大小)==================
		//=======================================================
		IplImage *src1 = cvLoadImage(FileName, 0); //讀進原圖
		IplImage *src2 = cvLoadImage(FileName2, 0);//讀進原圖
		IplImage *src3 = cvLoadImage(FileName2, 1);//讀取彩圖
		if(src1==NULL||src2==NULL||src3==NULL) break;

		CvSize pImgA_size; 
		CvSize pImgB_size;  
		CvSize pImgColor_size; //新的pic大小


		pImgA_size.width = src1->width* (1280.0 / src1->width); //重設pImgA大小
		pImgA_size.height = src1->height* (720.0 / src1->height);
		pImgB_size.width = src2->width* (1280.0 / src2->width); //重設pImgB大小
		pImgB_size.height = src2->height* (720.0 / src2->height);
		cout << pImgA_size.width << "   " << pImgA_size.height << "         "<<endl;  //DBG用

		IplImage *pImgA = cvCreateImage(pImgA_size, src1->depth, src1->nChannels); //創立目標影像A
		IplImage *pImgB = cvCreateImage(pImgB_size, src2->depth, src2->nChannels); //創立目標影像B
		IplImage *pImgColor = cvCreateImage(cvSize(src2->width* (1280.0 / src2->width), src2->height* (720.0 / src2->height)), src3->depth, src3->nChannels);
		
		//改變大小
		cvResize(src1, pImgA, CV_INTER_LINEAR); 
		cvResize(src2, pImgB, CV_INTER_LINEAR);
		cvResize(src3, pImgColor, CV_INTER_LINEAR);

		cvReleaseImage(&src1);
		cvReleaseImage(&src2);
		cvReleaseImage(&src3);

		//前置處理======
		

		IplImage *pImgC = cvCreateImage(cvSize(pImgA->width, pImgA->height), pImgA->depth, pImgA->nChannels); //空圖層 初始化
		cvCopy(pImgA, pImgC); //img1 copy to imgout

		//===========================================================



		//===========疊圖=====================
		//疊圖(input1,input2, *output)
		pImgC = xxhh(pImgA, pImgB, pImgC);  

		//============第一張先做canny==========
		//第一張的canny(沒有過濾)
		if (i == FristPic){  
			pImgCanny = canny(pImgC, pImgBuffer);
		}

		//============消失點function===========
		//Find the vanishing point
		//Create savePoint of Vector<CvPoint>
		static vector<CvPoint> savePoint;
		//call vanishing point function and get the vanishing point to vpfnpoint
		CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny, pImgC, &savePoint);
		cvLine( pImgColor, VanishingPoint, VanishingPoint, CV_RGB(0,255,0), 7);

		//檢查消失點正確性
		Check_VPoint(VanishingPoint.x,VanishingPoint.y); 
		//cvLine(pImgColor,VanishingPoint,VanishingPoint,CV_RGB(225,0,0),20,4); //劃出消失點

		cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl << endl;// 產生消失點(debug)
		//把第一個消失點放進 對比點陣列(100是上限)
		RLpoint[0][0] = VanishingPoint.x; RLpoint[0][1] = VanishingPoint.y; 

		//============後圖的canny==============
		if (i != FristPic) pImgCanny = canny(pImgC, pImgBuffer); //之後再做的canny
		cout << "Video-" << i - 1 << "and Video-" << i << "print out C" << endl; //debug

		//===========對比線(點)===================
		pImgC = drawline(pImgC, VanishingPoint.x, VanishingPoint.y); //drawline (輸入圖片,消失點X,消失點Y)//劃出對比點(取得)

		//========================================
		//draw the vanishing point range
		if (VanishingPoint.x != NULL || VanishingPoint.y != NULL)
			draw_VPoint(pImgColor, VanishingPoint.x, VanishingPoint.y, vp_range);
		//Find the best lines (Original picture,Full Canny,Vanishing point(CvPoint),Vanishing point range(vp_range)）
		FindTheBestLines(pImgColor, pImgCanny, VanishingPoint, vp_range);

		//車道偏移
		if( Lane_Offset(VanishingPoint,100,500)==-1){
		cvLine( pImgColor, cvPoint(0,0), cvPoint(1270,0), CV_RGB(255,0,0), 15);
		cvLine( pImgColor, cvPoint(0,0), cvPoint(0,715), CV_RGB(255,0,0), 15);
		}
		else if( Lane_Offset(VanishingPoint,100,500)==1){
		cvLine( pImgColor, cvPoint(0,0), cvPoint(1270,0), CV_RGB(255,0,0), 15);
		cvLine( pImgColor, cvPoint(0,0), cvPoint(0,715), CV_RGB(255,0,0),15);
		}
		

		//============遮罩canny圖 (如果是夜間模式請取消)=======================
		cvSet(pImgFilter,cvScalar(0,0,0));
		Filter_Init(VanishingPoint.x);

		//sprintf(maskout, "C:\\Users\\User\\Desktop\\LLSample\\Mask Test\\Mask\\MonXinFilterColor\\Video-%d.jpg",i);
		//cvSaveImage(maskout,pImgCanny);

		// Create Windows
		cvShowImage("pImgColor", pImgColor);
		//cvShowImage("pImgCanny", pImgCanny);

		cvReleaseImage(&pImgA); cvReleaseImage(&pImgB); cvReleaseImage(&pImgC);//cvReleaseImage(&pImgD); //釋放記憶體
		cvReleaseImage(&pImgColor);
		//if(i==400) waitKey(0);
		waitKey(1);
	}

	
	//cvReleaseImage(&pImgBlack); 
	cvReleaseImage(&pImgCanny);  
	cvReleaseImage(&pImgFilter);
	
	
	return 0;
}




