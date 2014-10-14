#include "StdAfx.h" //"By xxhh"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "math.h"
#include <vector>
#include "Vanishing Point.h"
#include "Find the best lines.h"
#include "Canny Line.h"

#define CPIndex 5

using namespace cv;
using namespace std; 

IplImage* ClusterLine(int ax,int ay,int bx,int by, IplImage *pImgDis,int centerX,int centerY,int flag,int CLrun);


int RLpoint[100][2]={0};
int MaskRL[200][2]={0};
int CPround;
int CheckXY[5][2]={0};   //排序XY
int AddressXY[5][2]={0}; //基礎XY

int Arrpointindex;

bool flag = false; //偏移是否成立
float avgR,avgL;
int newfilter_rx,newfilter_lx;

IplImage *pImgFilter =NULL;
IplImage *pImgCanny = NULL; //產生canny圖
IplImage *pImgBuffer = NULL;
IplImage *pImgHouf = NULL;
IplImage *pImgA=NULL;
IplImage *pImgB=NULL;
IplImage *pImgC=NULL;
IplImage *pImgGrayC=NULL;


//====================================
//===============初始化===============
void Mask_Init(char FileName[200])
{
	//char FileName[200];
	//sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",1);    //★日間高速
	IplImage *src1 = cvLoadImage(FileName, 0); //讀進原圖
	CvSize pImgA_size; //新的pic大小

	pImgA_size.width = src1->width* (640.0 / src1->width); //重設pImgA大小
	pImgA_size.height = src1->height* (480.0 / src1->height);
	pImgA = cvCreateImage(pImgA_size, src1->depth, src1->nChannels);
	pImgB = cvCreateImage(pImgA_size, src1->depth, src1->nChannels); //創立目標影像B

	cvResize(src1, pImgA, CV_INTER_LINEAR);  //改變大小


	pImgFilter = cvCreateImage(cvSize(640,480), 8,1);
	pImgCanny = cvCreateImage(cvSize(640,480),8,1); //產生canny圖
	pImgBuffer =cvCreateImage(cvSize(640,480), 8,1);
	pImgGrayC = cvCreateImage(cvSize(640,480), 8,1);
	pImgC = cvCreateImage(cvSize(640,480), 8,1); //空圖層 初始化
	pImgHouf = cvCreateImage(cvSize(640,480), 8,1);

	 CPround=0; //最剛開始初始化消失點紀錄
	 // Create Windows
	 cvReleaseImage(&src1); //fix leak
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
			//cout << CheckXY[0][0] << "  "<< CheckXY[1][0] << "  "<< CheckXY[2][0] << "  "<< CheckXY[3][0] << "  "<< CheckXY[4][0]  <<  endl;
			//cout << AddressXY[0][0] << "  "<< AddressXY[1][0] << "  "<< AddressXY[2][0] << "  "<< AddressXY[3][0] << "  "<< AddressXY[4][0]  <<  endl;
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
	//cout << AddressXY[0][0] << "  "<< AddressXY[1][0] << "  "<< AddressXY[2][0] << "  "<< AddressXY[3][0] << "  "<< AddressXY[4][0]  <<  endl;
}
//=====================================
//==============ImgFilter 創造=========
void Filter_Init(int VPx,int VPy){  // (傳入消失點X)
	 
	//====青黑
	//pImgFilter = cvCreateImage(cvSize(640,480), 8,1);
	//cvSet(pImgFilter,cvScalar(255,255,255));
	
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

	////--------新遮罩三角畫法--------
	//CvPoint ** PointArray1 = new CvPoint*[1];  
	//int arr[1];  
	//arr[0] = 3;  
	//PointArray1[0] = new CvPoint[3];  
	//PointArray1[0][0]=cvPoint(VPx,VPy); //
 //   PointArray1[0][1]=cvPoint(newfilter_rx,450);
 //   PointArray1[0][2]=cvPoint(newfilter_lx,450);
	//cvFillPoly(pImgFilter,PointArray1,arr,1,CV_RGB(0,0,0));  

 cvAnd(pImgFilter,pImgCanny,pImgCanny);
 ////=======================================


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
		if((sB+sR+sG)/3>120) //>80
		{
			imgout->imageData[h*imgout->widthStep+w]=img2->imageData[h*img2->widthStep+w];
			imgout->imageData[h*imgout->widthStep+w+1]=img2->imageData[h*img2->widthStep+w+1];
			imgout->imageData[h*imgout->widthStep+w+2]=img2->imageData[h*img2->widthStep+w+2];
		}
	  
     }
   
 }
 //cvEqualizeHist( imgout, imgout );//直方圖均化

 return imgout;
  //cvReleaseImage(&imgout);//釋放記憶體
}
//========================================
//============輸入原圖 輸出 canny圖=======
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo)  //canny(輸入圖片,緩衝圖層(空的))
{
	
	IplImage *Smo_pic =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //初始化

	
	cvSmooth(img1,img1,CV_GAUSSIAN,3,3,0,0); //GAUSSIAN濾波

	cvCanny(img1, dst_DThrSmo, 40,120, 3);				// 邊緣檢測30, 200, 3
		 
    cout << "test canny" << endl;
	img1 = dst_DThrSmo; 
    //cvReleaseImage(&dst_DThrSmo);
	cvReleaseImage(&Smo_pic);
	
	return img1;
	
}
//==================================================
//===============對比點線(暫時)=====================
IplImage* drawline(IplImage *pImgDis,int centerX,int centerY) //drawline (輸入圖片,消失點X,消失點Y)
{
	 //int centerX =100; //消失點的中縣X
	 Arrpointindex=0;
	 int RGBavg ; //顏色精確度越大越白
	 int RGBdiff=40; //中線與周圍偵測的顏色差異 (越小越精細)
	 int cRGB;
	 int LBef=5,RBef=5; //記錄前一個對比點與消失點X的距離

	 int FixMask_Lu[5],FixMask_Ld[5],FixMask_Ru[5],FixMask_Rd[5]; //修正中間雜訊過濾
	 int FMflag = 0 ;//修正中間雜訊過濾

	 int CLflag = 0 ;//紀錄是否到結尾了，觸發三角遮罩運算
	 avgR=0.0,avgL=0.0; //平均斜率歸零
	
	 	
	//IplImage *Keep_pic =cvCreateImage(cvSize(pImgC->width,pImgC->height),pImgC->depth,pImgC->nChannels); //初始化
	//cvCopy(pImgC ,Keep_pic); //img1 copy to imgout

	int check=0,check2=0;
	 CvPoint v1,v2;
 CvPoint r1,r2;
 CvPoint L1,L2;
 v1=cvPoint(centerX,centerY);  //Y數字小的在上面
 v2=cvPoint(centerX,pImgDis->height-20);


 int rindex=0 , lindex=0;

 for(int index = v1.y;index<v2.y;index+=10){  //620-800

	 
 //===========取上面座標點
 int h=index; //
 
   for(int w = centerX;w< pImgDis->widthStep ;w++) //右上座標
    {
		int sB=  pImgDis->imageData[h*pImgDis->widthStep+w];  //unsigned
		int sR=  pImgDis->imageData[h*pImgDis->widthStep+w+1];
		int sG=  pImgDis->imageData[h*pImgDis->widthStep+w+2];

		if (w==centerX){
			cRGB=(sB+sR+sG)/3;
			cout <<"cRGB==="<< sB<<","<<sR <<","<<sG << endl;
			if (FMflag<=3) {FixMask_Ru[FMflag]=cRGB;} //紀錄右上中線前4平均
		} 
		else{   
			//if(FMflag>3 && abs(cRGB-FixMask_Ru[4])>90 ) continue;
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check ==0){  //if((sB+sR+sG)/3>RGBavg && check ==0){
			r1=cvPoint(w,h);
			check++;}
		}
   }
   for(int w = centerX-20;w >0 ;w--) //左上座標
    {
		int sB=  pImgDis->imageData[h*pImgDis->widthStep+w];
		int sR=  pImgDis->imageData[h*pImgDis->widthStep+w+1];
		int sG=  pImgDis->imageData[h*pImgDis->widthStep+w+2];
		if (w==centerX-20){
			cRGB=(sB+sR+sG)/3;//紀錄該次中線的RGB平均
			if (FMflag<=3) {FixMask_Lu[FMflag]=cRGB;} //紀錄左上中線前4平均
		} 
		else{   
			//if(FMflag>3 && abs(cRGB-FixMask_Lu[4])>90 ) continue;
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check2 ==0){  //if((sB+sR+sG)/3>RGBavg && check2 ==0){
			L1=cvPoint(w,h);
			check2++;}
		}
   }
		//=============取下面座標點
     h=index+3;
		for(int w = centerX;w< pImgDis->widthStep ;w++) //右下座標
    {
		int sB=  pImgDis->imageData[h*pImgDis->widthStep+w];
		int sR=  pImgDis->imageData[h*pImgDis->widthStep+w+1];
		int sG=  pImgDis->imageData[h*pImgDis->widthStep+w+2];
		if (w==centerX){
			cRGB=(sB+sR+sG)/3;//紀錄該次中線的RGB平均	
			if (FMflag<=3) {FixMask_Rd[FMflag]=cRGB;} //紀錄右下中線前4平均
		} 
		else{   
			//if(FMflag>3 && abs(cRGB-FixMask_Rd[4])>90 ) continue;
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check ==1){  //if((sB+sR+sG)/3>RGBavg && check ==1){
			r2=cvPoint(w,h);
			check++;}
		}
	}
		for(int w = centerX-20;w>0 ;w--) //左下座標
    {
		int sB=  pImgDis->imageData[h*pImgDis->widthStep+w];
		int sR=  pImgDis->imageData[h*pImgDis->widthStep+w+1];
		int sG=  pImgDis->imageData[h*pImgDis->widthStep+w+2];
		if (w==centerX-20){
			cRGB=(sB+sR+sG)/3; //紀錄該次中線的RGB平均
			if (FMflag<=3) {FixMask_Ld[FMflag]=cRGB;} //紀錄左下中線前4平均
		}
		else{   
			//if(FMflag>3 && abs(cRGB-FixMask_Ld[4])>90 ) continue;
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
			//double m = abs( (double)(r1.y - r2.y) / (r1.x - r2.x) ) -  0.0 ;//取得斜率-0 的差距
			//if(m > 0.5)  //過濾太接近水平
			if(linelong<100) //linelong<50
			{
            
            cvLine( pImgDis, r1, r2, CV_RGB(255,255,255), 1);
			CvPoint RightAvg = (cvPoint)(((r1.x+r2.x)/2),((r1.y+r2.y)/2)); //取中間點
			
			//if(abs(RightAvg.x-centerX)>100){

			 RLpoint[Arrpointindex][0]=RightAvg.x; //把XY軸存進陣列
			 RLpoint[Arrpointindex][1]=RightAvg.y;
			 Arrpointindex++;
			 //RBef=abs(RightAvg.x-centerX);
			//}
			//pImgC=KeepLine(r1.x,r1.y,r2.x,r2.y,pImgC);//=====劃出延伸線=====
			 pImgDis=ClusterLine(RightAvg.x,RightAvg.y,centerX,centerY,pImgDis,centerX,centerY,0,0);
			
			//rindex++;
			}

			check=0;
		}

		//linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));
		if(check2==2){
			linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));
			//double m = abs( (double)(L1.y - L2.y) / (L1.x - L2.x) ) -  0.0 ;//取得斜率-0 的差距
			//if(m > 0.5)  //過濾太接近水平
			if(linelong<100){ //linelong<50
			cvLine( pImgDis, L1, L2, CV_RGB(255,255,255), 1);

			CvPoint LeftAvg = (cvPoint)(((L1.x+L2.x)/2),((L1.y+L2.y)/2)); //取中間點
			
			//if(abs(centerX-LeftAvg.x)>100){

			 RLpoint[Arrpointindex][0]=LeftAvg.x; //把XY軸存進陣列
			 RLpoint[Arrpointindex][1]=LeftAvg.y;
			 Arrpointindex++; 
			 //LBef=abs(LeftAvg.x-centerX);
			//}
			//pImgC=KeepLine(L1.x,L1.y,L2.x,L2.y,pImgC);//=====劃出延伸線=====
			 pImgDis=ClusterLine(LeftAvg.x,LeftAvg.y,centerX,centerY,pImgDis,centerX,centerY,1,0);
			
			//lindex++;
			}

			check2=0;
		}

		
		

		//=====修正中線標準平均====
		FMflag++;
		if(FMflag==4)
		{
			FixMask_Lu[4]=FixMask_Ld[4]=FixMask_Ru[4]=FixMask_Rd[4]=0; //Init 0
			for(int fi=0;fi<4;fi++)
			{
				FixMask_Lu[4]+=FixMask_Lu[fi];
				FixMask_Ru[4]+=FixMask_Ru[fi];
				FixMask_Ld[4]+=FixMask_Ld[fi];
				FixMask_Rd[4]+=FixMask_Rd[fi];
			}
			FixMask_Lu[4]/=4;
			FixMask_Ru[4]/=4;
			FixMask_Ld[4]/=4;
			FixMask_Rd[4]/=4;
		}


 }
 pImgDis=ClusterLine(0,0,centerX,centerY,pImgDis,centerX,centerY,NULL,1);
  //cvLine( pImgC, v1, v2, CV_RGB(255,255,255), 2);
//cout << "測試用" << Rxy[0][0] << endl;
 // pImgC=Keep_pic; //延長線要開要做

return pImgDis;


}

//=========
IplImage* ClusterLine(int ax,int ay,int bx,int by, IplImage *pImgDis,int centerX,int centerY,int flag,int CLrun)
{
	int y,x;
	float m =  ((float)(ay - by) / (float)(ax - bx)) ;//取得斜率
	//--過濾水平線--
	if(CLrun==0)
	if(abs(m)>0.3){ 
	cout << "M = " << m << endl;
	///直線的斜率為 3 及經過點 (1, 2) 方程：y - 2 = 3(x - 1)  y-cy=m(x-cx)
	///x= (y-cy+mcx)/m
	y=450; //Y為固定軸
	x= (y-centerY+ m*centerX) / m; //計算出坐落於Y軸的X

	MaskRL[Arrpointindex][flag]=x;

	if(flag==0) avgR += m;
	if(flag==1) avgL += m;
	cout<<"avgR="<<avgR<<"  avgL="<<avgL<<endl;
	//cvLine( pImgC, cvPoint(centerX,centerY),cvPoint(x,y), CV_RGB(255,255,255), 1);
	}
	//-----觸發三角遮罩估算-----
	if(CLrun==1){
		
		avgR = avgR / (Arrpointindex/2); //+0.3
		avgL = avgL / (Arrpointindex/2); //-0.3
		y=450; //Y為固定軸
		newfilter_rx= (y-centerY+ avgR*centerX) / avgR; //avgR 計算出坐落於Y軸的X
		cvLine( pImgC, cvPoint(centerX,centerY),cvPoint(newfilter_rx,y), CV_RGB(255,255,255), 3);
		newfilter_lx= (y-centerY+ avgL*centerX) / avgL; //avgL 計算出坐落於Y軸的X
		cvLine( pImgC, cvPoint(centerX,centerY),cvPoint(newfilter_lx,y), CV_RGB(255,255,255), 3);
	}



	return pImgDis;
}


//=========左右取範圍(矩形畫線)============

//==========畫出延長的線段================
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgDis)
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
		cvLine( pImgDis, v1,v2, CV_RGB(255,255,255), 3);
	}
	return pImgDis;
}


 


