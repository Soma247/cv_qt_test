#ifndef CVIMGOPS_H
#define CVIMGOPS_H
#include <QVector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>


class cvImgOps{
private:
    cv::Mat *curimg;
    QVector<double>hist;
    uchar histthr;
    unsigned int pixcount;
    void operator=(const cvImgOps&)=delete;
    cvImgOps(const cvImgOps&)=delete;
    bool binarized;

public:
    cvImgOps();
    cvImgOps(cv::Mat originalimg);
    ~cvImgOps();
    bool binarizeOtsu();
    bool binarizeBradly(int size, double offset);
    bool setimg(cv::Mat img);
    unsigned int getpcount(){return pixcount;}
    cv::Mat *getimg(){return curimg;}
    bool toGrey();
    bool GaussianBlur(cv::Size ksize, double sigmaX=0, double sigmaY=0, int borderType=cv::BORDER_DEFAULT);
    uchar gettermotsu(){return histthr;}
    QVector<double> gethist(){return hist;}
    cv::Size getsize(){return curimg->size();}

    bool dilate(int dilation_elem=0,int dilation_size=1);
    bool erode(int erosion_elem=0, int erosion_size=1);
    bool opening(int elem=0, int size=3);
    bool closing(int elem=0, int size=3);
    bool condDilate(int dil_elem=0, int dil_size=1, int er_elem=0, int er_size=1);
    bool combine(cv::Mat img);
    bool cross(cv::Mat img);
    bool to_skeleton(int erode_elem=0, int erode_size=1, int dilate_elem=0, int dilate_size=1);

};

#endif // CVIMGOPS
