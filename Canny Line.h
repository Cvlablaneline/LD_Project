#ifndef CANNY_LINE
#define CANNY_LINE

//CvPoint find_Vanishing_Point(IplImage* Canny,IplImage* Ori_pic);
IplImage* xxhh( IplImage *img1,IplImage *img2,IplImage *imgout,IplImage *pImgColor,int XXHHflag,IplConvKernel *pKernel);
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo);  //canny(輸入圖片,緩衝圖層)
IplImage* drawline(IplImage *pImgC,int centerX,int centerY);
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgDis);


//void Mask_Init();
void Mask_Init(char FileName[200]);
void Filter_Init(int VPx,int VPy);
void Check_VPoint(int &VPx,int &VPy);
bool Lane_Offset(CvPoint VPoint,int lx,int rx); //傳入 (消失點,左X,右X)



extern int RLpoint[][2];
extern int MaskRL[][2];  //新遮罩左右分群點
extern IplImage *pImgCanny ; //產生canny圖
extern IplImage *pImgBuffer; //緩衝圖層初始化
extern IplImage *pImgFilter ; //對比遮罩圖層 初始化
extern IplImage *pImgHouf; //霍夫線

extern IplImage *pImgA; //第一章通用圖
extern IplImage *pImgB; //第二章通用圖
extern IplImage *pImgC;//通用對比點
extern IplImage *pImgGrayC;
extern IplImage* pImg3C;




#endif
