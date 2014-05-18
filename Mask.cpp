#include "StdAfx.h" //"C:\\Users\\User\\Desktop\\LLSample\\output\\Video-TEST.jpg"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "math.h"
#include <vector>
#include "Vanishing Point.h"


using namespace cv;
using namespace std; 

IplImage* xxhh(const IplImage *img1,const IplImage *img2);
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo);  //canny(輸入圖片,緩衝圖層)
IplImage* drawline(IplImage *pImgC,int centerX,int centerY);
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgC);

int RLpoint[100][2]={0};
char FileName[200],FileName2[200],FileName3[200],maskout[200];
int  FristPic  =  1;  //第一張出現的圖片編號


int main( int argc, char** argv ){

	 #ifdef _DEBUG
_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
//_CrtSetBreakAlloc(#);
#endif
 IplImage *pImgCanny = cvCreateImage(cvSize(1280,720),8,1); //產生canny圖
 IplImage *pImgBlack = cvCreateImage( cvSize(1280,720), 8,1); //全黑圖層
 IplImage *pImgBuffer =cvCreateImage(cvSize(1280,720), 8,1); //緩衝圖層初始化
 for(int h=0;h<pImgBlack->height;h++) 
		for(int w = 0;w< pImgBlack->widthStep ;w++){
			pImgBlack->imageData[h*pImgBlack->widthStep+w]=0;
			pImgBlack->imageData[h*pImgBlack->widthStep+w+1]=0;
			pImgBlack->imageData[h*pImgBlack->widthStep+w+2]=0;}


 //  /// Create Windows
   namedWindow("CV Lab", 1);
   
   
 //  //imwrite( "girl_blending.jpg", overpro );
for(int i =FristPic;i<250;i+=1){

	for(int j=0;j<100;j++){ RLpoint[j][0]=0;RLpoint[j][1]=0;}//陣列初始化
	
	//for(int ss=0;ss<100;ss++){cout << RLpoint[ss][0]<<"  "<< RLpoint[ss][1]<< endl;}
	
	//===========原wj6qu04
	//sprintf(FileName, "C:\\Users\\user\\Desktop\\日間車道線\\VIDEO0003 (2014-4-20 下午 10-10-12)\\Video-%d.jpg",i);
	//sprintf(FileName2, "C:\\Users\\user\\Desktop\\日間車道線\\VIDEO0003 (2014-4-20 下午 10-10-12)\\Video-%d.jpg",i+1);
	//sprintf(FileName, "C:\\Users\\user\\Desktop\\夜間車道線\\CIMG3461 (2014-4-20 下午 10-15-45)\\Video-%d.jpg",i);
	//sprintf(FileName2, "C:\\Users\\user\\Desktop\\夜間車道線\\CIMG3461 (2014-4-20 下午 10-15-45)\\Video-%d.jpg",i+1);
	//sprintf(FileName, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i);
	//sprintf(FileName2, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i+1);
	sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);
    sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+1);
	////sprintf(FileName3, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+2);

	//=======================
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


 


 IplImage *pImgColor = cvLoadImage(FileName2,1);
 IplImage *pImgC = cvCreateImage(cvSize(pImgA->width,pImgA->height),pImgA->depth,pImgA->nChannels); //空圖層 初始化
 IplImage *pImgFilter = cvCreateImage(cvSize(pImgA->width,pImgA->height),IPL_DEPTH_8U,1); //對比範圍圖層 初始化

 
   
pImgC= xxhh(pImgA,pImgB);  //俺們的副程式(input1,input2, *output)
 //xxhh(pImgC,pImgD,*pImgC);


if (i==FristPic){
	pImgCanny=canny(pImgC,pImgBuffer);}

// IplImage *pImgCanny = cvCreateImage(cvSize(pImgB->width,pImgB->height),pImgB->depth,pImgB->nChannels); //產生canny圖
//pImgCanny=canny(pImgB);
CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny,pImgC);
cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl<<endl;// 產生消失點

RLpoint[0][0]=VanishingPoint.x;RLpoint[0][1]=VanishingPoint.y;

if(i!=FristPic) pImgCanny=canny(pImgC,pImgBuffer); //之後再做

//cvLine(pImgCanny,VanishingPoint,VanishingPoint,CV_RGB(225,225,225),20,4); //劃出消失點


		
 cout<< "Video-"<<i-1<<"and Video-"<< i << "print out C" << endl;

 //================
 pImgC=drawline(pImgC,VanishingPoint.x,VanishingPoint.y); //drawline (輸入圖片,消失點X,消失點Y)//劃出基本點

 //==============

 //====青黑
 /*for(int h=0;h<pImgFilter->height;h++) 
		for(int w = 0;w< pImgFilter->widthStep ;w++){
			pImgFilter->imageData[h*pImgFilter->widthStep+w]=0;
			pImgFilter->imageData[h*pImgFilter->widthStep+w+1]=0;
			pImgFilter->imageData[h*pImgFilter->widthStep+w+2]=0;}
*/
	cvCopy(pImgBlack ,pImgFilter); //img1 copy to imgout

 //====
 for(int fs=0;fs<100;fs++)
 {
	 if(RLpoint[fs][0]!=0 && RLpoint[fs][1]!=0){
		 if(RLpoint[fs][0]>=VanishingPoint.x){ //在右邊的點
			 for(int i=RLpoint[fs][0]-20;i<RLpoint[fs][0]+(pImgFilter->widthStep-RLpoint[fs][0]);i++) //右邊擴展 i=RLpoint[fs][0];i<RLpoint[fs][0]+1000 左邊擴展 i=RLpoint[fs][0]-1000;i<RLpoint[fs][0]
		for(int j=RLpoint[fs][1]+30;j>RLpoint[fs][1]-30;j--)
			pImgFilter->imageData[j*pImgFilter->width+i]=255;}

		  if(RLpoint[fs][0]<=VanishingPoint.x){ //在左邊的點
	 for(int i=0;i<RLpoint[fs][0]+20;i++) //右邊擴展 i=RLpoint[fs][0];i<RLpoint[fs][0]+1000 左邊擴展 i=RLpoint[fs][0]-1000;i<RLpoint[fs][0]
		for(int j=RLpoint[fs][1]+30;j>RLpoint[fs][1]-30;j--)
			pImgFilter->imageData[j*pImgFilter->width+i]=255;}
		 }
 }
 
 cvAnd(pImgFilter,pImgCanny,pImgCanny);

 cvLine(pImgCanny,VanishingPoint,VanishingPoint,CV_RGB(225,225,225),20,4); //劃出消失點

//sprintf(maskout, "C:\\Users\\User\\Desktop\\LLSample\\Mask Test\\Mask\\MaskoutCannyFilter\\Video-%d.jpg",i);
//cvSaveImage(maskout,pImgCanny);

   
   cv::Mat cvMatImg(pImgCanny, 0);imshow( "CV Lab", cvMatImg );
 

   cvReleaseImage(&pImgA);cvReleaseImage(&pImgB);cvReleaseImage(&pImgC);//cvReleaseImage(&pImgD); //釋放記憶體
   cvReleaseImage(&pImgColor);cvReleaseImage(&pImgFilter);
  //if(i==400) waitKey(0);
  waitKey(1);
}
 cvReleaseImage(&pImgBlack);cvReleaseImage(&pImgCanny);cvReleaseImage(&pImgBuffer);
 waitKey(0);
   return 0;
}
//============輸入原圖 輸出 canny圖=======
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo)  //canny(輸入圖片,緩衝圖層)
{
	
	IplImage *Smo_pic =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //初始化
	//IplImage *dst_DThrSmo =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //初始化
	//cvCopy(img1 ,Smo_pic); //img1 copy to imgout
	//cvCopy(img1 ,dst_DThrSmo); //img1 copy to imgout

	cvSmooth(img1, Smo_pic, CV_BLUR);               //濾波
	cvCanny(Smo_pic, dst_DThrSmo, 30, 200, 3);				// 邊緣檢測30, 200, 3

	 ////霍夫線轉換
		//CvMemStorage* storage_DThrSmo = cvCreateMemStorage(0);
		//CvSeq* lines_DThrSmo = cvHoughLines2(dst_DThrSmo, storage_DThrSmo, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 59, 10);
		////輸入,儲存,變換方法,距離精度,角度精度,臨界值,最小長度,
		 
    cout << "test canny" << endl;
    img1= dst_DThrSmo;
    //cvReleaseImage(&dst_DThrSmo);
	cvReleaseImage(&Smo_pic);
	
	return img1;
	


}

//==================================================
IplImage* drawline(IplImage *pImgC,int centerX,int centerY) //drawline (輸入圖片,消失點X,消失點Y)
{
	 //int centerX =100; //消失點的中縣X
	 int Arrpointindex=0;
	 int RGBavg ; //顏色精確度越大越白
	 int RGBdiff=40; //中線與周圍偵測的顏色差異 (越小越精細)
	 int cRGB;

	 //==========
	
	 //==========

	 	
	//IplImage *Keep_pic =cvCreateImage(cvSize(pImgC->width,pImgC->height),pImgC->depth,pImgC->nChannels); //初始化
	//cvCopy(pImgC ,Keep_pic); //img1 copy to imgout

	int check=0,check2=0;
	 CvPoint v1,v2;
 CvPoint r1,r2;
 CvPoint L1,L2;
 v1=cvPoint(centerX,centerY);  //Y數字小的在上面
 v2=cvPoint(centerX,pImgC->height-100);


 int rindex=0 , lindex=0;

 for(int index = v1.y;index<v2.y;index+=10){  //620-800
 //===========取上面座標點
 int h=index; //
   for(int w = centerX;w< pImgC->widthStep ;w++) //右上座標
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];

		if (w==centerX){cRGB=(sB+sR+sG)/3;} //紀錄該次中線的RGB平均
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check ==0){  //if((sB+sR+sG)/3>RGBavg && check ==0){
			r1=cvPoint(w,h);
			check++;}
		}
   }
   for(int w = centerX-20;w >0 ;w--) //左上座標
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];
		if (w==centerX-20){cRGB=(sB+sR+sG)/3;} //紀錄該次中線的RGB平均
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check2 ==0){  //if((sB+sR+sG)/3>RGBavg && check2 ==0){
			L1=cvPoint(w,h);
			check2++;}
		}
   }
		//=============取下面座標點
     h=index+3;
		for(int w = centerX;w< pImgC->widthStep ;w++) //右下座標
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];
		if (w==centerX){cRGB=(sB+sR+sG)/3;} //紀錄該次中線的RGB平均
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check ==1){  //if((sB+sR+sG)/3>RGBavg && check ==1){
			r2=cvPoint(w,h);
			check++;}
		}
	}
		for(int w = centerX-20;w>0 ;w--) //左下座標
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];
		if (w==centerX-20){cRGB=(sB+sR+sG)/3;} //紀錄該次中線的RGB平均
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check2 ==1){  //if((sB+sR+sG)/3>RGBavg && check2 ==1){
			L2=cvPoint(w,h);
			check2++;}
		}
	}
		//算線段長加入判段 計算方法: 根號[ (x座標-x座標)^2 + (y座標-y座標)^2 ]
		double linelong=0;

		//
		if(check==2 ){
			linelong= sqrt (pow(double(r2.x-r1.x),2) + pow(double(r2.y-r1.y),2));
			double m = abs( (double)(r1.y - r2.y) / (r1.x - r2.x) ) -  0.0 ;//取得斜率-0 的差距
	        //if(m > 0.5)  //過濾太接近水平
			if(linelong<50)
			{
            
            cvLine( pImgC, r1, r2, CV_RGB(255,255,255), 7);
			CvPoint RightAvg = (cvPoint)(((r1.x+r2.x)/2),((r1.y+r2.y)/2)); //取中間點
			
			 RLpoint[Arrpointindex][0]=RightAvg.x; //把XY軸存進陣列
			 RLpoint[Arrpointindex][1]=RightAvg.y;
			 Arrpointindex++;

			//Keep_pic=KeepLine(r1.x,r1.y,r2.x,r2.y,Keep_pic);//=====劃出延伸線=====
			
			//rindex++;
			}

			check=0;
		}

		//linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));
		if(check2==2){
			linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));
			double m = abs( (double)(L1.y - L2.y) / (L1.x - L2.x) ) -  0.0 ;//取得斜率-0 的差距
	        //if(m > 0.5)  //過濾太接近水平
			if(linelong<50){
			cvLine( pImgC, L1, L2, CV_RGB(255,255,255), 7);
			CvPoint LeftAvg = (cvPoint)(((L1.x+L2.x)/2),((L1.y+L2.y)/2)); //取中間點

			 RLpoint[Arrpointindex][0]=LeftAvg.x; //把XY軸存進陣列
			 RLpoint[Arrpointindex][1]=LeftAvg.y;
			 Arrpointindex++;

			//Keep_pic=KeepLine(L1.x,L1.y,L2.x,L2.y,Keep_pic);//=====劃出延伸線=====
			
			//lindex++;
			}

			check2=0;
		}

		//cvReleaseImage(&pImgOut);
		//cvReleaseImage(&pImgC);
		//cvReleaseImage(&Keep_pic);
 }
  //cvLine( pImgC, v1, v2, CV_RGB(255,255,255), 2);
//cout << "測試用" << Rxy[0][0] << endl;
 // pImgC=Keep_pic; //延長線要開要做

return pImgC;


}
//=========左右取範圍(矩形畫線)============

//==========畫出延長的線段================
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgC)
{
	int linelong= sqrt (pow(double(bx-ax),2) + pow(double(by-ay),2));

	double m = abs( (double)(ay - by) / (ax - bx) ) -  0.0 ;//取得斜率-0 的差距
	if(m > 0.5)  //過濾太接近水平
	if(linelong<50  ){

		CvPoint v1,v2;
		int distance=500;
		int dab=sqrt((double)(ax-bx)*(ax-bx)+(double)(ay-by)*(ay-by));
		int cx=distance*(ax-bx)/dab+bx;  //(ax-bx)為向前延伸 (bx-ax)為向後
		int cy=distance*(ay-by)/dab+by;  //(ay-by)為向前延伸 (by-ay)為向後
		//cout<<"("<<cx<<", "<<cy<<")"<<endl;
	
		v1=cvPoint(cx,cy);  //Y數字小的在上面
		v2=cvPoint(ax,ay);  //Y數字小的在上面
		//cvLine( pImgC, v1,v2, CV_RGB(255,0,0), 3);
		cvLine( pImgC, v1,v2, CV_RGB(255,255,255), 3);
	}
	return pImgC;
}

//=========圖形合併(網狀)===============
IplImage* xxhh(const IplImage *img1,const IplImage *img2)
{
	 IplImage *imgout =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //初始化
	 cvCopy(img1 ,imgout); //img1 copy to imgout


	 int startX = 1;int startY = 1; //start index
 for(int h = 0 ;h< img2->height ;h++) //高度跳躍+=2
 {
   for(int w = 0;w< img2->widthStep ;w++) //寬度跳躍(BGR)+=4
    {
		unsigned  int sB=  img2->imageData[h*img2->widthStep+w];
		unsigned  int sR=  img2->imageData[h*img2->widthStep+w+1];
		unsigned  int sG=  img2->imageData[h*img2->widthStep+w+2];
		if((sB+sR+sG)/3>80)
		{
	  imgout->imageData[h*imgout->widthStep+w]=img2->imageData[h*img2->widthStep+w];
      imgout->imageData[h*imgout->widthStep+w+1]=img2->imageData[h*img2->widthStep+w+1];
      imgout->imageData[h*imgout->widthStep+w+2]=img2->imageData[h*img2->widthStep+w+2];
		}
	  
     }
   
 }
  
 //dts=*imgout;
 return imgout;
  //cvReleaseImage(&imgout);//釋放記憶體
}
 


