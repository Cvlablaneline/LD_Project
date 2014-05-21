#ifndef CANNY_LINE
#define CANNY_LINE

//CvPoint find_Vanishing_Point(IplImage* Canny,IplImage* Ori_pic);
IplImage* xxhh( IplImage *img1,IplImage *img2,IplImage *imgout);
IplImage *canny(IplImage *img1,IplImage *dst_DThrSmo);  //canny(輸入圖片,緩衝圖層)
IplImage* drawline(IplImage *pImgC,int centerX,int centerY);
IplImage* KeepLine(int ax,int ay,int bx,int by, IplImage *pImgC);

extern int RLpoint[][2];


#endif