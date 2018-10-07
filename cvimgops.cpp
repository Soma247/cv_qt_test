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
    binarized=0;
    return 0;
}

bool cvImgOps::dilate(int dilation_elem, int dilation_size){//element 1(rect),2(cross),else(ellipse)
    if(!binarized)
        if(binarizeOtsu())
            return 1;

    int dilation_type;
    if(dilation_elem == 0) dilation_type = cv::MORPH_RECT;
    else if(dilation_elem == 1) dilation_type = cv::MORPH_CROSS;
    else dilation_type = cv::MORPH_ELLIPSE;

    cv::Mat element = getStructuringElement( dilation_type,
                                             cv::Size(2*dilation_size+1, 2*dilation_size+1),
                                             cv::Point(dilation_size, dilation_size));
    cv::dilate( *curimg, *curimg, element );
    return 0;
}

bool cvImgOps::erode(int erosion_elem, int erosion_size){//element 1(rect),2(cross),else(ellipse)

    if(!binarized)
        if(binarizeOtsu())
            return 1;

    int erosion_type;
    if(erosion_elem == 0) erosion_type = cv::MORPH_RECT;
    else if(erosion_elem == 1) erosion_type = cv::MORPH_CROSS;
    else erosion_type = cv::MORPH_ELLIPSE;

    cv::Mat element = getStructuringElement(erosion_type,
                                            cv::Size( 2*erosion_size+1, 2*erosion_size+1 ),
                                            cv::Point( erosion_size, erosion_size ) );
    cv::erode( *curimg, *curimg, element );
    return 0;
}

bool cvImgOps::opening(int element,int size){
    if(!binarized)
        if(binarizeOtsu())
            return 1;
    int type;
    switch (element) {
    case 0:
        type=cv::MORPH_RECT;
        break;
    case 1:
        type=cv::MORPH_CROSS;
        break;
    case 2:
    default://2
        type=cv::MORPH_ELLIPSE;
    }
    if(erode(type,size)||dilate(type,size))return 2;
    return 0;
}
bool cvImgOps::closing(int element,int size){
    if(!binarized)
        if(binarizeOtsu())
            return 1;
    int type;
    switch (element) {
    case 0:
        type=cv::MORPH_RECT;
        break;
    case 1:
        type=cv::MORPH_CROSS;
        break;
    case 2:
    default://2
        type=cv::MORPH_ELLIPSE;
    }
    if(dilate(type,size)||erode(type,size))return 2;
    return 0;
}

bool cvImgOps::condDilate(int dil_elem, int dil_size, int er_elem, int er_size)
{
    if(!binarized)
        if(binarizeOtsu())
            return 1;
    int dil_type,er_type;
    if(dil_elem == 0) dil_type = cv::MORPH_RECT;
    else if(dil_elem == 1) dil_type = cv::MORPH_CROSS;
    else dil_type = cv::MORPH_ELLIPSE;

    if(er_elem == 0) er_type = cv::MORPH_RECT;
    else if(er_elem == 1) er_type = cv::MORPH_CROSS;
    else er_type = cv::MORPH_ELLIPSE;

    namedWindow( "Display window", cv::WINDOW_NORMAL);
    imshow("Display window",*curimg);
    cv::Mat original,temp;
    curimg->copyTo(original);

    curimg->copyTo(temp);
    bool done;
      erode(er_type,er_size);
    do
    {
        done=true;
        curimg->copyTo(temp);
        dilate(dil_type,dil_size);
        if(cross(original))std::cerr<<"error cross"<<std::endl;
        for(int i=0;i<curimg->size().height && done==true;i++)
            for(int j=0;j<curimg->size().width && done==true;j++)
                if(curimg->at<uchar>(i,j)!=temp.at<uchar>(i,j))
                        done=false;
    } while (!done);
    return 0;
}

bool cvImgOps::combine(cv::Mat img){
    if(!pixcount||img.type()!=CV_8UC1)return 1;
    int minwidth=img.size().width>curimg->size().width?curimg->size().width:img.size().width;
    int minheigth=img.size().height>curimg->size().height?curimg->size().height:img.size().height;

    for(int i=0;i<minheigth;i++)
        for(int j=0;j<minwidth;j++)
            curimg->at<uchar>(i,j)=(curimg->at<uchar>(i,j)||img.at<uchar>(i,j))?255:0;
    return 0;
}

bool cvImgOps::cross(cv::Mat img)
{
    if(curimg->size()!=img.size())return 1;
    if(!pixcount||img.type()!=CV_8UC1)return 2;

    if(!binarized)
        if(binarizeOtsu())
            return 3;



    for(int i=0;i<img.size().height;i++)
        for(int j=0;j<img.size().width;j++){
            if(curimg->at<uchar>(i,j)==255&&curimg->at<uchar>(i,j)!=img.at<uchar>(i,j))
                curimg->at<uchar>(i,j)=0;
        }
    return 0;
}

bool cvImgOps::to_skeleton(int erode_elem, int erode_size, int dilate_elem, int dilate_size){
    if(!binarized)
        if(binarizeOtsu())
            return 1;
    int erode_type,dilate_type;
    if(erode_elem == 0) erode_type = cv::MORPH_RECT;
    else if(erode_elem == 1) erode_type = cv::MORPH_CROSS;
    else erode_type = cv::MORPH_ELLIPSE;
    if(dilate_elem == 0) dilate_type = cv::MORPH_RECT;
    else if(dilate_elem == 1) dilate_type = cv::MORPH_CROSS;
    else dilate_type = cv::MORPH_ELLIPSE;
    cv::Mat dilated,eroded,skel(curimg->size(), CV_8UC1, cv::Scalar(0));

    if(dilate_size<2)dilate_size=2;
    if(erode_size<2)erode_size=2;
    cv::Mat dilelement = cv::getStructuringElement(dilate_type, cv::Size(dilate_size, dilate_size));
    cv::Mat erlelement = cv::getStructuringElement(erode_type, cv::Size(erode_size, erode_size));
    bool done;
    cv::imshow("name",skel);
    do
    {
        cv::erode(*curimg, eroded, erlelement);//erode curimg to eroded
        cv::dilate(eroded, dilated, dilelement); // dilate eroded to dilated
        cv::subtract(*curimg, dilated, dilated);//curimg-dilated =>dilated
        cv::bitwise_or(skel, dilated, skel);//skel | dilated =>skel
        eroded.copyTo(*curimg);//eroded to curimg
        done = (cv::countNonZero(*curimg) == 0);
    } while (!done);

    skel.copyTo(*curimg);
    skel.release();
    dilated.release();
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
    if(!ter)ter=1;
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

            wheight=wsize,
            wwidth=wsize;
    std::cerr<<imax<<" "<<jmax<<" "<<wwidth<<" "<<wheight<<std::endl;
    ulong ** intmatr=new ulong*[imax];
    cv::Mat intmatrim;
    curimg->copyTo(intmatrim);

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
    int x1,x2,y1,y2,wpcount;
    ulong sum;
    for(y1=0;y1<imax-1;y1++){
        if((y2=y1+wheight)>=imax)y2=imax-1;
        for(x1=0;x1<jmax-1;x1++){
            if((x2=x1+wwidth)>=jmax)x2=jmax-1;

            if((wpcount=(x2-x1)*(y2-y1)) && x2>x1 && y2>y1)
                if((sum=intmatr[y1][x1]+intmatr[y2][x2]-intmatr[y1][x2]-intmatr[y2][x1]) && sum<255ul*wpcount){
                    curimg->at<uchar>(y1,x1) = (static_cast<double>(curimg->at<uchar>(y1,x1))>(sum*(1-offset)/wpcount))? 255 : 0;
                }
        }
    }

    //------------------free & ret-----------------------------------------
    for(int i=0;i<imax;i++)delete[] intmatr[i];
    delete[]intmatr;
    binarized=true;
    return 0;
}
