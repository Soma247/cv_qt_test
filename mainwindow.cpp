#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdnoreturn.h>
#include <qpixmap.h>
#include <QFileDialog>
#include "skel_by_eichiiro_momma.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->hst_otsu->hide();//hist hide
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::printImg(){
    scene = new QGraphicsScene (this);
    QPixmap pixmap = cvMatToQPixmap(*image.getimg());
    scene->addPixmap(pixmap);
    QRectF r = scene->sceneRect();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->fitInView(r, Qt::KeepAspectRatio);
    ui->graphicsView->repaint();
}


void MainWindow::on_btn_load_clicked(){
    ui->hst_otsu->hide();//hist hide
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "../cvtest", tr("Image Files (*.jpg *.bmp *.png)"));
    if(filename.isEmpty()||filename.isNull())return;
    image.setimg(cv::imread(filename.toStdString(), 1));
    ui->spb_bradly_sz->setValue(image.getimg()->size().width/16);
    this->printImg();
}

void MainWindow::on_btn_exit_clicked(){
    cvDestroyAllWindows();
    exit(EXIT_SUCCESS);
}

void MainWindow::on_btn_gaussianBlur_clicked(){
    ui->hst_otsu->hide();//hist hide
    image.GaussianBlur(cv::Size(5,5),0,0);
    this->printImg();
}

void MainWindow::on_btn_bin_otsu_clicked(){
    image.binarizeOtsu();
    ui->hst_otsu->clearGraphs();
    ui->hst_otsu->clearPlottables();
    ui->hst_otsu->clearItems();
    //-------histogramm--------------------
    ui->hst_otsu->show();
    QVector<double> keys;
    QVector<double> termdata;
    QVector<double> histdata=image.gethist();
    double maxhist{.0};
    termdata.resize(256);
    keys.resize(256);

    for(int i=0;i<histdata.size();i++){
        keys[i]=i;
        if(maxhist<histdata[i])maxhist=histdata[i];
    }
    termdata[image.gettermotsu()]=maxhist;
    //----------hist bars & cfgn------------
    QCPBars *bars1 = new QCPBars(ui->hst_otsu->xAxis, ui->hst_otsu->yAxis);//hist
    bars1->setWidth(1);
    bars1->setData(keys,image.gethist());
    bars1->setPen(Qt::NoPen);
    bars1->setBrush(QColor(10, 140, 70, 160));
    QCPBars *bars2 = new QCPBars(ui->hst_otsu->xAxis, ui->hst_otsu->yAxis);//threshold
    bars2->setWidth(3);
    bars2->setData(keys,termdata);
    bars2->setPen(Qt::NoPen);
    bars2->setBrush(QColor(Qt::red));
    ui->hst_otsu->xAxis->setRange(0,256);
    ui->hst_otsu->yAxis->setRange(0,maxhist);
    ui->hst_otsu->axisRect()->setupFullAxesBox();
    ui->hst_otsu->replot();
    //-------------------------------------	
    this->printImg();
    this->repaint();
}

void MainWindow::on_btn_bin_bradly_clicked(){
    ui->hst_otsu->hide();//hist hide
    cv::Size imsz=image.getsize();
    int bsize=imsz.height>imsz.width?imsz.width:imsz.height;
    int spbsz=ui->spb_bradly_sz->value();
    bsize=bsize>spbsz?spbsz:bsize;
    if(!bsize%2)bsize--;
    if(bsize!=spbsz)ui->spb_bradly_sz->setValue(bsize);
    if(image.binarizeBradly(bsize,ui->dsb_bradly_ofst->value()))std::cerr<<"bradly_error"<<std::endl;
    this->printImg();
}

void MainWindow::on_btn_dilate_clicked(){
    ui->hst_otsu->hide();//hist hide
    cv::Size imgsize=image.getsize();
        int minsize=imgsize.height<imgsize.width?imgsize.height:imgsize.width;
    image.dilate(ui->spb_dilate_elem->value(),ui->spb_dilate_size->value()>minsize?minsize:ui->spb_dilate_size->value());
    this->printImg();
}

void MainWindow::on_btn_erode_clicked(){
    ui->hst_otsu->hide();//hist hide
    cv::Size imgsize=image.getsize();
        int minsize=imgsize.height<imgsize.width?imgsize.height:imgsize.width;
    image.erode(ui->spb_erode_elem->value(),ui->spb_erode_size->value()>minsize?minsize:ui->spb_erode_size->value());
    this->printImg();
}

void MainWindow::on_btn_closing_clicked(){
    ui->hst_otsu->hide();//hist hide
    cv::Size imgsize=image.getsize();
       int minsize=imgsize.height<imgsize.width?imgsize.height:imgsize.width;
    image.closing(ui->spb_morph_elem->value(),ui->spb_morph_size->value()>minsize?minsize:ui->spb_morph_size->value());
    this->printImg();
}

void MainWindow::on_btn_opening_clicked(){
    ui->hst_otsu->hide();//hist hide
    cv::Size imgsize=image.getsize();
       int minsize=imgsize.height<imgsize.width?imgsize.height:imgsize.width;
    image.opening(ui->spb_morph_elem->value(),ui->spb_morph_size->value()>minsize?minsize:ui->spb_morph_size->value());
    this->printImg();
}

void MainWindow::on_btn_cond_dilate_clicked(){
    ui->hst_otsu->hide();//hist hide

    cv::Size imgsize=image.getsize();
    int minsize=imgsize.height<imgsize.width?imgsize.height:imgsize.width;
    image.condDilate(ui->spb_dilate_elem->value(),ui->spb_dilate_size->value()>minsize?minsize:ui->spb_dilate_size->value(),
                     ui->spb_erode_elem->value(),ui->spb_erode_size->value()>minsize?minsize:ui->spb_erode_size->value()
                     );
    this->printImg();
}

void MainWindow::on_btn_skelenonize_clicked(){
      image.GaussianBlur(cv::Size(11,11),0,0);
      image.binarizeOtsu();
    ui->hst_otsu->hide();//hist hide
    /*
    cv::Size imgsize=image.getsize();
    int maxsize=imgsize.height>imgsize.width?imgsize.height:imgsize.width;
    image.to_skeleton(ui->spb_erode_elem->value(),
                     ui->spb_erode_size->value()>maxsize?maxsize:ui->spb_erode_size->value(),
                     ui->spb_dilate_elem->value(),
                     ui->spb_dilate_size->value()>maxsize?maxsize:ui->spb_dilate_size->value());*/
    skel_by_eiichiro_momma(image.getimg(),image.getimg());
    this->printImg();
}
