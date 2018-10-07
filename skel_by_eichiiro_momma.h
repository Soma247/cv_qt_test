//Kasvandの反復型線検出オペレータ 作成:Eiichiro Momma
//2008.1.21 ざっくり実装
#ifndef SKELBYMOMA_H
#define SKELBYMOMA_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#define MAX1(a,b) (a)<(b)?(b):(a)
void skel_by_eiichiro_momma(cv::Mat *src,cv::Mat *dst)
{

    cv::Mat *kpb = new cv::Mat[8];
    cv::Mat *kpw = new cv::Mat[8];
    kpb[0]=(cv::Mat_<float>(3,3) << 1,1,0,1,0,0,0,0,0);
    kpb[1]=(cv::Mat_<float>(3,3) << 1,1,1,0,0,0,0,0,0);
    kpb[2]=(cv::Mat_<float>(3,3) << 0,1,1,0,0,1,0,0,0);
    kpb[3]=(cv::Mat_<float>(3,3) << 0,0,1,0,0,1,0,0,1);
    kpb[4]=(cv::Mat_<float>(3,3) << 0,0,0,0,0,1,0,1,1);
    kpb[5]=(cv::Mat_<float>(3,3) << 0,0,0,0,0,0,1,1,1);
    kpb[6]=(cv::Mat_<float>(3,3) << 0,0,0,1,0,0,1,1,0);
    kpb[7]=(cv::Mat_<float>(3,3) << 1,0,0,1,0,0,1,0,0);

    kpw[0]=(cv::Mat_<float>(3,3) << 0,0,0, 0,1,1, 0,1,0);
    kpw[1]=(cv::Mat_<float>(3,3) << 0,0,0, 0,1,0, 1,1,0);
    kpw[2]=(cv::Mat_<float>(3,3) << 0,0,0, 1,1,0, 0,1,0);
    kpw[3]=(cv::Mat_<float>(3,3) << 1,0,0, 1,1,0, 0,0,0);
    kpw[4]=(cv::Mat_<float>(3,3) << 0,1,0, 1,1,0, 0,0,0);
    kpw[5]=(cv::Mat_<float>(3,3) << 0,1,1, 0,1,0, 0,0,0);
    kpw[6]=(cv::Mat_<float>(3,3) << 0,1,0, 0,1,1, 0,0,0);
    kpw[7]=(cv::Mat_<float>(3,3) << 0,0,0, 0,1,1, 0,0,1);


    cv::Mat src1;
    src->copyTo(src1);
    cv::Mat src_w(src1.rows,src1.cols, CV_32FC1);
    cv::Mat src_b(src1.rows,src1.cols, CV_32FC1);
    cv::Mat src_f(src1.rows,src1.cols, CV_32FC1);
    src1.convertTo(src_f, CV_32FC1);
    src_f.mul(1./255.);
    cv::threshold(src_f, src_f, 0.5, 1.0, CV_THRESH_BINARY);
    cv::threshold(src_f, src_w, 0.5, 1.0, CV_THRESH_BINARY);
    cv::threshold(src_f, src_b, 0.5, 1.0, CV_THRESH_BINARY_INV);

    double sum=1;
    while (sum>0) {
        sum=0;
        for (int i=0; i<8; i++) {
            cv::filter2D(src_w, src_w, CV_32FC1, kpw[i]);
            cv::filter2D(src_b, src_b, CV_32FC1, kpb[i]);
            cv::threshold(src_w, src_w, 2.99, 1.0, CV_THRESH_BINARY);
            cv::threshold(src_b, src_b, 2.99, 1.0, CV_THRESH_BINARY);
            cv::bitwise_and(src_w, src_b, src_w);
            sum += cv::sum(src_w).val[0];
            cv::bitwise_xor(src_f, src_w, src_f);
            src_f.copyTo(src_w);
            cv::threshold(src_f, src_b, 0.5, 1.0, CV_THRESH_BINARY_INV);
        }
    }
    src_f.copyTo(*dst);
    cv::imshow("Result", src_f);
    dst->convertTo(*dst,CV_8UC1, 255.0);
}
#endif
