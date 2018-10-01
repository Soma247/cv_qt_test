#include "cvimgops.h"
#include <qdebug.h>
cvImgOps::cvImgOps(){
    pixcount=0;
    curimg=new cv::Mat();
    binarized=false;
}

cvImgOps::cvImgOps(cv::Mat originalimg){
    pixcount=static_cast<unsigned int>(originalimg.size().width*originalimg.size().height);
    if(pixcount)curimg=new cv::Mat(originalimg);
    else curimg=new cv::Mat();
    binarized=false;
}


cvImgOps::~cvImgOps(){
    if(curimg!=nullptr){curimg->release();delete curimg;}
    pixcount=0;

}

bool cvImgOps::setimg(cv::Mat img){
    if(curimg!=nullptr){curimg->release();delete curimg;}
    pixcount=static_cast<unsigned int>(img.size().width*img.size().height);
    if(!pixcount)return 1;
    curimg=new cv::Mat(img);
    binarized=false;
    return 0;
}
bool cvImgOps::toGrey(){
    if(!pixcount)return 1;//blank img
    if (curimg->type()!=CV_8UC1){
        cv::cvtColor(*curimg,*curimg,cv::COLOR_RGB2GRAY);
    }
    return 0;
}

bool cvImgOps::GaussianBlur(cv::Size ksize, double sigmaX, double sigmaY, int borderType){
    if(toGrey())return 1;
    cv::GaussianBlur(*curimg,*curimg, ksize, sigmaX,sigmaY,borderType);
    return 0;
}

bool cvImgOps::dilate(int dilation_elem, int dilation_size){//element 1(rect),2(cross),else(ellipse)
    if(!binarized)
        if(binarizeBradly(5,0.15))
            return 1;

    int dilation_type;
    if(dilation_elem == 0) dilation_type = cv::MORPH_RECT;
    else if(dilation_elem == 1) dilation_type = cv::MORPH_CROSS;
    else dilation_type = cv::MORPH_ELLIPSE;

    cv::Mat element = getStructuringElement( dilation_type,
                                             cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                             cv::Point( dilation_size, dilation_size ) );
    cv::dilate( *curimg, *curimg, element );
    return 0;
}

bool cvImgOps::erode(int erosion_elem, int erosion_size){//element 1(rect),2(cross),else(ellipse)

    if(!binarized)
        if(binarizeBradly(5,0.15))
            return 1;

    int erosion_type;
    if(erosion_elem == 0) erosion_type = cv::MORPH_RECT;
    else if(erosion_elem == 1) erosion_type = cv::MORPH_CROSS;
    else erosion_type = cv::MORPH_ELLIPSE;

    cv::Mat element = getStructuringElement(erosion_type,
                                            cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                            cv::Point( erosion_size, erosion_size ) );
    cv::erode( *curimg, *curimg, element );
    return 0;
}

bool cvImgOps::morph(int morph_elem, int morph_size, int operation){//element 1(rect),2(cross),else(ellipse); ops 0-4 :op.close,grad,tophat,blhat
    if(!binarized)
        if(binarizeBradly(5,0.15))
            return 1;

    // Since MORPH_X : 2,3,4,5 and 6

    int morph_type;
    if(morph_elem == 0) morph_type = cv::MORPH_RECT;
    else if(morph_elem == 1) morph_type = cv::MORPH_CROSS;
    else morph_type = cv::MORPH_ELLIPSE;

    operation+=2;
    cv::Mat element = getStructuringElement( morph_type,
                                             cv::Size( 2*morph_size + 1, 2*morph_size+1 ),
                                             cv::Point( morph_size, morph_size ) );
    morphologyEx(*curimg,*curimg, operation, element );
    return 0;

}

bool cvImgOps::binarizeOtsu(){
    if(!pixcount)return 1;//blank img
    if (curimg->type()!=CV_8UC1)toGrey();

    int imax=curimg->size().height,
            jmax=curimg->size().width,
            size=256;
    hist.clear();
    hist.resize(size);
    for(int i=0;i<imax;i++)
        for(int j=0;j<jmax;j++)
            ++hist[curimg->at<uchar>(i,j)];

    unsigned long m(0),n(0);//summ
    for(int i=0;i<size;i++){
        m+=static_cast<unsigned long>(i)*hist[i];
        n+=hist[i];
    }

    long double sigMax(-1),w1,sig,dif_a;//disp. max,probability,curr disp, diff. of averages
    unsigned long summcnt(0),summcnthavg(0);//for class1: sum of count of pix(brightness)*brightness, count of all pix
    unsigned char ter(0);

    for(int i=0;i<size;i++){//for all brightnesses
        summcnt+=static_cast<unsigned long>(i)*hist[i];
        summcnthavg+=hist[i];
        if(!summcnthavg)continue;
        w1=static_cast<long double>(summcnthavg)/n;
        dif_a=(static_cast<long double>(summcnt)/summcnthavg)-static_cast<long double>(m-summcnt)/(n-summcnthavg);
        sig=w1*(1-w1)*dif_a*dif_a;
        if(sig>sigMax){
            sigMax=sig;
            ter=static_cast<uchar>(i);//new threshold
        }
    }
    histthr=ter;
    for(int i=0;i<imax;i++)
        for(int j=0;j<jmax;j++)
            curimg->at<uchar>(i,j) = (curimg->at<uchar>(i,j)>=ter)? 255 : 0;//new binary img
    binarized=true;
    return 0;
}


bool cvImgOps::binarizeBradly(int wsize,double offset=0.15){
    //-------------init----------------------------------------------------
    if(!pixcount)return 1;//blank img
    if (curimg->type()!=CV_8UC1)toGrey();
    int imax=curimg->size().height,
            jmax=curimg->size().width,
            count=imax/wsize+imax%wsize?1:0,
            wheight=imax/count,
            wwidth=jmax/count;

    ulong ** intmatr=new ulong*[imax];
    for(int i=0;i<imax;i++)
        intmatr[i]=new ulong[jmax];
    **intmatr=0;
    //-----------------first line and col----------------------------------
    for (int i=1;i<imax;i++)
        intmatr[i][0]=intmatr[i-1][0]+static_cast<ulong>(curimg->at<uchar>(i,0));
    for (int j=1;j<jmax;j++)
        intmatr[0][j]=intmatr[0][j-1]+static_cast<ulong>(curimg->at<uchar>(0,j));
    //-----------------fill intmatr----------------------------------------
    for (int i=1;i<imax;i++)
        for(int j=1;j<jmax;j++)
            intmatr[i][j]=static_cast<ulong>(curimg->at<uchar>(i,j))
                    + intmatr[i-1][j]
                    + intmatr[i][j-1]
                    - intmatr[i-1][j-1];
    //---------------------------------------------------------------------
    int x1,x2,y1,y2,i(0),j(0),wpcount;
    ulong sum;
    while(i<imax && j<jmax){
        x1=j;
        y1=i;
        if((x2=j+wwidth)>=jmax)x2=jmax-1;
        if((y2=i+wheight)>=imax)y2=imax-1;
        wpcount=(x2-x1)*(y2-y1);
        sum=intmatr[y1][x1]+intmatr[y2][x2]-intmatr[y1][x2]-intmatr[y2][x1];
        for(int l=y1;l<=y2;l++)
            for(int c=x1;c<=x2;c++)
                curimg->at<uchar>(l,c) = (static_cast<double>(curimg->at<uchar>(l,c))>=sum*(1.0+offset)/wpcount)? 255 : 0;//new binary img
        if((j=x2+1)>=jmax){//to next line
            j=0;
            i=y2+1;
        }
    }


    //------------------free & ret-----------------------------------------
    for(int i=0;i<imax;i++)delete[] intmatr[i];
    delete[]intmatr;
    binarized=true;
    return 0;
}
