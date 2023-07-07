#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv){
    Mat img=imread("apple.png");
    imshow("test", img);
    waitKey(0);
    return 0;
}