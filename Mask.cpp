#include "StdAfx.h" //"C:\\Users\\User\\Desktop\\LLSample\\output\\Video-TEST.jpg"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "math.h"
#include <vector>
#include "Vanishing Point.h"
#include "Canny Line.h"


using namespace cv;
using namespace std; 


char FileName[200],FileName2[200],FileName3[200],maskout[200];
int  FristPic  = 120;  //第一張出現的圖片編號


int main( int argc, char** argv ){

	
	Mask_Init(); //Mask 初始化

//=============開始程式================
for(int i =FristPic;i<600;i+=1){ 

	
	//===========圖片brbrbr
	//sprintf(FileName, "C:\\Users\\user\\Desktop\\日間車道線\\VIDEO0003 (2014-4-20 下午 10-10-12)\\Video-%d.jpg",i);
	//sprintf(FileName2, "C:\\Users\\user\\Desktop\\日間車道線\\VIDEO0003 (2014-4-20 下午 10-10-12)\\Video-%d.jpg",i+1);
	//sprintf(FileName, "C:\\Users\\user\\Desktop\\夜間車道線\\CIMG3461 (2014-4-20 下午 10-15-45)\\Video-%d.jpg",i);
	//sprintf(FileName2, "C:\\Users\\user\\Desktop\\夜間車道線\\CIMG3461 (2014-4-20 下午 10-15-45)\\Video-%d.jpg",i+1);
	//sprintf(FileName, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i);
	//sprintf(FileName2, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i+1);
	sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);
    sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+1);
	////sprintf(FileName3, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+2);

	//===================讀進圖片(重設大小)==================
	//=======================================================
	IplImage *src1=cvLoadImage(FileName,0); //讀進原圖
	IplImage *src2=cvLoadImage(FileName2,0);//讀進原圖
	
	CvSize pImgA_size;CvSize pImgB_size;  //新的pic大小


 pImgA_size.width=src1->width* (1280.0/src1->width); //重設pImgA大小
 pImgA_size.height=src1->height* (720.0/src1->height);
 pImgB_size.width=src2->width* (1280.0/src2->width); //重設pImgB大小
 pImgB_size.height=src2->height* (720.0/src2->height);
 cout << pImgA_size.width << "   "<< pImgA_size.height << "         "  ;  //DBG用

 IplImage *pImgA = cvCreateImage( pImgA_size, src1->depth, src1->nChannels); //創立目標影像A
 IplImage *pImgB = cvCreateImage( pImgB_size, src2->depth, src2->nChannels); //創立目標影像B
 cvResize(src1, pImgA, CV_INTER_LINEAR); //改變大小
 cvResize(src2, pImgB, CV_INTER_LINEAR); //改變大小
 cvReleaseImage(&src1);
 cvReleaseImage(&src2);

//前置處理======
 IplImage *pImgColor = cvLoadImage(FileName2,1);
 IplImage *pImgC = cvCreateImage(cvSize(pImgA->width,pImgA->height),pImgA->depth,pImgA->nChannels); //空圖層 初始化
 cvCopy(pImgA ,pImgC); //img1 copy to imgout
 
 //===========================================================



 //===========疊圖=====================
pImgC= xxhh(pImgA,pImgB,pImgC);  //疊圖(input1,input2, *output)

//============第一張先做canny==========
if (i==FristPic){  //第一張的canny(沒有過濾)
	pImgCanny=canny(pImgC,pImgBuffer);}

//============消失點function===========
CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny,pImgC); //canny進去找消失點
cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl<<endl;// 產生消失點(debug)
RLpoint[0][0]=VanishingPoint.x;RLpoint[0][1]=VanishingPoint.y; //把第一個消失點放進 對比點陣列(100是上限)


//============後圖的canny==============

if(i!=FristPic) pImgCanny=canny(pImgC,pImgBuffer); //之後再做的canny

cout<< "Video-"<<i-1<<"and Video-"<< i << "print out C" << endl; //debug


 //===========對比線(點)===================
 pImgC=drawline(pImgC,VanishingPoint.x,VanishingPoint.y); //drawline (輸入圖片,消失點X,消失點Y)//劃出對比點(取得)

 
//============遮罩canny圖=======================
  Filter_Init(VanishingPoint.x);


 cvLine(pImgCanny,VanishingPoint,VanishingPoint,CV_RGB(225,225,225),20,4); //劃出消失點


//sprintf(maskout, "C:\\Users\\User\\Desktop\\LLSample\\Mask Test\\Mask\\MonXinFilterColor\\Video-%d.jpg",i);
//cvSaveImage(maskout,pImgCanny);

    //  /// Create Windows
   namedWindow("CV Lab", 1);
   cv::Mat cvMatImg(pImgCanny, 0);imshow( "CV Lab", cvMatImg );
 

   cvReleaseImage(&pImgA);cvReleaseImage(&pImgB);cvReleaseImage(&pImgC);//cvReleaseImage(&pImgD); //釋放記憶體
   cvReleaseImage(&pImgColor);
  //if(i==400) waitKey(0);
  waitKey(1);
}
 cvReleaseImage(&pImgBlack);cvReleaseImage(&pImgCanny);cvReleaseImage(&pImgBuffer);cvReleaseImage(&pImgFilter);
 waitKey(0);
   return 0;
}
