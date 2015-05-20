/*M///////////////////////////////////////////////////////////////////////////////////////
 //
 //  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 //
 //  By downloading, copying, installing or using the software you agree to this license.
 //  If you do not agree to this license, do not download, install,
 //  copy or use the software.
 //
 //
 //                        Intel License Agreement
 //                For Open Source Computer Vision Library
 //
 // Copyright (C) 2000, Intel Corporation, all rights reserved.
 // Third party copyrights are property of their respective owners.
 //
 // Redistribution and use in source and binary forms, with or without modification,
 // are permitted provided that the following conditions are met:
 //
 //   * Redistribution's of source code must retain the above copyright notice,
 //     this list of conditions and the following disclaimer.
 //
 //   * Redistribution's in binary form must reproduce the above copyright notice,
 //     this list of conditions and the following disclaimer in the documentation
 //     and/or other materials provided with the distribution.
 //
 //   * The name of Intel Corporation may not be used to endorse or promote products
 //     derived from this software without specific prior written permission.
 //
 // This software is provided by the copyright holders and contributors "as is" and
 // any express or implied warranties, including, but not limited to, the implied
 // warranties of merchantability and fitness for a particular purpose are disclaimed.
 // In no event shall the Intel Corporation or contributors be liable for any direct,
 // indirect, incidental, special, exemplary, or consequential damages
 // (including, but not limited to, procurement of substitute goods or services;
 // loss of use, data, or profits; or business interruption) however caused
 // and on any theory of liability, whether in contract, strict liability,
 // or tort (including negligence or otherwise) arising in any way out of
 // the use of this software, even if advised of the possibility of such damage.
 //
 //M*/

#include "VP_canny_precomp.hpp"//xin_2015_4_18
#include <cv.h>
#include <highgui.h>
/*
 #if defined (HAVE_IPP) && (IPP_VERSION_MAJOR >= 7)
 #define USE_IPP_CANNY 1
 #else
 #undef USE_IPP_CANNY
 #endif
 */
//xin_2015_4_18
CV_IMPL void AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high)//輸入梯度值
{
    CvSize size;  //宣告一大小
    IplImage *imge=0; //宣告一影像
    int i,j;
    CvHistogram *hist; //宣告一直方資料
    int hist_size = 255;
    float range_0[]={0,256};//直方圖範圍
    float* ranges[] = { range_0 };
    double  PercentOfPixelsNotEdges = 0.99; //影像中canny後所有導數數量占整張圖的幾%
    size = cvGetSize(dx);//取得原圖X導數後寬、高
    imge = cvCreateImage(size, IPL_DEPTH_32F, 1);//建立一單通道影像
    // 计算边缘的强度, 并存于图像中
    float maxv = 0;//改成幾行陣列
    for(i = 0; i < size.height; i++ )
    {
        const short* _dx = (short*)(dx->data.ptr + dx->step*i);//每行導數x資料
        const short* _dy = (short*)(dy->data.ptr + dy->step*i);//每行導數y資料
        
        float* _image = (float *)(imge->imageData + imge->widthStep*i);//建立影像行資料
        for(j = 0; j < size.width; j++)
        {
            _image[j] = (float)(abs(_dx[j]) + abs(_dy[j]));//存每列梯度大小
            if(i> size.height/2)
            {
                maxv = maxv < _image[j] ? _image[j]: maxv;//找出整張圖最大梯度值
            }
        }
        
        
    }
    
    // 计算直方图
    range_0[1] = maxv;
    hist_size = (int)(hist_size > maxv ? maxv:hist_size);//放入最高導數值
    //第一個引數為維度的選擇,第二個引數為要將直方圖切割為多少區塊,第三個引數為選擇cvCreateHist()函數的參數或代號,第四個引數為每一個維度的上下界
    hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);//建立一密集矩陣直方圖
    //計算直方圖累積數據,第一個引數為輸入目標IplImage圖形陣列資料結構,第二個引數輸出為CvHistogram資料結構
    cvCalcHist( &imge, hist, 0, NULL );//計算梯度影像直方圖
    int total = (int)(size.height * size.width * PercentOfPixelsNotEdges);//至少要去除的數量
    float sum=0;
    int icount = hist->mat.dim[0].size;//取出直方圖所有(最大導數值減導數值)(x)
    
    float *h = (float*)cvPtr1D( hist->bins, 0 );//取出直方圖(所有影像pixel數減所有導數值數量)(Y)
    for(i = 0; i < icount; i++)
    {
        sum += h[i];//取出所屬導數值的反數量
        if( sum > total )//假如大於整張影像之pixel的幾%
            break;
    }
    std::cout<< "total=" << total  << std::endl;
    std::cout<< "sum=" << sum  << std::endl;
    // 计算高低门限
    *high = (i+1) * maxv / hist_size ;
    *low = *high * 0.4;
    //cvNamedWindow("Histogram");
    // cvShowImage("Histogram",hist);
    
    //cvNamedWindow("hist");
    //cvMoveWindow("hist",660,300);
    //cvShowImage("hist",imge);
    // cvWaitKey(0);
    //cvReleaseImage( &imge );
    cvReleaseHist(&hist);
}
#ifdef USE_IPP_CANNY
namespace cv
{
    static bool ippCanny(const Mat& _src, Mat& _dst, float low,  float high)
    {
        int size = 0, size1 = 0;
        IppiSize roi = { _src.cols, _src.rows };
        
        ippiFilterSobelNegVertGetBufferSize_8u16s_C1R(roi, ippMskSize3x3, &size);
        ippiFilterSobelHorizGetBufferSize_8u16s_C1R(roi, ippMskSize3x3, &size1);
        size = std::max(size, size1);
        ippiCannyGetSize(roi, &size1);
        size = std::max(size, size1);
        
        AutoBuffer<uchar> buf(size + 64);
        uchar* buffer = alignPtr((uchar*)buf, 32);
        
        Mat _dx(_src.rows, _src.cols, CV_16S);
        if( ippiFilterSobelNegVertBorder_8u16s_C1R(_src.data, (int)_src.step,
                                                   _dx.ptr<short>(), (int)_dx.step, roi,
                                                   ippMskSize3x3, ippBorderRepl, 0, buffer) < 0 )
            return false;
        
        Mat _dy(_src.rows, _src.cols, CV_16S);
        if( ippiFilterSobelHorizBorder_8u16s_C1R(_src.data, (int)_src.step,
                                                 _dy.ptr<short>(), (int)_dy.step, roi,
                                                 ippMskSize3x3, ippBorderRepl, 0, buffer) < 0 )
            return false;
        
        if( ippiCanny_16s8u_C1R(_dx.ptr<short>(), (int)_dx.step,
                                _dy.ptr<short>(), (int)_dy.step,
                                _dst.data, (int)_dst.step, roi, low, high, buffer) < 0 )
            return false;
        return true;
    }
}
#endif

void cv::VP_Canny( InputArray _src, OutputArray _dst,
                  double low_thresh, double high_thresh,
                  int aperture_size, bool L2gradient )
{
    Mat src = _src.getMat();
    CV_Assert( src.depth() == CV_8U );//CV_Assert（）若括号中的表达式值为false，则返回一个错误信息。
    
    _dst.create(src.size(), CV_8U);
    Mat dst = _dst.getMat();
    
    if (!L2gradient && (aperture_size & CV_CANNY_L2_GRADIENT) == CV_CANNY_L2_GRADIENT)
    {
        //backward compatibility向後兼容性
        aperture_size &= ~CV_CANNY_L2_GRADIENT;
        L2gradient = true;
    }
    
    if ((aperture_size & 1) == 0 || (aperture_size != -1 && (aperture_size < 3 || aperture_size > 7)))
        CV_Error(CV_StsBadFlag, "");
    
    if (low_thresh > high_thresh)//高低閥值交換
        std::swap(low_thresh, high_thresh);
    
#ifdef HAVE_TEGRA_OPTIMIZATION//下面是针对TEGRA处理器的优化方法
    if (tegra::canny(src, dst, low_thresh, high_thresh, aperture_size, L2gradient))
        return;
#endif
    
#ifdef USE_IPP_CANNY//實現IPP_canny_http://book.51cto.com/art/200912/172356.htm
    if( aperture_size == 3 && !L2gradient &&
       ippCanny(src, dst, (float)low_thresh, (float)high_thresh) )
        return;
#endif
    
    const int cn = src.channels();
    Mat dx(src.rows, src.cols, CV_16SC(cn));
    Mat dy(src.rows, src.cols, CV_16SC(cn));

    Sobel(src, dx, CV_16S, 1, 0, aperture_size, 1, 0, cv::BORDER_REPLICATE);
    Sobel(src, dy, CV_16S, 0, 1, aperture_size, 1, 0, cv::BORDER_REPLICATE);
    //xin_2015_4_18
    if(low_thresh == -1 && high_thresh == -1)
    {
        CvMat _dx = dx; //(huang 5/21 fix for OSX xcode)
        CvMat _dy = dy; //(huang 5/21 fix for OSX xcode)
        AdaptiveFindThreshold(&_dx, &_dy, &low_thresh, &high_thresh); //(huang 5/21 fix for OSX xcode)
    }
    if (L2gradient)//前面什麼間容性處理
    {
        low_thresh = std::min(32767.0, low_thresh);
        high_thresh = std::min(32767.0, high_thresh);
        
        if (low_thresh > 0) low_thresh *= low_thresh;
        if (high_thresh > 0) high_thresh *= high_thresh;
    }
    int low = cvFloor(low_thresh);//cvFloor返回不大于参数的最大整数值
    int high = cvFloor(high_thresh);
    
    ptrdiff_t mapstep = src.cols + 2;//longlongint (64bit)
    AutoBuffer<uchar> buffer((src.cols+2)*(src.rows+2) + cn * mapstep * 3 * sizeof(int));
    
    int* mag_buf[3];
    mag_buf[0] = (int*)(uchar*)buffer;
    mag_buf[1] = mag_buf[0] + mapstep*cn;
    mag_buf[2] = mag_buf[1] + mapstep*cn;
    memset(mag_buf[0], 0, /* cn* */mapstep*sizeof(int));
    
    uchar* map = (uchar*)(mag_buf[2] + mapstep*cn);
    memset(map, 1, mapstep);
    memset(map + mapstep*(src.rows + 1), 1, mapstep);
    
    int maxsize = std::max(1 << 10, src.cols * src.rows / 10);
    std::vector<uchar*> stack(maxsize);
    uchar **stack_top = &stack[0];
    uchar **stack_bottom = &stack[0];
    //扇區號
    /* sector numbers
     (Top-Left Origin)
     
     1   2   3
     *  *  *
     * * *
     0*******0
     * * *
     *  *  *
     3   2   1
     */
    
#define CANNY_PUSH(d)    *(d) = uchar(2), *stack_top++ = (d)
#define CANNY_POP(d)     (d) = *--stack_top
    //計算梯度向量,非最大值抑制
    // calculate magnitude and angle of gradient, perform non-maxima suppression.
    //根據下列判斷值畫圖
    // fill the map with one of the following values:
    //   0 - the pixel might belong to an edge//可能是邊緣
    //   1 - the pixel can not belong to an edge//不是邊緣
    //   2 - the pixel does belong to an edge//不屬於邊緣
    for (int i = 0; i <= src.rows; i++)
    {
        int* _norm = mag_buf[(i > 0) + 1] + 1;
        if (i < src.rows)
        {
            short* _dx = dx.ptr<short>(i);
            short* _dy = dy.ptr<short>(i);
            
            if (!L2gradient)
            {
                for (int j = 0; j < src.cols*cn; j++)
                    _norm[j] = std::abs(int(_dx[j])) + std::abs(int(_dy[j]));
            }
            else
            {
                for (int j = 0; j < src.cols*cn; j++)
                    _norm[j] = int(_dx[j])*_dx[j] + int(_dy[j])*_dy[j];
            }
            
            if (cn > 1)
            {
                for(int j = 0, jn = 0; j < src.cols; ++j, jn += cn)
                {
                    int maxIdx = jn;
                    for(int k = 1; k < cn; ++k)
                        if(_norm[jn + k] > _norm[maxIdx]) maxIdx = jn + k;
                    _norm[j] = _norm[maxIdx];
                    _dx[j] = _dx[maxIdx];
                    _dy[j] = _dy[maxIdx];
                }
            }
            _norm[-1] = _norm[src.cols] = 0;
        }
        else
            memset(_norm-1, 0, /* cn* */mapstep*sizeof(int));
        
        // at the very beginning we do not have a complete ring
        // buffer of 3 magnitude rows for non-maxima suppression
        if (i == 0)
            continue;
        
        uchar* _map = map + mapstep*i + 1;
        _map[-1] = _map[src.cols] = 1;
        
        int* _mag = mag_buf[1] + 1; // take the central row
        ptrdiff_t magstep1 = mag_buf[2] - mag_buf[1];
        ptrdiff_t magstep2 = mag_buf[0] - mag_buf[1];
        
        const short* _x = dx.ptr<short>(i-1);
        const short* _y = dy.ptr<short>(i-1);
        
        if ((stack_top - stack_bottom) + src.cols > maxsize)
        {
            int sz = (int)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            stack.resize(maxsize);
            stack_bottom = &stack[0];
            stack_top = stack_bottom + sz;
        }
        
        int prev_flag = 0;
        for (int j = 0; j < src.cols; j++)
        {
#define CANNY_SHIFT 15
            const int TG22 = (int)(0.4142135623730950488016887242097*(1<<CANNY_SHIFT) + 0.5);
            
            int m = _mag[j];
            
            if (m > low)//@@@@@@@@@依照不同區域改變(依I,J值判斷位置)
            {
                int xs = _x[j];
                int ys = _y[j];
                int x = std::abs(xs);
                int y = std::abs(ys) << CANNY_SHIFT;
                
                int tg22x = x * TG22;
                
                if (y < tg22x)
                {
                    if (m > _mag[j-1] && m >= _mag[j+1]) goto __ocv_canny_push;
                }
                else
                {
                    int tg67x = tg22x + (x << (CANNY_SHIFT+1));
                    if (y > tg67x)
                    {
                        if (m > _mag[j+magstep2] && m >= _mag[j+magstep1]) goto __ocv_canny_push;
                    }
                    else
                    {
                        int s = (xs ^ ys) < 0 ? -1 : 1;
                        if (m > _mag[j+magstep2-s] && m > _mag[j+magstep1+s]) goto __ocv_canny_push;
                    }
                }
            }
            prev_flag = 0;
            _map[j] = uchar(1);
            continue;
        __ocv_canny_push:
            if (!prev_flag && m > high && _map[j-mapstep] != 2)
            {
                CANNY_PUSH(_map + j);
                prev_flag = 1;
            }
            else
                _map[j] = 0;
        }
        
        // scroll the ring buffer
        _mag = mag_buf[0];
        mag_buf[0] = mag_buf[1];
        mag_buf[1] = mag_buf[2];
        mag_buf[2] = _mag;
    }
    
    // now track the edges (hysteresis thresholding)
    while (stack_top > stack_bottom)
    {
        uchar* m;
        if ((stack_top - stack_bottom) + 8 > maxsize)
        {
            int sz = (int)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            stack.resize(maxsize);
            stack_bottom = &stack[0];
            stack_top = stack_bottom + sz;
        }
        
        CANNY_POP(m);
        
        if (!m[-1])         CANNY_PUSH(m - 1);
        if (!m[1])          CANNY_PUSH(m + 1);
        if (!m[-mapstep-1]) CANNY_PUSH(m - mapstep - 1);
        if (!m[-mapstep])   CANNY_PUSH(m - mapstep);
        if (!m[-mapstep+1]) CANNY_PUSH(m - mapstep + 1);
        if (!m[mapstep-1])  CANNY_PUSH(m + mapstep - 1);
        if (!m[mapstep])    CANNY_PUSH(m + mapstep);
        if (!m[mapstep+1])  CANNY_PUSH(m + mapstep + 1);
    }
    
    // the final pass, form the final image
    const uchar* pmap = map + mapstep + 1;
    uchar* pdst = dst.ptr();
    for (int i = 0; i < src.rows; i++, pmap += mapstep, pdst += dst.step)
    {
        for (int j = 0; j < src.cols; j++)
            pdst[j] = (uchar)-(pmap[j] >> 1);
    }
}
//xin_2015_4_18
void VP_cvCanny( const CvArr* image, CvArr* edges, double threshold1,
                double threshold2, int aperture_size )
{
    cv::Mat src = cv::cvarrToMat(image), dst = cv::cvarrToMat(edges);
    CV_Assert( src.size == dst.size && src.depth() == CV_8U && dst.type() == CV_8U );
    //xin_2015_4_18
    cv::VP_Canny(src, dst, threshold1, threshold2, aperture_size & 255,//傳入src,傳出dst
                 (aperture_size & CV_CANNY_L2_GRADIENT) != 0);
}

/* End of file. */
