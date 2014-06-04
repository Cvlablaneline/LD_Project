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
int CheckXY[5][2]={0};   //�Ƨ�XY
int AddressXY[5][2]={0}; //��¦XY

bool flag = false; //�����O�_����

IplImage *pImgFilter =NULL;
IplImage *pImgCanny = NULL; //����canny��
IplImage *pImgBuffer = NULL;
IplImage *pImgA=NULL;
IplImage *pImgB=NULL;

//====================================
//===============��l��===============
void Mask_Init()
{
	char FileName[200];
	sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",1);    //���鶡���t
	IplImage *src1 = cvLoadImage(FileName, 0); //Ū�i���
	CvSize pImgA_size; //�s��pic�j�p

	pImgA_size.width = src1->width* (1280.0 / src1->width); //���]pImgA�j�p
	pImgA_size.height = src1->height* (720.0 / src1->height);
	pImgA = cvCreateImage(pImgA_size, src1->depth, src1->nChannels);
	pImgB = cvCreateImage(pImgA_size, src1->depth, src1->nChannels); //�Хߥؼмv��B

	cvResize(src1, pImgA, CV_INTER_LINEAR);  //���ܤj�p


	pImgFilter = cvCreateImage(cvSize(1280,720), 8,1);
	pImgCanny = cvCreateImage(cvSize(1280,720),8,1); //����canny��
	pImgBuffer =cvCreateImage(cvSize(1280,720), 8,1);

	 CPround=0; //�̭�}�l��l�Ʈ����I����
}

//====================================
//===========���D�u�����˴�===========

bool Lane_Offset(CvPoint VPoint,int lx,int rx) //�ǤJ (�����I,��X,�kX)
{
	if(lx!=0 || rx!=0){

		float sumLong=abs(rx-lx); //���k���D�`��

		if(abs(VPoint.x-lx)/sumLong<=0.33){
			flag=true;
			return flag; //���u����
		}
		else if(abs(rx-VPoint.x)/sumLong<=0.33){
			flag = true;
			return flag; //���u����
		}
		else{
			flag = false;
			return flag;
		}
	}
	return flag;
}
//====================================
//==============�ˬd�����I���T��======
void Check_VPoint(int &VPx,int &VPy)
{
	int oldx=VPx;int oldy=VPy;
	if (CPround<CPIndex) //�e���i��}�C
	{
		AddressXY[CPround][0] = CheckXY[CPround][0]= VPx; //save X
		AddressXY[CPround][1] = CheckXY[CPround][1]= VPy; //save Y
	}
	else
	{
		for(int i=0;i<CPIndex;i++) //CheckXY�Ƨ�
			for(int j=0;j<CPIndex-1;j++)
			{
				if(CheckXY[j][0]>CheckXY[j+1][0]){ //X �Ƨ�
				int resx=CheckXY[j][0];
				CheckXY[j][0]=CheckXY[j+1][0];
				CheckXY[j+1][0]=resx;
				}
				if(CheckXY[j][1]>CheckXY[j+1][1]){ //Y �Ƨ�
				int resy=CheckXY[j][1];
				CheckXY[j][1]=CheckXY[j+1][1];
				CheckXY[j+1][1]=resy;
				}
			}
			//===================
			cout << CheckXY[0][0] << "  "<< CheckXY[1][0] << "  "<< CheckXY[2][0] << "  "<< CheckXY[3][0] << "  "<< CheckXY[4][0]  <<  endl;
			cout << AddressXY[0][0] << "  "<< AddressXY[1][0] << "  "<< AddressXY[2][0] << "  "<< AddressXY[3][0] << "  "<< AddressXY[4][0]  <<  endl;
			int midx=CheckXY[2][0]; //������X
			int midy=CheckXY[2][1];//������Y

			VPx=midx;
			VPy=midy;
			//if(abs(VPx-midx)>30){  VPx=midx; cout <<endl<< "�ץ�X ������������ "<< midx <<endl; }//�~�t�Ӥj(20) �ץ�
			//if(abs(VPy-midy)>30){  VPy=midy; cout <<endl<< "�ץ�Y ������������ "<<  midy <<endl; }//�~�t�Ӥj(20) �ץ�

			for(int i=1;i<CPIndex;i++) {
				AddressXY[i-1][0]=AddressXY[i][0]; //��¦�h��X
				AddressXY[i-1][1]=AddressXY[i][1]; //��¦�h��Y
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
//==============ImgFilter �гy=========
void Filter_Init(int VPx){  // (�ǤJ�����IX)
	 
	//====�C��
	pImgFilter = cvCreateImage(cvSize(1280,720), 8,1);
	cvSet(pImgFilter,cvScalar(0,0,0));
	//cvCopy(pImgBlack ,pImgFilter); //img1 copy to imgout
	
 //======���;B�n�� Filter================
 for(int fs=0;fs<100;fs++)
 {
	 //if (RLpoint[fs][0]<=0)RLpoint[fs][0]=0;if (RLpoint[fs][1]<=0)RLpoint[fs][1]=0;
	 if(RLpoint[fs][0]!=0 && RLpoint[fs][1]!=0){
		 if(RLpoint[fs][0]>=VPx){ //�b�k�䪺�I
			 for(int i=RLpoint[fs][0]-20;i<RLpoint[fs][0]+(pImgFilter->widthStep-RLpoint[fs][0]);i++) //�k���X�i i=RLpoint[fs][0];i<RLpoint[fs][0]+1000 �����X�i i=RLpoint[fs][0]-1000;i<RLpoint[fs][0]
		for(int j=RLpoint[fs][1]+30;j>RLpoint[fs][1]-30;j--){
			if(j<0)continue;
			pImgFilter->imageData[j*pImgFilter->width+i]=255;
		}}

		  if(RLpoint[fs][0]<=VPx){ //�b���䪺�I
	 for(int i=0;i<RLpoint[fs][0]+20;i++) //�k���X�i i=RLpoint[fs][0];i<RLpoint[fs][0]+1000 �����X�i i=RLpoint[fs][0]-1000;i<RLpoint[fs][0]
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
//==========�v���|��(����)=============== xxhh(��J�v��1,��J�v��2,��X�|��)
IplImage* xxhh( IplImage *img1,IplImage *img2,IplImage *imgout)
{
	 for(int j=0;j<100;j++){ RLpoint[j][0]=0;RLpoint[j][1]=0;}//�}�C��l��

	 int startX = 1;int startY = 1; //start index
 for(int h = 0 ;h< img2->height ;h++) //���׸��D+=2
 {
   for(int w = 0;w< img2->widthStep ;w++) //�e�׸��D(BGR)+=4
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
  //cvReleaseImage(&imgout);//����O����
}
//========================================
//============��J��� ��X canny��=======
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo)  //canny(��J�Ϥ�,�w�Ĺϼh(�Ū�))
{
	
	IplImage *Smo_pic =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //��l��
	//IplImage *dst_DThrSmo =cvCreateImage(cvSize(img1->width,img1->height),img1->depth,img1->nChannels); //��l��
	//cvCopy(img1 ,Smo_pic); //img1 copy to imgout
	//cvCopy(img1 ,dst_DThrSmo); //img1 copy to imgout

	//cvSmooth(img1, Smo_pic, CV_BLUR);               //�o�i
	//cvCanny(Smo_pic, dst_DThrSmo, 30, 200, 3);				// ��t�˴�30, 200, 3
	cvCanny(img1, dst_DThrSmo, 30, 200, 3);				// ��t�˴�30, 200, 3

	 ////�N�ҽu�ഫ
		//CvMemStorage* storage_DThrSmo = cvCreateMemStorage(0);
		//CvSeq* lines_DThrSmo = cvHoughLines2(dst_DThrSmo, storage_DThrSmo, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 59, 10);
		////��J,�x�s,�ܴ���k,�Z�����,���׺��,�{�ɭ�,�̤p����,
		 
    cout << "test canny" << endl;
	img1 = dst_DThrSmo;
    //cvReleaseImage(&dst_DThrSmo);
	cvReleaseImage(&Smo_pic);
	
	return img1;
	
}
//==================================================
//===============����I�u(�Ȯ�)=====================
IplImage* drawline(IplImage *pImgC,int centerX,int centerY) //drawline (��J�Ϥ�,�����IX,�����IY)
{
	 //int centerX =100; //�����I������X
	 int Arrpointindex=0;
	 int RGBavg ; //�C���T�׶V�j�V��
	 int RGBdiff=40; //���u�P�P�򰻴����C��t�� (�V�p�V���)
	 int cRGB;
	 int LBef=5,RBef=5; //�O���e�@�ӹ���I�P�����IX���Z��

	 	
	//IplImage *Keep_pic =cvCreateImage(cvSize(pImgC->width,pImgC->height),pImgC->depth,pImgC->nChannels); //��l��
	//cvCopy(pImgC ,Keep_pic); //img1 copy to imgout

	int check=0,check2=0;
	 CvPoint v1,v2;
 CvPoint r1,r2;
 CvPoint L1,L2;
 v1=cvPoint(centerX,centerY);  //Y�Ʀr�p���b�W��
 v2=cvPoint(centerX,pImgC->height-100);


 int rindex=0 , lindex=0;

 for(int index = v1.y;index<v2.y;index+=10){  //620-800
 //===========���W���y���I
 int h=index; //
   for(int w = centerX;w< pImgC->widthStep ;w++) //�k�W�y��
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];

		if (w==centerX){cRGB=(sB+sR+sG)/3;} //�����Ӧ����u��RGB����
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check ==0){  //if((sB+sR+sG)/3>RGBavg && check ==0){
			r1=cvPoint(w,h);
			check++;}
		}
   }
   for(int w = centerX-20;w >0 ;w--) //���W�y��
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];
		if (w==centerX-20){cRGB=(sB+sR+sG)/3;} //�����Ӧ����u��RGB����
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check2 ==0){  //if((sB+sR+sG)/3>RGBavg && check2 ==0){
			L1=cvPoint(w,h);
			check2++;}
		}
   }
		//=============���U���y���I
     h=index+3;
		for(int w = centerX;w< pImgC->widthStep ;w++) //�k�U�y��
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];
		if (w==centerX){cRGB=(sB+sR+sG)/3;} //�����Ӧ����u��RGB����
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check ==1){  //if((sB+sR+sG)/3>RGBavg && check ==1){
			r2=cvPoint(w,h);
			check++;}
		}
	}
		for(int w = centerX-20;w>0 ;w--) //���U�y��
    {
		unsigned  int sB=  pImgC->imageData[h*pImgC->widthStep+w];
		unsigned  int sR=  pImgC->imageData[h*pImgC->widthStep+w+1];
		unsigned  int sG=  pImgC->imageData[h*pImgC->widthStep+w+2];
		if (w==centerX-20){cRGB=(sB+sR+sG)/3;} //�����Ӧ����u��RGB����
		
		else{   
			RGBavg=abs(cRGB-(int)((sB+sR+sG)/3));
			if(RGBavg>RGBdiff && check2 ==1){  //if((sB+sR+sG)/3>RGBavg && check2 ==1){
			L2=cvPoint(w,h);
			check2++;}
		}
	}
		//��u�q���[�J�P�q �p���k: �ڸ�[ (x�y��-x�y��)^2 + (y�y��-y�y��)^2 ]
		double linelong=0;

		//
		if(check==2 ){
			linelong= sqrt (pow(double(r2.x-r1.x),2) + pow(double(r2.y-r1.y),2));
			
			if(linelong<50)
			{
            
            cvLine( pImgC, r1, r2, CV_RGB(255,255,255), 7);
			CvPoint RightAvg = (cvPoint)(((r1.x+r2.x)/2),((r1.y+r2.y)/2)); //�������I
			
			//if(abs(RightAvg.x-centerX)>100){

			 RLpoint[Arrpointindex][0]=RightAvg.x; //��XY�b�s�i�}�C
			 RLpoint[Arrpointindex][1]=RightAvg.y;
			 Arrpointindex++;
			 //RBef=abs(RightAvg.x-centerX);
			//}
			//Keep_pic=KeepLine(r1.x,r1.y,r2.x,r2.y,Keep_pic);//=====���X�����u=====
			
			//rindex++;
			}

			check=0;
		}

		//linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));
		if(check2==2){
			linelong= sqrt (pow(double(L2.x-L1.x),2) + pow(double(L2.y-L1.y),2));

			if(linelong<50){
			cvLine( pImgC, L1, L2, CV_RGB(255,255,255), 7);
			CvPoint LeftAvg = (cvPoint)(((L1.x+L2.x)/2),((L1.y+L2.y)/2)); //�������I

			//if(abs(centerX-LeftAvg.x)>100){

			 RLpoint[Arrpointindex][0]=LeftAvg.x; //��XY�b�s�i�}�C
			 RLpoint[Arrpointindex][1]=LeftAvg.y;
			 Arrpointindex++;
			 //LBef=abs(LeftAvg.x-centerX);
			//}
			//Keep_pic=KeepLine(L1.x,L1.y,L2.x,L2.y,Keep_pic);//=====���X�����u=====
			
			//lindex++;
			}

			check2=0;
		}

		//cvReleaseImage(&pImgOut);
		//cvReleaseImage(&pImgC);
		//cvReleaseImage(&Keep_pic);
 }
  //cvLine( pImgC, v1, v2, CV_RGB(255,255,255), 2);
//cout << "���ե�" << Rxy[0][0] << endl;
 // pImgC=Keep_pic; //�����u�n�}�n��

return pImgC;


}
//=========���k���d��(�x�εe�u)============

//==========�e�X�������u�q================
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgC)
{
	int linelong= sqrt (pow(double(bx-ax),2) + pow(double(by-ay),2));

	double m = abs( (double)(ay - by) / (ax - bx) ) -  0.0 ;//���o�ײv-0 ���t�Z
	if(m > 0.5)  //�L�o�ӱ������
	if(linelong<50  ){

		CvPoint v1,v2;
		int distance=500;
		int dab=sqrt((double)(ax-bx)*(ax-bx)+(double)(ay-by)*(ay-by));
		int cx=distance*(ax-bx)/dab+bx;  //(ax-bx)���V�e���� (bx-ax)���V��
		int cy=distance*(ay-by)/dab+by;  //(ay-by)���V�e���� (by-ay)���V��
		//cout<<"("<<cx<<", "<<cy<<")"<<endl;
	
		v1=cvPoint(cx,cy);  //Y�Ʀr�p���b�W��
		v2=cvPoint(ax,ay);  //Y�Ʀr�p���b�W��
		//cvLine( pImgC, v1,v2, CV_RGB(255,0,0), 3);
		cvLine( pImgC, v1,v2, CV_RGB(255,255,255), 3);
	}
	return pImgC;
}


 


