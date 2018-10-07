#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "cvimgops.h"
#include "asmOpenCV.h"
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btn_load_clicked();
    void on_btn_exit_clicked()__attribute__((noreturn));
    void on_btn_gaussianBlur_clicked();
    void on_btn_bin_otsu_clicked();
    void on_btn_bin_bradly_clicked();
    void on_btn_dilate_clicked();
    void on_btn_erode_clicked();
    void on_btn_closing_clicked();
    void on_btn_opening_clicked();
    void on_btn_cond_dilate_clicked();
    void on_btn_skelenonize_clicked();

private:
    QGraphicsScene *scene;
    cvImgOps image;
    Ui::MainWindow *ui;
    void printImg();
};

#endif // MAINWINDOW_H
