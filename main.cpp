#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include "Vanishing Point.h"
#include "Canny Line.h"
#include "Find the best lines.h"
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



int main(int argc, char *argv[])
{
	char FileName[200], FileName2[200], FileName3[200], maskout[200];
	int oldXX=0,oldYY=0;
	//第一張出現的圖片編號
	int  FristPic = 4;  
	
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
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i);    //★日間高速
		sprintf(FileName2, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i+1); //★日間高速
		//sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+1);
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
		

		IplImage *pImgC = cvCreateImage(cvSize(pImgA->width, pImgA->height), pImgA->depth, pImgA->nChannels); //空圖層 初始化
		cvCopy(pImgA, pImgC); //img1 copy to imgout
		
		//===========================================================


		//===========疊圖=====================
		//疊圖(input1,input2, *output)
			pImgC = xxhh(pImgA, pImgB, pImgC);  
	
		//============canny==========
			pImgCanny = canny(pImgC, pImgBuffer);
			
		//===========對比線(點)===================
			pImgC = drawline(pImgC, oldXX, oldYY); //drawline (輸入圖片,消失點X,消失點Y)//劃出對比點(取得)
            cvShowImage("canny",pImgCanny);
            cout << "======END DRAWLINE========="<<endl;
		 
		//============遮罩canny圖 (如果是夜間模式請取消)=======================
			cvSet(pImgFilter,cvScalar(0,0,0));
			Filter_Init(oldXX);//VanishingPoint.x
			

			//============消失點function===========
            //Find the vanishing point
            //Create savePoint of Vector<CvPoint>
            static vector<line_property> LSPointSave,RSPointSave;
            //call vanishing point function and get the vanishing point to vpfnpoint
           CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny, pImgC, &LSPointSave, &RSPointSave);
            cvLine( pImgColor, VanishingPoint, VanishingPoint, CV_RGB(0,255,0), 7);
            
			//檢查消失點正確性
            Check_VPoint(VanishingPoint.x,VanishingPoint.y);
            //cvLine(pImgColor,VanishingPoint,VanishingPoint,CV_RGB(225,0,0),20,4); //劃出消失點
            
            cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl << endl;// 產生消失點(debug)
            //把第一個消失點放進 對比點陣列(100是上限)
            RLpoint[0][0] = VanishingPoint.x; RLpoint[0][1] = VanishingPoint.y;
			oldXX=VanishingPoint.x;oldYY=VanishingPoint.y;


           
            //=======黃建富===================
            //draw the vanishing point range
            if (VanishingPoint.x != NULL || VanishingPoint.y != NULL)
                draw_VPoint(pImgColor, VanishingPoint.x, VanishingPoint.y, vp_range);
            //Find the best lines (Original picture,Full Canny,Vanishing point(CvPoint),Vanishing point range(vp_range)）
        /***    FTBL ftblData = FindTheBestLines(pImgColor, &LSPointSave, &RSPointSave, VanishingPoint, vp_range);  ***/
		

		/*//車道偏移
		if( Lane_Offset(VanishingPoint, ftblData.FTBL_Point_L.x, ftblData.FTBL_Point_R.x)==true){
			CvFont font;
			// PlaySound(TEXT("C:\\Users\\user\\Desktop\\AudioJoiner140604213842.wav"),NULL,SND_FILENAME | SND_SYNC );
			cvInitFont(&font,CV_FONT_HERSHEY_TRIPLEX  ,4.0f,1.0f,0,3,CV_AA);
			cvPutText(pImgColor,"Warning!" , Point(640/4,480/2),& font ,CV_RGB(255,0,0));
		}
		else if (Lane_Offset(VanishingPoint, ftblData.FTBL_Point_L.x, ftblData.FTBL_Point_R.x) == false){
		}*/
		
			
		

		//sprintf(maskout, "C:\\Users\\User\\Desktop\\LLSample\\Mask Test\\Mask\\MonXinFilterColor\\Video-%d.jpg",i);
		//cvSaveImage(maskout,pImgCanny);

		// Create Windows
		cvShowImage("pImgColor", pImgColor);
		//cvShowImage("pImgCanny", pImgCanny);

		 cvReleaseImage(&pImgC); //釋放記憶體
		cvReleaseImage(&pImgColor);
		//if(i==400) waitKey(0);
		waitKey(1);
	}
	}
	cvReleaseImage(&pImgA);
	cvReleaseImage(&pImgB);
	//cvReleaseImage(&pImgBlack); 
	cvReleaseImage(&pImgCanny);  
	cvReleaseImage(&pImgFilter);
	
	
	return 0;
}




