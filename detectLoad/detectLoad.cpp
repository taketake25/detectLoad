#include "common.h"
//#include 
using namespace cv;
using namespace std;

Mat hsv2bin(Mat src,Mat dst,uchar param[2][3],bool drawparam = false);
void changeRangePamameter(int key, uchar param[2][3]);

int main(void) {
	Mat src, hsv_copy, hsv, bin, small;
	uchar param[2][3] = { {0,0,120},{100,256,256} };

	VideoCapture cap;
	cap.open(1);
	if (!cap.isOpened())return 1;

	while (1) {
		cap >> src;
		resize(src, small, Size(src.cols / 5, src.rows / 5));
		cvtColor(small, hsv, CV_BGR2HSV);
		hsv_copy = hsv.clone();

		hsv2bin(hsv,bin,param,false);

		//		Mat structElem = getStructuringElement(MORPH_RECT, Size(2, 2));
		//		dilate(bin, bin, structElem);
		//		morphologyEx(bin, bin, MORPH_GRADIENT, structElem); //ó÷äsâÊëúÇ…Ç∑ÇÈ

		vector<vector<Point>> contours;
		findContours(bin, contours, RETR_LIST, CHAIN_APPROX_NONE); //ó÷äsÇíäèo
		double maxarea=0,maxnum=0;
		for (int i = 0; i < contours.size(); i++) {
			double area = contourArea(contours.at(i));
			//cout << area << endl;
			if (maxarea < area) { maxarea = area; maxnum = i; }
		}
		
		Mat mask = Mat::zeros(bin.rows, bin.cols, CV_8UC1);
		drawContours(mask, contours, maxnum, Scalar(255), -1);

		Mat maxonly;
		bin.copyTo(maxonly, mask);
//		imshow("binary", bin);
		imshow("maxonly", maxonly);

		int key = waitKey(20);
		if (key == 'q') break;
		changeRangePamameter(key, param);
	}
	destroyAllWindows();

	return 0;
}

Mat hsv2bin(Mat src, Mat dst, uchar param[2][3],bool drawparam = false) {
	char str[13] = { 0 };
	inRange(src, Scalar(param[0][0], param[0][1], param[0][2]), Scalar(param[1][0], param[1][1], param[1][2]), src);

	if(drawparam){
		snprintf(str, 13, "%3d,%3d,%3d", param[0][0], param[0][1], param[0][2]);
		putText(dst, str, Point(dst.cols / 5, dst.rows*3 / 5), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255));
		memset(str, '\0', sizeof(str) / sizeof(str[0]));
		snprintf(str, 13, "%3d,%3d,%3d", param[1][0], param[1][1], param[1][2]);
		putText(dst, str, Point(dst.cols / 5, dst.rows*4 / 5), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255));
		memset(str, '\0', sizeof(str) / sizeof(str[0]));
	}
}

void changeRangePamameter(int key, uchar param[2][3]) {
	switch (key) {
	case 'w':if (param[0][0] - 5 >= 0)         param[0][0] -= 5; break;
	case 'e':if (param[0][0] + 5 < param[1][0])param[0][0] += 5; break;
	case 's':if (param[0][1] - 5 >= 0)         param[0][1] -= 5; break;
	case 'd':if (param[0][1] + 5 < param[1][1])param[0][1] += 5; break;
	case 'x':if (param[0][2] - 5 >= 0)         param[0][2] -= 5; break;
	case 'c':if (param[0][2] + 5 < param[1][2])param[0][2] += 5; break;

	case 'r':if (param[1][0] - 5 > param[0][0])param[1][0] -= 5; break;
	case 't':if (param[1][0] + 5 <= 180)       param[1][0] += 5; break;
	case 'f':if (param[1][1] - 5 > param[0][1])param[1][1] -= 5; break;
	case 'g':if (param[1][1] + 5 <= 256)       param[1][1] += 5; break;
	case 'v':if (param[1][2] - 5 > param[0][2])param[1][2] -= 5; break;
	case 'b':if (param[1][2] + 5 <= 256)       param[1][2] += 5; break;
	}
}
