#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdnoreturn.h>
#include "binarize.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    cv::Mat image = cv::imread("d://Qt//projects//cvtest//kqrehk93.jpg", 1),
            image_blurred_with_5x5_kernel;
    cv::GaussianBlur(image, image_blurred_with_5x5_kernel, cv::Size(5, 5), 0,0);
    binarize nbo(image_blurred_with_5x5_kernel),
             nbb(image_blurred_with_5x5_kernel);

    nbo.binarizeOtsu();
    nbb.binarizeBradly(5,0.15);

    cv::namedWindow("Original");
    cv::namedWindow("gauss_blurred_with_5x5_kernel");
    cv::namedWindow("binarized_otsu");
    cv::namedWindow("binarized_bradly");

    cv::imshow("Original",image);
    cv::imshow("gauss_blurred_with_5x5_kernel",image_blurred_with_5x5_kernel);
    cv::imshow("binarized_otsu",*nbo.getimg());
    cv::imshow("binarized_bradly",*nbb.getimg());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked(){
    exit(EXIT_SUCCESS);
}
