#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include "x86intrin.h"
#include <math.h>
#include <time.h>

using namespace cv;
using namespace std;

int main( )
{
    Mat in_img;
    int index;
    time_t start, end;
    time_t time1, time2;
    
    unsigned char *inIm, *outIm;
    
    int counter = 0;   
    
    // LOAD image
    in_img = imread("/Users/ben/Desktop/HPC/Homework/HW1/2/SSE/flag.png", 0);
    Mat out_img(512,512,CV_8UC1, Scalar(0));
    Mat out_img2(512,512,CV_8UC1, Scalar(0));      
    inIm = in_img.data;
    //outIm = &out_img.at<uchar>(0,0);

    unsigned char *pSrc;
    unsigned char *pRes;
    unsigned char *pResC;
    
    pSrc = inIm;
    pRes = &out_img2.at<uchar>(0,0);
    pResC = &out_img.at<uchar>(0,0);

    __m128i zero = _mm_setzero_si128();

    __m128i m1, m2, m3, m4, m5, m6, m7, m8;
    __m128i m1h, m2h, m3h, m4h, m5h, m6h, m7h, m8h;

    __m128i m1n4,m3n6,m2n5;
    __m128i m1n3,m4n6,m7n8;
    __m128i m1n4h,m3n6h,m2n5h;
    __m128i m1n3h,m4n6h,m7n8h;

    __m128i m2n5x2l,m2n5x2h,m1n4p3n6l,m1n4p3n6h,mloY,mhiY;
    __m128i m7n8x2l,m7n8x2h,m1n3p4n6l,m1n3p4n6h,mloX,mhiX;
    __m128i mreslo,mreshi,mres, Cres, mask;

    __m128 mreshh,mreshl,mreslh,mresll;

    start = clock();
    for (int i = 0; i < 511; i++)
    {
      index = i<<9;
      for (int j = 0; j <= 496; j+=16)
      {
    m1 = _mm_loadu_si128((const __m128i*)(pSrc + index + j));
    m2 = _mm_loadu_si128((const __m128i*)(pSrc + index + j + 1));
    m3 = _mm_loadu_si128((const __m128i*)(pSrc + index + j + 2));
    m4 = _mm_loadu_si128((const __m128i*)(pSrc + index+1024 + j));
    m5 = _mm_loadu_si128((const __m128i*)(pSrc + index+1025 + j));
    m6 = _mm_loadu_si128((const __m128i*)(pSrc + index+1026 + j));
    m7 = _mm_loadu_si128((const __m128i*)(pSrc + index+512 + j));
    m8 = _mm_loadu_si128((const __m128i*)(pSrc + index+514 + j));
    
    m1h = _mm_unpackhi_epi8(m1, zero);
    m2h = _mm_unpackhi_epi8(m2, zero);
    m3h = _mm_unpackhi_epi8(m3, zero);
    m4h = _mm_unpackhi_epi8(m4, zero);
    m5h = _mm_unpackhi_epi8(m5, zero);
    m6h = _mm_unpackhi_epi8(m6, zero);
    m7h = _mm_unpackhi_epi8(m7, zero);
    m8h = _mm_unpackhi_epi8(m8, zero);
    m1 = _mm_unpacklo_epi8(m1, zero);
    m2 = _mm_unpacklo_epi8(m2, zero);
    m3 = _mm_unpacklo_epi8(m3, zero);
    m4 = _mm_unpacklo_epi8(m4, zero);
    m5 = _mm_unpacklo_epi8(m5, zero);
    m6 = _mm_unpacklo_epi8(m6, zero);
    m7 = _mm_unpacklo_epi8(m7, zero);
    m8 = _mm_unpacklo_epi8(m8, zero);

/////*********************************************************** filterY
    m1n4h = _mm_sub_epi16(m1h, m4h);
    m3n6h = _mm_sub_epi16(m3h, m6h);
    m2n5h = _mm_sub_epi16(m2h, m5h);
    m1n4 = _mm_sub_epi16(m1, m4);
    m3n6 = _mm_sub_epi16(m3, m6);
    m2n5 = _mm_sub_epi16(m2, m5);
/////*********************************************************** filterX
    m1n3h = _mm_sub_epi16(m1h, m3h);
    m4n6h = _mm_sub_epi16(m4h, m6h);
    m7n8h = _mm_sub_epi16(m7h, m8h);
    m1n3 = _mm_sub_epi16(m1, m3);
    m4n6 = _mm_sub_epi16(m4, m6);
    m7n8 = _mm_sub_epi16(m7, m8);

/////*********************************************************** filterY
    m2n5x2l = _mm_slli_epi16(m2n5 ,1);
    m2n5x2h = _mm_slli_epi16(m2n5h ,1);
    m1n4p3n6l = _mm_add_epi16(m1n4 ,m3n6);
    m1n4p3n6h = _mm_add_epi16(m1n4h ,m3n6h);
    mloY = _mm_add_epi16(m2n5x2l,m1n4p3n6l);
    mhiY = _mm_add_epi16(m2n5x2h,m1n4p3n6h);
/////*********************************************************** filterX
    m7n8x2l = _mm_slli_epi16(m7n8 ,1);
    m7n8x2h = _mm_slli_epi16(m7n8h ,1);
    m1n3p4n6l = _mm_add_epi16(m1n3 ,m4n6);
    m1n3p4n6h = _mm_add_epi16(m1n3h ,m4n6h );
    mloX = _mm_add_epi16(m7n8x2l,m1n3p4n6l);
    mhiX = _mm_add_epi16(m7n8x2h,m1n3p4n6h);
/////*********************************************************** Result
    mloX = _mm_mullo_epi16(mloX,mloX);
    mloY = _mm_mullo_epi16(mloY,mloY);
    mhiX = _mm_mullo_epi16(mhiX,mhiX);
    mhiY = _mm_mullo_epi16(mhiY,mhiY);
    mreslo = _mm_add_epi16(mloX,mloY);
    mreshi = _mm_add_epi16(mhiX,mhiY);
/////*********************************************************** Convert to float
    mresll = _mm_cvtepi32_ps(_mm_unpacklo_epi16(mreslo, zero));
    mreslh = _mm_cvtepi32_ps(_mm_unpackhi_epi16(mreslo, zero));
    mreshl = _mm_cvtepi32_ps(_mm_unpacklo_epi16(mreshi, zero));
    mreshh = _mm_cvtepi32_ps(_mm_unpackhi_epi16(mreshi, zero));
    mreshh = _mm_sqrt_ps(mreshh);
    mreshl = _mm_sqrt_ps(mreshl);
    mreslh = _mm_sqrt_ps(mreslh);
    mresll = _mm_sqrt_ps(mresll);
/////*********************************************************** Convert to int
    mres = _mm_packus_epi16(_mm_packs_epi32(_mm_cvtps_epi32(mresll),_mm_cvtps_epi32(mreslh)),_mm_packs_epi32(_mm_cvtps_epi32(mreshl),_mm_cvtps_epi32(mreshh)));
    Cres = _mm_loadu_si128((__m128i*)(pResC + index+513 + j));
    mask = _mm_cmpeq_epi8(mres,Cres);
    
    _mm_storeu_si128((__m128i*)(pSrc + index+513 + j), mres);
      }
    }
    end   = clock();
    time2 = end - start;


    int co,so;
    int tempo;
    int max = 0;
    for (int i = 16; i < 496; i++)
    {
      for (int j = 16; j < 496; j++)
      {
    co = out_img.at<uchar>(i,j); 
    so = out_img2.at<uchar>(i,j);
    if (co != so)
    {
      tempo = abs(co - so);
      if (tempo > max)
      {
        max = tempo;
      }
      counter+= tempo;
    }
      }
    }

    //DISPLAY image
    imshow( "SIMD", out_img2 );             // Show our image inside it.
    imwrite( "OutC_Image.jpg", out_img );
    imwrite( "OutSIMD_Image.jpg", out_img2 );

    waitKey(0);                         // Wait for a keystroke in the window
    //printf ("Speedup = %4.2f\n", (float) (time1)/(float) time2);
    //printf ("counter = %d \n", counter);
    printf ("Parallel Run time = %4.1f \n", (float)time2);

    return 0;
}
