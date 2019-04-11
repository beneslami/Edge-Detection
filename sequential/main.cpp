#include<iostream>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "time.h"

using namespace std;
using namespace cv;

int xGradient(Mat image, int x, int y)
{
    return image.at<uchar>(y-1, x-1) +
           2*image.at<uchar>(y, x-1) +
           image.at<uchar>(y+1, x-1) -
           image.at<uchar>(y-1, x+1) -
           2*image.at<uchar>(y, x+1) -
           image.at<uchar>(y+1, x+1);
}

int yGradient(Mat image, int x, int y)
{
    return image.at<uchar>(y-1, x-1) +
           2*image.at<uchar>(y-1, x) +
           image.at<uchar>(y-1, x+1) -
           image.at<uchar>(y+1, x-1) -
           2*image.at<uchar>(y+1, x) -
           image.at<uchar>(y+1, x+1);
}

int main()
{

    Mat src, dst;
    int gx, gy, sum;
    time_t start, end;

    // Load an image
    src = imread("/Users/ben/Desktop/HPC/Homework/HW1/2/flag.png", -1);
    dst = src.clone();
    if( src.empty() )
    {
        cout << "can't open\n";
        return -1;
    }

    //namedWindow("initial");
    //imshow("initial", src);
    //waitKey(0);
    
    start = clock();
    for(int y = 0; y < src.rows; y++)
        for(int x = 0; x < src.cols; x++)
            dst.at<uchar>(y,x) = 0.0;

    for(int y = 1; y < src.rows - 1; y++){
        for(int x = 1; x < src.cols - 1; x++){
            gx = xGradient(src, x, y);
            gy = yGradient(src, x, y);
            sum = abs(gx) + abs(gy);
            sum = sum > 255 ? 255:sum;
            sum = sum < 0 ? 0 : sum;
            dst.at<uchar>(y,x) = sum;
        }
    }
    end = clock();

    //namedWindow("final");
    //imshow("final", dst);
    //waitKey(0);

    cout << end - start << endl;
    return 0;
}