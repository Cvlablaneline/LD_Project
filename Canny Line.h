#ifndef CANNY_LINE
#define CANNY_LINE

//CvPoint find_Vanishing_Point(IplImage* Canny,IplImage* Ori_pic);
IplImage* xxhh( IplImage *img1,IplImage *img2,IplImage *imgout);
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo);  //canny(��J�Ϥ�,�w�Ĺϼh)
IplImage* drawline(IplImage *pImgC,int centerX,int centerY);
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgC);
void Mask_Init();
void Filter_Init(int VPx);


 extern int RLpoint[][2];

 extern IplImage *pImgCanny ; //����canny��
 extern IplImage *pImgBlack; //���¹ϼh
 extern IplImage *pImgBuffer =cvCreateImage(cvSize(1280,720), 8,1); //�w�Ĺϼh��l��
 extern IplImage *pImgFilter ; //���B�n�ϼh ��l��

 


#endif