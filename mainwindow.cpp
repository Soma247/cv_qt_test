#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read an image
    cv::Mat image = cv::imread("d://Qt//projects//cvtest//kqrehk93.jpg", 1),
            grimg,
            image_blurred_with_3x3_kernel;

    // create image window named "My Image"
    cv::namedWindow("My Image");
    cv::namedWindow("window_gray");
    cv::namedWindow("window_blurred_with_5x5_kernel");

    // Show our images inside the created windows.

    // show the image on window
    cv::cvtColor(image,grimg,cv::COLOR_RGB2GRAY);

    cv::GaussianBlur(image, image_blurred_with_3x3_kernel, cv::Size(111, 111), 0);

    uint imax=grimg.size().height;
    uint jmax=grimg.size().width;

    uint**integm=new uint*[imax];
    for(int i=0;i<imax;i++)
        integm[i]=new uint[jmax];

    std::cout<<imax<<" "<<jmax;
    for(uint i=0;i<imax;i++)
        for(uint j=0;j<jmax;j++){
            if(grimg.at<uchar>(i,j)<255/2)
                grimg.at<uchar>(i,j)=0;
            else grimg.at<uchar>(i,j)=255;
        }

    cv::imshow("window_blurred_with_5x5_kernel", image_blurred_with_3x3_kernel);
    cv::imshow("window_gray", grimg);


}

MainWindow::~MainWindow()
{
    delete ui;
}
