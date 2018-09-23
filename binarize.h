#ifndef BINARIZE_H
#define BINARIZE_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

class binarize
{
private:
    cv::Mat *greyimg;
    unsigned int pixcount;
    void operator=(const binarize&){;}
    binarize(const binarize&){}

public:
    binarize();
    binarize(cv::Mat originalimg);
    ~binarize();
    bool binarizeOtsu();
    bool binarizeBradly(int size);
    bool setimg(cv::Mat img);

    unsigned int getpcount(){return pixcount;}
    cv::Mat *getimg(){return greyimg;}

};

#endif // BINARIZE_H
