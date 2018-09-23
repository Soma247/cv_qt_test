#include "binarize.h"
#include <qdebug.h>
binarize::binarize(){
    pixcount=0;
    greyimg=new cv::Mat();
}

binarize::binarize(cv::Mat originalimg){
    pixcount=static_cast<unsigned int>(originalimg.size().width*originalimg.size().height);
    greyimg=new cv::Mat();
    if(pixcount>0)
        cv::cvtColor(originalimg,*greyimg,cv::COLOR_RGB2GRAY);
    else binarize();
}


binarize::~binarize(){
    if(greyimg!=nullptr)delete greyimg;
    //  greyimg=nullptr;
}

bool binarize::setimg(cv::Mat img){
    if(greyimg!=nullptr)delete greyimg;
    pixcount=static_cast<unsigned int>(img.size().width*img.size().height);
    greyimg=new cv::Mat();
    if(!pixcount)return 1;
        cv::cvtColor(img,*greyimg,cv::COLOR_RGB2GRAY);
    return 0;
}


bool binarize::binarizeOtsu(){
    if(!pixcount)return 1;//blank img
    int imax=greyimg->size().height,
            jmax=greyimg->size().width,
            size=256,
            tsize=size+1;

    unsigned int *h=new unsigned int[size];//hist arr pointer

    for(unsigned *p=h+tsize-1 ; p>=h;p--)
        *p=0;
    for(int i=0;i<imax;i++)
        for(int j=0;j<jmax;j++)
            ++h[greyimg->at<uchar>(i,j)];

    unsigned long m(0),n(0);//summ
    for(int i=0;i<size;i++){
        m+=static_cast<unsigned long>(i)*h[i];
        n+=h[i];
    }

    long double sigMax(-1),w1,sig,dif_a;//disp. max,probability,curr disp, diff. of averages
    unsigned long summcnt(0),summcnthavg(0);//for class1: sum of count of pix(brightness)*brightness, count of all pix
    unsigned char ter(0);

    for(int i=0;i<size;i++){//for all brightnesses
        summcnt+=static_cast<unsigned long>(i)*h[i];
        summcnthavg+=h[i];
        if(!summcnthavg)continue;
        w1=static_cast<long double>(summcnthavg)/n;
        dif_a=(static_cast<long double>(summcnt)/summcnthavg)-static_cast<long double>(m-summcnt)/(n-summcnthavg);
        sig=w1*(1-w1)*dif_a*dif_a;
        if(sig>sigMax){
            sigMax=sig;
            ter=static_cast<uchar>(i);//new threshold
        }
    }

    for(int i=0;i<imax;i++)
        for(int j=0;j<jmax;j++)
            greyimg->at<uchar>(i,j) = (greyimg->at<uchar>(i,j)>=ter)? 255 : 0;//new binary img
    delete[]h;
    return 0;
}


bool binarize::binarizeBradly(int wsize){
    //-------------init----------------------------------------------------
    if(!pixcount)return 1;//blank img
    int imax=greyimg->size().height,
        jmax=greyimg->size().width,
        count=imax/wsize+imax%wsize?1:0,
        wheight=imax/count,
        wwidth=jmax/count;

    ulong ** intmatr=new ulong*[imax];
    for(int i=0;i<imax;i++)
        intmatr[i]=new ulong[jmax];
    **intmatr=0;
    //-----------------first line and col----------------------------------
    for (int i=1;i<imax;i++)
        intmatr[i][0]=intmatr[i-1][0]+static_cast<ulong>(greyimg->at<uchar>(i,0));
    for (int j=1;j<jmax;j++)
        intmatr[0][j]=intmatr[0][j-1]+static_cast<ulong>(greyimg->at<uchar>(0,j));
    //-----------------fill intmatr----------------------------------------
    for (int i=1;i<imax;i++)
        for(int j=1;j<jmax;j++)
            intmatr[i][j]=static_cast<ulong>(greyimg->at<uchar>(i,j))
                           + intmatr[i-1][j]
                            + intmatr[i][j-1]
                             - intmatr[i-1][j-1];
    //---------------------------------------------------------------------
    int x1,x2,y1,y2,i(0),j(0);
    while(i<imax && j<jmax){
          x1=j;
          y1=i;
          if((x2=j+wwidth)>=jmax)x2=jmax-1;
          if((y2=i+wheight)>=imax)y2=imax-1;





          if((j=x2+1)>=jmax){j=0;i=y2+1;}
        }


    //------------------free & ret-----------------------------------------
    for(int i=0;i<imax;i++)delete[] intmatr[i];
    delete[]intmatr;
    return 0;
}
