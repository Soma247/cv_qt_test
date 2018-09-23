#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdnoreturn.h>
#include "binarize.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read an image
    int *t=new int();
    int *dt=new int[3];
    delete t;
    delete[]dt;



    cv::Mat image = cv::imread("d://Qt//projects//cvtest//kqrehk93.jpg", 1),
            grimg,
            image_blurred_with_3x3_kernel;
    printf("-1\n");
    binarize nb(image);
    printf("-2\n");
    nb.binarizeOtsu();
printf("-3\n");
    // create image window named "My Image"
    cv::namedWindow("My Image");
    cv::namedWindow("window_gray");
    cv::namedWindow("window_bw");
    cv::namedWindow("window_blurred_with_5x5_kernel");

    // Show our images inside the created windows.

    // show the image on window
  //  cv::cvtColor(image,grimg,cv::COLOR_RGB2GRAY);

  //  cv::GaussianBlur(image, image_blurred_with_3x3_kernel, cv::Size(111, 111), 0);
    cv::imshow("window_bw",*nb.getimg());
    cv::imshow("My Image",image);

}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::on_pushButton_clicked(){
    exit(EXIT_SUCCESS);
}
