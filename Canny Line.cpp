#include "StdAfx.h" //"By xxhh"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "math.h"
#include <vector>
#include "Vanishing Point.h"

#define CPIndex 5

using namespace cv;
using namespace std; 


int RLpoint[100][2]={0};
int CPround;
int CheckXY[5][2]={0};   //排序XY
int AddressXY[5][2]={0}; //基礎XY

bool flag = false; //偏移是否成立

IplImage *pImgFilter =NULL;
IplImage *pImgCanny = NULL; //產生canny圖
IplImage *pImgBuffer = NULL;
IplImage *pImgA=NULL;
IplImage *pImgB=NULL;

//====================================
//===============初始化===============
void Mask_Init()
{
	char FileName[200];
	sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",1);    //★日間高速
	IplImage *src1 = cvLoadImage(FileName, 0); //讀進原圖
	CvSize pImgA_size; //新的pic大小

	pImgA_size.width = src1->width* (1280.0 / src1->width); //重設pImgA大小
	pImgA_size.height = src1->height* (720.0 / src1->height);
	pImgA = cvCreateImage(pImgA_size, src1->depth, src1->nChannels);
	pImgB = cvCreateImage(pImgA_size, src1->depth, src1->nChannels); //創立目標影像B

	cvResize(src1, pImgA, CV_INTER_LINEAR);  //改變大小


	pImgFilter = cvCreateImage(cvSize(1280,720), 8,1);
	pImgCanny = cvCreateImage(cvSize(1280,720),8,1); //產生canny圖
	pImgBuffer =cvCreateImage(cvSize(1280,720), 8,1);

	 CPround=0; //最剛開始初始化消失點紀錄
}

//====================================
//===========車道線偏移檢測===========

bool Lane_Offset(CvPoint VPoint,int lx,int rx) //傳入 (消失點,左X,右X)
{
	if(lx!=0 || rx!=0){

		float sumLong=abs(rx-lx); //左右車道總長

		if(abs(VPoint.x-lx)/sumLong<=0.33){
			flag=true;
			return flag; //左線偏移
		}
		else if(abs(rx-VPoint.x)/sumLong<=0.33){
			flag = true;
			return flag; //左線偏移
		}
		else{
			flag = false;
			return flag;
		}
	}
	return flag;
}
//====================================
//==============檢查消失點正確性======
void Check_VPoint(int &VPx,int &VPy)
{
	int oldx=VPx;int oldy=VPy;
	if (CPround<CPIndex) //前五張丟陣列
	{
		AddressXY[CPround][0] = CheckXY[CPround][0]= VPx; //save X
		AddressXY[CPround][1] = CheckXY[CPround][1]= VPy; //save Y
	}
	else
	{
		for(int i=0;i<CPIndex;i++) //CheckXY排序
			for(int j=0;j<CPIndex-1;j++)
			{
				if(CheckXY[j][0]>CheckXY[j+1][0]){ //X 排序
				int resx=CheckXY[j][0];
				CheckXY[j][0]=CheckXY[j+1][0];
				CheckXY[j+1][0]=resx;
				}
				if(CheckXY[j][1]>CheckXY[j+1][1]){ //Y 排序
				int resy=CheckXY[j][1];
				CheckXY[j][1]=CheckXY[j+1][1];
				CheckXY[j+1][1]=resy;
				}
			}
			//===================
			cout << CheckXY[0][0] << "  "<< CheckXY[1][0] << "  "<< CheckXY[2][0] << "  "<< CheckXY[3][0] << "  "<< CheckXY[4][0]  <<  endl;
			cout << AddressXY[0][0] << "  "<< AddressXY[1][0] << "  "<< AddressXY[2][0] << "  "<< AddressXY[3][0] << "  "<< AddressXY[4][0]  <<  endl;
			int midx=CheckXY[2][0]; //中間的X
			int midy=CheckXY[2][1];//中間的Y

			VPx=midx;
			VPy=midy;
			//if(abs(VPx-midx)>30){  VPx=midx; cout <<endl<< "修正X ★★★★★★ "<< midx <<endl; }//誤差太大(20) 修正
			//if(abs(VPy-midy)>30){  VPy=midy; cout <<endl<< "修正Y ☆☆☆☆☆☆ "<<  midy <<endl; }//誤差太大(20) 修正

			for(int i=1;i<CPIndex;i++) {
				AddressXY[i-1][0]=AddressXY[i][0]; //基礎搬移X
				AddressXY[i-1][1]=AddressXY[i][1]; //基礎搬移Y
				CheckXY[i-1][0]=AddressXY[i-1][0];
				CheckXY[i-1][1]=AddressXY[i-1][1];
			}
			AddressXY[CPIndex-1][0] = CheckXY[CPIndex-1][0]= oldx; //save X
			AddressXY[CPIndex-1][1] = CheckXY[CPIndex-1][1]= oldy; //save Y

	}

	if(CPround <= CPIndex) 
		CPround++;
	//if(CPround ==15) CPround=0;
	cout << AddressXY[0][0] << "  "<< AddressXY[1][0] << "  "<< AddressXY[2][0] << "  "<< AddressXY[3][0] << "  "<< AddressXY[4][0]  <<  endl;
}
//=====================================
//==============ImgFilter 創造=========
void Filter_Init(int VPx){  // (傳入消失點X)
	 
	//====青黑
	pImgFilter = cvCreateImage(cvSize(1280,720), 8,1);
	cvSet(pImgFilter,cvScalar(0,0,0));
	//cvCopy(pImgBlack ,pImgFilter); //img1 copy to imgout
	
 //======產生遮罩圖 Filter================
 for(int fs=0;fs<100;fs++)
 {
	 //if (RLpoint[fs][0]<=0)RLpoint[fs][0]=0;if (RLpoint[fs][1]<=0)RLpoint[fs][1]=0;
	 if(RLpoint[fs][0]!=0 && RLpoint[fs][1]!=0){
		 if(RLpoint[fs][0]>=VPx){ //在右邊的點
			 for(int i=RLpoint[fs][0]-20;i<RLpoint[fs][0]+(pImgFilter->widthStep-RLpoint[fs][0]);i++) //右邊擴展 i=RLpoint[fs][0];i<RLpoint[fs][0]+1000 左邊擴展 i=RLpoint[fs][0]-1000;i<RLpoint[fs][0]
		for(int j=RLpoint[fs][1]+30;j>RLpoint[fs][1]-30;j--){
			if(j<0)continue;
			pImgFilter->imageData[j*pImgFilter->width+i]=255;
		}}

		  if(RLpoint[fs][0]<=VPx){ //在左邊的點
	 for(int i=0;i<RLpoint[fs][0]+20;i++) //右邊擴展 i=RLpoint[fs][0];i<RLpoint[fs][0]+1000 左邊擴展 i=RLpoint[fs][0]-1000;i<RLpoint[fs][0]
		for(int j=RLpoint[fs][1]+30;j>RLpoint[fs][1]-30;j--){
			//if(j>pImgFilter->height)continue;
			pImgFilter->imageData[j*pImgFilter->width+i]=255;}
		  }
		 }
 }
 

 cvAnd(pImgFilter,pImgCanny,pImgCanny);
 //=======================================


}


//=======================================
//==========影像疊圖(網狀)=============== xxhh(輸入影像1,輸入影像2,輸出疊圖)
IplImage* xxhh( IplImage *img1,IplImage *img2,IplImage *imgout)
{
	 for(int j=0;j<100;j++){ RLpoint[j][0]=0;RLpoint[j][1]=0;}//陣列初始化

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
  
 return imgout;
  //cvReleaseImage(&imgout);//釋放記憶體
}
//========================================
//============輸入原圖 輸出 canny圖=======
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo)  //canny(輸入圖片,緩衝圖層(空的))
{
	
	IplImage *Smo_pic =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //初始化
	//IplImage *dst_DThrSmo =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //初始化
	//cvCopy(img1 ,Smo_pic); //img1 copy to imgout
	//cvCopy(img1 ,dst_DThrSmo); //img1 copy to imgout

	//cvSmooth(img1, Smo_pic, CV_BLUR);               //濾波
	//cvCanny(Smo_pic, dst_DThrSmo, 30, 200, 3);				// 邊緣檢測30, 200, 3
	cvCanny(img1, dst_DThrSmo, 30, 200, 3);				// 邊緣檢測30, 200, 3

	 ////霍夫線轉換
		//CvMemStorage* storage_DThrSmo = cvCreateMemStorage(0);
		//CvSeq* lines_DThrSmo = cvHoughLines2(dst_DThrSmo, storage_DThrSmo, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 59, 10);
		////輸入,儲存,變換方法,距離精度,角度精度,臨界值,最小長度,
		 
    cout << "test canny" << endl;
	img1 = dst_DThrSmo;
    //cvReleaseImage(&dst_DThrSmo);
	cvReleaseImage(&Smo_pic);
	
	return img1;
	
}
//==================================================
//===============對比點線(暫時)=====================
IplImage* drawline(IplImage *pImgC,int centerX,int centerY) //drawline (輸入圖片,消失點X,消失點Y)
{
	 //int centerX =100; //消失點的中縣X
	 int Arrpointindex=0;
	 int RGBavg ; //顏色精確度越大越白
	 int RGBdiff=40; //中線與周圍偵測的顏色差異 (越小越精細)
	 int cRGB;
	 int LBef=5,RBef=5; //記錄前一個對比點與消失點X的距離

	 	
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
			
			if(linelong<50)
			{
            
            cvLine( pImgC, r1, r2, CV_RGB(255,255,255), 7);
			CvPoint RightAvg = (cvPoint)(((r1.x+r2.x)/2),((r1.y+r2.y)/2)); //取中間點
			
			//if(abs(RightAvg.x-centerX)>100){

			 RLpoint[Arrpointindex][0]=RightAvg.x; //把XY軸存進陣列
			 RLpoint[Arrpointindex][1]=RightAvg.y;
			 Arrpointindex++;
			 //RBef=abs(RightAvg.x-centerX);
			//}
			//Keep_pic=KeepLine(r1.x,r1.y,r2.x,r2.y,Keep_pic);//=====劃出延伸線=====
			
			//rindex++;
			}

			check=0;
		}

		//linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));
		if(check2==2){
			linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));

			if(linelong<50){
			cvLine( pImgC, L1, L2, CV_RGB(255,255,255), 7);
			CvPoint LeftAvg = (cvPoint)(((L1.x+L2.x)/2),((L1.y+L2.y)/2)); //取中間點

			//if(abs(centerX-LeftAvg.x)>100){

			 RLpoint[Arrpointindex][0]=LeftAvg.x; //把XY軸存進陣列
			 RLpoint[Arrpointindex][1]=LeftAvg.y;
			 Arrpointindex++;
			 //LBef=abs(LeftAvg.x-centerX);
			//}
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


 


