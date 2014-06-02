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

//�Ϲ��ɮצ�m
//#define	SEQ_NAME	"/Users/chienfu/Desktop/���D�u����/demo/Video- (%d).jpg"
//#define	SEQ_NAME	"D:\\���D�uphoto\\demo\\Video- (%d).jpg"
//#define	SEQ_NAME	"D:\\���D�uphoto\\Maskout\\Video-%d.jpg"
//#define	SEQ_NAME	"D:\\���D�uphoto\\downtown\\Video-%d.jpg"
//#define	SEQ_NAME	"D:\\���D�uphoto\\night\\Vidio- (%d).jpg"
//#define	SEQ_NAME	"D:\\���D�uphoto\\freeway2\\Vidoi- (%d).jpg"


//�e�X�����I��m�P�P��P����
//�]�w���������I���W�U���k�h�ֽd��
#define vp_range 50

int main(int argc, char *argv[])
{
	char FileName[200], FileName2[200], FileName3[200], maskout[200];
	
	//�Ĥ@�i�X�{���Ϥ��s��
	int  FristPic = 1;  
	//Mask ��l��
	Mask_Init(); 
	for (int i = FristPic; i < 3000; i += 1){


		//===========�Ϥ�brbrbr
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\�]�����D�u\\goodmothing\\PICTURE\\Video-%d.jpg", i); //���]�����t
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\�]�����D�u\\goodmothing\\PICTURE\\Video-%d.jpg", i + 9); //���]�����t
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\�鶡���D�u\\\good0002\\PICTURE\\video-%d.jpg",i);  //���鶡��D
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\�鶡���D�u\\\good0002\\PICTURE\\video-%d.jpg",i+9);//���鶡��D
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\�鶡���D�u\\VIDEO0003 (2014-4-20 �U�� 10-10-12)\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\�鶡���D�u\\VIDEO0003 (2014-4-20 �U�� 10-10-12)\\Video-%d.jpg",i+1);
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\�]�����D�u\\CIMG3461 (2014-4-20 �U�� 10-15-45)\\Video-%d.jpg", i);
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\�]�����D�u\\CIMG3461 (2014-4-20 �U�� 10-15-45)\\Video-%d.jpg", i + 1);
		sprintf(FileName, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i);    //���鶡���t
		sprintf(FileName2, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i+1); //���鶡���t
		//sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+1);
		////sprintf(FileName3, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+2);

		//===================Ū�i�Ϥ�(���]�j�p)==================
		//=======================================================
		IplImage *src1 = cvLoadImage(FileName, 0); //Ū�i���
		IplImage *src2 = cvLoadImage(FileName2, 0);//Ū�i���
		IplImage *src3 = cvLoadImage(FileName2, 1);//Ū���m��
		if(src1==NULL||src2==NULL||src3==NULL) break;

		CvSize pImgA_size; 
		CvSize pImgB_size;  
		CvSize pImgColor_size; //�s��pic�j�p


		pImgA_size.width = src1->width* (1280.0 / src1->width); //���]pImgA�j�p
		pImgA_size.height = src1->height* (720.0 / src1->height);
		pImgB_size.width = src2->width* (1280.0 / src2->width); //���]pImgB�j�p
		pImgB_size.height = src2->height* (720.0 / src2->height);
		cout << pImgA_size.width << "   " << pImgA_size.height << "         "<<endl;  //DBG��

		IplImage *pImgA = cvCreateImage(pImgA_size, src1->depth, src1->nChannels); //�Хߥؼмv��A
		IplImage *pImgB = cvCreateImage(pImgB_size, src2->depth, src2->nChannels); //�Хߥؼмv��B
		IplImage *pImgColor = cvCreateImage(cvSize(src2->width* (1280.0 / src2->width), src2->height* (720.0 / src2->height)), src3->depth, src3->nChannels);
		
		//���ܤj�p
		cvResize(src1, pImgA, CV_INTER_LINEAR); 
		cvResize(src2, pImgB, CV_INTER_LINEAR);
		cvResize(src3, pImgColor, CV_INTER_LINEAR);

		cvReleaseImage(&src1);
		cvReleaseImage(&src2);
		cvReleaseImage(&src3);

		//�e�m�B�z======
		

		IplImage *pImgC = cvCreateImage(cvSize(pImgA->width, pImgA->height), pImgA->depth, pImgA->nChannels); //�Źϼh ��l��
		cvCopy(pImgA, pImgC); //img1 copy to imgout

		//===========================================================



		//===========�|��=====================
		//�|��(input1,input2, *output)
		pImgC = xxhh(pImgA, pImgB, pImgC);  

		//============�Ĥ@�i����canny==========
		//�Ĥ@�i��canny(�S���L�o)
		if (i == FristPic){  
			pImgCanny = canny(pImgC, pImgBuffer);
		}

		//============�����Ifunction===========
		//Find the vanishing point
		//Create savePoint of Vector<CvPoint>
		static vector<CvPoint> savePoint;
		//call vanishing point function and get the vanishing point to vpfnpoint
		CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny, pImgC, &savePoint);
		cvLine( pImgColor, VanishingPoint, VanishingPoint, CV_RGB(0,255,0), 7);

		//�ˬd�����I���T��
		Check_VPoint(VanishingPoint.x,VanishingPoint.y); 
		//cvLine(pImgColor,VanishingPoint,VanishingPoint,CV_RGB(225,0,0),20,4); //���X�����I

		cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl << endl;// ���ͮ����I(debug)
		//��Ĥ@�Ӯ����I��i ����I�}�C(100�O�W��)
		RLpoint[0][0] = VanishingPoint.x; RLpoint[0][1] = VanishingPoint.y; 

		//============��Ϫ�canny==============
		if (i != FristPic) pImgCanny = canny(pImgC, pImgBuffer); //����A����canny
		cout << "Video-" << i - 1 << "and Video-" << i << "print out C" << endl; //debug

		//===========���u(�I)===================
		pImgC = drawline(pImgC, VanishingPoint.x, VanishingPoint.y); //drawline (��J�Ϥ�,�����IX,�����IY)//���X����I(���o)

		//========================================
		//draw the vanishing point range
		if (VanishingPoint.x != NULL || VanishingPoint.y != NULL)
			draw_VPoint(pImgColor, VanishingPoint.x, VanishingPoint.y, vp_range);
		//Find the best lines (Original picture,Full Canny,Vanishing point(CvPoint),Vanishing point range(vp_range)�^
		FindTheBestLines(pImgColor, pImgCanny, VanishingPoint, vp_range);

		//���D����
		if( Lane_Offset(VanishingPoint,100,500)==-1){
		cvLine( pImgColor, cvPoint(0,0), cvPoint(1270,0), CV_RGB(255,0,0), 15);
		cvLine( pImgColor, cvPoint(0,0), cvPoint(0,715), CV_RGB(255,0,0), 15);
		}
		else if( Lane_Offset(VanishingPoint,100,500)==1){
		cvLine( pImgColor, cvPoint(0,0), cvPoint(1270,0), CV_RGB(255,0,0), 15);
		cvLine( pImgColor, cvPoint(0,0), cvPoint(0,715), CV_RGB(255,0,0),15);
		}
		

		//============�B�ncanny�� (�p�G�O�]���Ҧ��Ш���)=======================
		cvSet(pImgFilter,cvScalar(0,0,0));
		Filter_Init(VanishingPoint.x);

		//sprintf(maskout, "C:\\Users\\User\\Desktop\\LLSample\\Mask Test\\Mask\\MonXinFilterColor\\Video-%d.jpg",i);
		//cvSaveImage(maskout,pImgCanny);

		// Create Windows
		cvShowImage("pImgColor", pImgColor);
		//cvShowImage("pImgCanny", pImgCanny);

		cvReleaseImage(&pImgA); cvReleaseImage(&pImgB); cvReleaseImage(&pImgC);//cvReleaseImage(&pImgD); //����O����
		cvReleaseImage(&pImgColor);
		//if(i==400) waitKey(0);
		waitKey(1);
	}

	
	//cvReleaseImage(&pImgBlack); 
	cvReleaseImage(&pImgCanny);  
	cvReleaseImage(&pImgFilter);
	
	
	return 0;
}




