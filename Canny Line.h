#ifndef CANNY_LINE
#define CANNY_LINE

//CvPoint find_Vanishing_Point(IplImage* Canny,IplImage* Ori_pic);
IplImage* xxhh( IplImage *img1,IplImage *img2,IplImage *imgout);
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo);  //canny(��J�Ϥ�,�w�Ĺϼh)
IplImage* drawline(IplImage *pImgC,int centerX,int centerY);
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgDis);


//void Mask_Init();
void Mask_Init(char FileName[200]);
void Filter_Init(int VPx,int VPy);
void Check_VPoint(int &VPx,int &VPy);
bool Lane_Offset(CvPoint VPoint,int lx,int rx); //�ǤJ (�����I,��X,�kX)



 extern int RLpoint[][2];
 extern int MaskRL[][2];  //�s�B�n���k���s�I
 extern IplImage *pImgCanny ; //����canny��
 extern IplImage *pImgBuffer; //�w�Ĺϼh��l��
 extern IplImage *pImgFilter ; //���B�n�ϼh ��l��
 extern IplImage *pImgHouf; //�N�ҽu

 extern IplImage *pImgA; //�Ĥ@���q�ι�
 extern IplImage *pImgB; //�ĤG���q�ι�
 extern IplImage *pImgC;//�q�ι���I
 extern IplImage *pImgGrayC;





#endif