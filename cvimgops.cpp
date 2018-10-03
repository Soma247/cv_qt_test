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
        if(binarizeOtsu())
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
        if(binarizeOtsu())
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
        if(binarizeOtsu())
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

bool cvImgOps::condDilate(int erode_elem, int erode_size,int dilate_elem,int dilate_size)
{
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

    cv::Mat tempimg;
    curimg->copyTo(tempimg);
    /*   if(morph(morph_type,morph_size,1)||
        dilate(dilate_elem,dilate_size)||
          cross(tempimg))return 1;*/

    cv::namedWindow("temp");
    cv::namedWindow("2");
    cv::namedWindow("3");
    cv::namedWindow("4");
    cv::imshow("temp",tempimg);

    erode(erode_type,erode_size);
    cv::imshow("2",*curimg);
    dilate(dilate_type,dilate_size);
    cv::imshow("3",*curimg);
    cross(tempimg);
    cv::imshow("4",*curimg);
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
    if(!pixcount||img.type()!=CV_8UC1)return 1;
    std::cout<<"cross"<<std::endl;
    int minwidth=img.size().width>curimg->size().width?curimg->size().width:img.size().width;
    int minheigth=img.size().height>curimg->size().height?curimg->size().height:img.size().height;

    for(int i=0;i<minheigth;i++)
        for(int j=0;j<minwidth;j++){
            curimg->at<uchar>(i,j)=(curimg->at<uchar>(i,j)== img.at<uchar>(i,j))?255:0;
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
    cv::Mat temp,eroded,skel(curimg->size(), CV_8UC1, cv::Scalar(0));

    if(dilate_size<2)dilate_size=2;
    if(erode_size<2)erode_size=2;
    cv::Mat dilelement = cv::getStructuringElement(dilate_type, cv::Size(dilate_size, dilate_size));
    cv::Mat erlelement = cv::getStructuringElement(erode_type, cv::Size(erode_size, erode_size));
    bool done;
    int iter=0;
    do{
        std::cout<<iter++<<std::endl;
        cv::erode(*curimg, eroded, erlelement);
        cv::dilate(eroded, temp, dilelement); // temp = open(img)
        cv::subtract(*curimg, temp, temp);
        cv::bitwise_or(skel, temp, skel);
        eroded.copyTo(*curimg);
        done = (cv::countNonZero(*curimg) == 0);
    } while (!done && iter<10000);
    skel.copyTo(*curimg);
    skel.release();
    temp.release();
    return 0;
}

bool cvImgOps::binarizeOtsu(){
    if(!pixcount)return 1;//blank img
    if(binarized)return 0;
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


bool cvImgOps::binarizeBradly(int wsize,double offset=0.0){
    //-------------init----------------------------------------------------
    if(!pixcount)return 1;//blank img
    if(binarized)return 0;
    if (curimg->type()!=CV_8UC1)toGrey();
    int imax=curimg->size().height,
            jmax=curimg->size().width,
            count=imax/wsize+(imax%wsize?1:0),
            wheight=imax/count,
            wwidth=jmax/count;
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
    int x1,x2,y1,y2,i(0),j(0),wpcount;
    ulong sum;
    int it=0;
    while(i<imax && j<jmax){

        x1=j;
        y1=i;
        if((x2=j+wwidth)>=jmax)x2=jmax-1;
        if((y2=i+wheight)>=imax)y2=imax-1;
        if((wpcount=(x2-x1)*(y2-y1))){
            sum=intmatr[y1][x1]+intmatr[y2][x2]-intmatr[y1][x2]-intmatr[y2][x1];
            std::cerr<<x1<<" "<<y1<<" "<<wpcount<<" "<<sum<<std::endl;
            std::cerr<<x2<<" "<<y2<<" "<<sum*(1.0+offset)/wpcount<<std::endl<<std::endl;
            for(int l=y1;l<=y2;l++)
                for(int c=x1;c<=x2;c++){
                    if(sum)
                        curimg->at<uchar>(l,c) = (static_cast<double>(curimg->at<uchar>(l,c))>=sum*(1.0+offset)/wpcount)? 255 : 0;//new binary
                    else
                        curimg->at<uchar>(l,c) = (static_cast<double>(curimg->at<uchar>(l,c))>sum*(1.0+offset)/wpcount)? 255 : 0;
                }
            if((j=x2+1)>=jmax){//to next line
                j=0;
                i=y2+1;
            }

            it++;
        }
        else break;
    }


    /*
    for(int i=0;i<imax;i++){//print integral (summ) matrix
        for(int j=0;j<jmax;j++)
            std::cout<<intmatr[i][j]<<" ";
        std::cout<<std::endl;
    }
*/

    //------------------free & ret-----------------------------------------
    for(int i=0;i<imax;i++)delete[] intmatr[i];
    delete[]intmatr;
    binarized=true;
    return 0;
}
