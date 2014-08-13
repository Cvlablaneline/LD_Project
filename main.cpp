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

//¹Ï¹³ÀÉ®×¦ì¸m
//#define	SEQ_NAME	"/Users/chienfu/Desktop/¨®¹D½u°»´ú/demo/Video- (%d).jpg"
//#define	SEQ_NAME	"D:\\¨®¹D½uphoto\\demo\\Video- (%d).jpg"
//#define	SEQ_NAME	"D:\\¨®¹D½uphoto\\Maskout\\Video-%d.jpg"
//#define	SEQ_NAME	"D:\\¨®¹D½uphoto\\downtown\\Video-%d.jpg"
//#define	SEQ_NAME	"D:\\¨®¹D½uphoto\\night\\Vidio- (%d).jpg"
//#define	SEQ_NAME	"D:\\¨®¹D½uphoto\\freeway2\\Vidoi- (%d).jpg"


//µe¥X®ø¥¢ÂI¦ì¸m»P©P³ò·PÀ³°Ï
//³]©w°»´ú®ø¥¢ÂIªº¤W¤U¥ª¥k¦h¤Ö½d³ò
#define vp_range 50

int main(int argc, char *argv[])
{
	char FileName[200], FileName2[200], FileName3[200], maskout[200];
	
	//²Ä¤@±i¥X²{ªº¹Ï¤ù½s¸¹
	int  FristPic = 4;  
	
	for (int i = FristPic; i < 3000; i += 1){


		//===========¹Ï¤ùbrbrbr
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\©]¶¡¨®¹D½u\\goodmothing\\PICTURE\\Video-%d.jpg", i); //¡¹©]¶¡°ª³t
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\©]¶¡¨®¹D½u\\goodmothing\\PICTURE\\Video-%d.jpg", i + 9); //¡¹©]¶¡°ª³t
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\¤é¶¡¨®¹D½u\\\good0002\\PICTURE\\video-%d.jpg",i);  //¡¹¤é¶¡µó¹D
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\¤é¶¡¨®¹D½u\\\good0002\\PICTURE\\video-%d.jpg",i+9);//¡¹¤é¶¡µó¹D
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\¤é¶¡¨®¹D½u\\VIDEO0003 (2014-4-20 ¤U¤È 10-10-12)\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\¤é¶¡¨®¹D½u\\VIDEO0003 (2014-4-20 ¤U¤È 10-10-12)\\Video-%d.jpg",i+1);
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\©]¶¡¨®¹D½u\\CIMG3461 (2014-4-20 ¤U¤È 10-15-45)\\Video-%d.jpg", i);
		//sprintf(FileName2, "C:\\Users\\user\\Desktop\\©]¶¡¨®¹D½u\\CIMG3461 (2014-4-20 ¤U¤È 10-15-45)\\Video-%d.jpg", i + 1);
		//sprintf(FileName, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i);    //¡¹¤é¶¡°ª³t
		sprintf(FileName2, "C:\\Users\\user\\Desktop\\output\\Out2\\Video-%d.jpg",i+1); //¡¹¤é¶¡°ª³t
		//sprintf(FileName, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i);
		//sprintf(FileName2, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+1);
		////sprintf(FileName3, "C:\\Users\\User\\Desktop\\LLSample\\output\\Video-%d.jpg",i+2);

		//Mask ªì©l¤Æ (¦b²Ä¤@±i ¥u°µ¤@¦¸)
		if(i==FristPic)
		{
		Mask_Init(FileName2); 
		}
		else
		{

		//===================Åª¶i¹Ï¤ù(­«³]¤j¤p)==================
		//=======================================================
		
		IplImage *src2 = cvLoadImage(FileName2, 0);//Åª¶i­ì¹Ï
		IplImage *src3 = cvLoadImage(FileName2, 1);//Åª¨ú±m¹Ï
		
		if(src2==NULL||src3==NULL) break;

		
		CvSize pImgB_size;  
		CvSize pImgColor_size; //·sªºpic¤j¤p


		
		pImgB_size.width = src2->width* (1280.0 / src2->width); //­«³]pImgB¤j¤p
		pImgB_size.height = src2->height* (720.0 / src2->height);
		//cout << pImgA_size.width << "   " << pImgA_size.height << "         "<<endl;  //DBG¥Î
		
		
		IplImage *pImgColor = cvCreateImage(cvSize(src2->width* (1280.0 / src2->width), src2->height* (720.0 / src2->height)), src3->depth, src3->nChannels);
		
		
		cvCopy(pImgB, pImgA); //img1 copy to imgout
		//§ïÅÜ¤j¤p
		cvResize(src2, pImgB, CV_INTER_LINEAR);
		cvResize(src3, pImgColor, CV_INTER_LINEAR);

		//cvReleaseImage(&src1);
		cvReleaseImage(&src2);
		cvReleaseImage(&src3);

		//«e¸m³B²z======
		

		IplImage *pImgC = cvCreateImage(cvSize(pImgA->width, pImgA->height), pImgA->depth, pImgA->nChannels); //ªÅ¹Ï¼h ªì©l¤Æ
		cvCopy(pImgA, pImgC); //img1 copy to imgout
		
		//===========================================================



		//===========Å|¹Ï=====================
		//Å|¹Ï(input1,input2, *output)
		pImgC = xxhh(pImgA, pImgB, pImgC);  

		//============²Ä¤@±i¥ı°µcanny==========
		//²Ä¤@±iªºcanny(¨S¦³¹LÂo)
		if (i == FristPic){  
			pImgCanny = canny(pImgC, pImgBuffer);
<<<<<<< HEAD
			
		//===========å°æ¯”ç·š(é»)===================
			pImgC = drawline(pImgC, oldXX, oldYY); //drawline (è¼¸å…¥åœ–ç‰‡,æ¶ˆå¤±é»X,æ¶ˆå¤±é»Y)//åŠƒå‡ºå°æ¯”é»(å–å¾—)
            
            cout << "======END DRAWLINE========="<<endl;
		 
		//============é®ç½©cannyåœ– (å¦‚æœæ˜¯å¤œé–“æ¨¡å¼è«‹å–æ¶ˆ)=======================
			cvSet(pImgFilter,cvScalar(0,0,0));
			Filter_Init(oldXX);//VanishingPoint.x
			cvShowImage("canny",pImgCanny);

			//============æ¶ˆå¤±é»function===========
            //Find the vanishing point
            //Create savePoint of Vector<CvPoint>
            static vector<line_property> LSPointSave,RSPointSave;
            //call vanishing point function and get the vanishing point to vpfnpoint
            CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny, pImgC, &LSPointSave, &RSPointSave);
            cvLine( pImgColor, VanishingPoint, VanishingPoint, CV_RGB(0,255,0), 7);
            
			//æª¢æŸ¥æ¶ˆå¤±é»æ­£ç¢ºæ€§
            Check_VPoint(VanishingPoint.x,VanishingPoint.y);
            //cvLine(pImgColor,VanishingPoint,VanishingPoint,CV_RGB(225,0,0),20,4); //åŠƒå‡ºæ¶ˆå¤±é»
            
            cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl << endl;// ç”¢ç”Ÿæ¶ˆå¤±é»(debug)
            //æŠŠç¬¬ä¸€å€‹æ¶ˆå¤±é»æ”¾é€² å°æ¯”é»é™£åˆ—(100æ˜¯ä¸Šé™)
            RLpoint[0][0] = VanishingPoint.x; RLpoint[0][1] = VanishingPoint.y;
			oldXX=VanishingPoint.x;oldYY=VanishingPoint.y;


           
            //=======é»ƒå»ºå¯Œ===================
            //draw the vanishing point range
            if (VanishingPoint.x != NULL || VanishingPoint.y != NULL)
                draw_VPoint(pImgColor, VanishingPoint.x, VanishingPoint.y, vp_range);
            //Find the best lines (Original picture,Full Canny,Vanishing point(CvPoint),Vanishing point range(vp_range)ï¼‰
            FTBL ftblData = FindTheBestLines(pImgColor, &LSPointSave, &RSPointSave, VanishingPoint, vp_range);
            //æ¸…é™¤è»Šé“ç·šæ–œç‡è¨˜éŒ„vector
            vector <line_property> Nls, Nrs;
            LSPointSave.clear();
            RSPointSave.clear();
            LSPointSave.swap(Nls);
            RSPointSave.swap(Nrs); 
		

		//è»Šé“åç§»
=======
		}

		//============®ø¥¢ÂIfunction===========
		//Find the vanishing point
		//Create savePoint of Vector<CvPoint>
		static vector<CvPoint> savePoint;
		//call vanishing point function and get the vanishing point to vpfnpoint
		CvPoint VanishingPoint = find_Vanishing_Point(pImgCanny, pImgC, &savePoint);
		cvLine( pImgColor, VanishingPoint, VanishingPoint, CV_RGB(0,255,0), 7);

		//ÀË¬d®ø¥¢ÂI¥¿½T©Ê
		Check_VPoint(VanishingPoint.x,VanishingPoint.y); 
		//cvLine(pImgColor,VanishingPoint,VanishingPoint,CV_RGB(225,0,0),20,4); //¹º¥X®ø¥¢ÂI

		cout << "VanishingPoint Find!>> " << VanishingPoint.x << "   " << VanishingPoint.y << endl << endl;// ²£¥Í®ø¥¢ÂI(debug)
		//§â²Ä¤@­Ó®ø¥¢ÂI©ñ¶i ¹ï¤ñÂI°}¦C(100¬O¤W­­)
		RLpoint[0][0] = VanishingPoint.x; RLpoint[0][1] = VanishingPoint.y; 

		//============«á¹Ïªºcanny==============
		if (i != FristPic) pImgCanny = canny(pImgC, pImgBuffer); //¤§«á¦A°µªºcanny
		cout << "Video-" << i - 1 << "and Video-" << i << "print out C" << endl; //debug

		//===========¹ï¤ñ½u(ÂI)===================
		pImgC = drawline(pImgC, VanishingPoint.x, VanishingPoint.y); //drawline (¿é¤J¹Ï¤ù,®ø¥¢ÂIX,®ø¥¢ÂIY)//¹º¥X¹ï¤ñÂI(¨ú±o)

		//========================================
		//draw the vanishing point range
		if (VanishingPoint.x != NULL || VanishingPoint.y != NULL)
			draw_VPoint(pImgColor, VanishingPoint.x, VanishingPoint.y, vp_range);
		//Find the best lines (Original picture,Full Canny,Vanishing point(CvPoint),Vanishing point range(vp_range)¡^
		FTBL ftblData = FindTheBestLines(pImgColor, pImgCanny, VanishingPoint, vp_range);

		//¨®¹D°¾²¾
>>>>>>> parent of ffb9864... 7/10 meeting progress
		if( Lane_Offset(VanishingPoint, ftblData.FTBL_Point_L.x, ftblData.FTBL_Point_R.x)==true){
			CvFont font;
			// PlaySound(TEXT("C:\\Users\\user\\Desktop\\AudioJoiner140604213842.wav"),NULL,SND_FILENAME | SND_SYNC );
			cvInitFont(&font,CV_FONT_HERSHEY_TRIPLEX  ,8.0f,1.0f,0,5,CV_AA);
			cvPutText(pImgColor,"Warning!" , Point(1280/4,720/2),& font ,CV_RGB(255,0,0));
		}
		else if (Lane_Offset(VanishingPoint, ftblData.FTBL_Point_L.x, ftblData.FTBL_Point_R.x) == false){
		}
<<<<<<< HEAD
		
			
=======
>>>>>>> parent of ffb9864... 7/10 meeting progress
		

		//============¾B¸ncanny¹Ï (¦pªG¬O©]¶¡¼Ò¦¡½Ğ¨ú®ø)=======================
		cvSet(pImgFilter,cvScalar(0,0,0));
		Filter_Init(VanishingPoint.x);

		//sprintf(maskout, "C:\\Users\\User\\Desktop\\LLSample\\Mask Test\\Mask\\MonXinFilterColor\\Video-%d.jpg",i);
		//cvSaveImage(maskout,pImgCanny);

		// Create Windows
		cvShowImage("pImgColor", pImgColor);
		//cvShowImage("pImgCanny", pImgCanny);

		 cvReleaseImage(&pImgC); //ÄÀ©ñ°O¾ĞÅé
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




