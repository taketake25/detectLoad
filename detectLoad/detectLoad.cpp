/**
 * g++ detectLoad.cpp `pkg-config --cflags --libs opencv` -lwiringPi -lpthread
 *
 **/

#include "common.h"
using namespace cv;
using namespace std;

Mat hsv2bin(Mat src, uchar param[2][3], bool drawparam);
void changeRangePamameter(int key, uchar param[2][3]);
Mat pickUpBiggestLine(Mat src);
void moveDirection(Mat src,int cols, int* x,int* y);

int main(void) {
	Mat src/*, hsv_copy*/, hsv, bin, small, maxonly;
	uchar param[2][3] = { {0,0,120},{180,255,255} };
	int x = 0, y = 0,count = 0;

	VideoCapture cap;
	cap.open(0);
	if (!cap.isOpened())return 1;

	cap >> src;
	//flip(src,src,-1);
	//imwrite("src.jpg",src);
	while (1) {
		cap >> src;
		//cout << "width:" << src.cols << "  height:" << src.rows << endl;

		resize(src, small, Size(src.cols / 5, src.rows / 5));
		flip(small,small,-1);
		cvtColor(small, hsv, CV_BGR2HSV);

		bin = hsv2bin(hsv,param,false);
		maxonly=pickUpBiggestLine(bin);
		Mat structElem = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(maxonly, maxonly,structElem);
		//imshow("maxonly", maxonly);

		moveDirection(maxonly, maxonly.cols, &x, &y);

		int key = waitKey(100);
		if (key == 'q') break;
		changeRangePamameter(key, param);
/*		
		count++;
		if(count>10){
			//imwrite("bin.jpg",bin);
			//imwrite("maxonly.jpg",maxonly);
			//imwrite("hsv.jpg",hsv);
			count=0;
			waitKey(1000);
		}
		*/
	}
	destroyAllWindows();

	return 0;
}

void changeRangePamameter(int key, uchar param[2][3]) {
	uchar p[2][3] = { { 0,0,120 },{ 100,255,255 } };
	switch (key) {
	case 'a':param = p;  break;
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
//	for(int i=0;i<3;i++) cout <<(int)param[0][i]<< " ";
//	for(int i=0;i<3;i++) cout <<(int)param[1][i]<<" ";
//	cout << endl;
}

Mat pickUpBiggestLine(Mat src) {
	double maxarea = 0, maxnum = 0;

	Mat dst;
	Mat mask = Mat::zeros(src.rows, src.cols, CV_8UC1);
	vector<vector<Point>> contours;

	findContours(src, contours, RETR_LIST, CHAIN_APPROX_NONE); //�֊s�𒊏o
	for (int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours.at(i));
		if (maxarea < area) { maxarea = area; maxnum = i; }
	}

	drawContours(mask, contours, maxnum, Scalar(255), -1);
	src.copyTo(dst, mask);
	return dst;
}

Mat hsv2bin(Mat src, uchar param[2][3], bool drawparam) {
	char str[13] = { 0 };
	Mat dst;
	inRange(src, Scalar(param[0][0], param[0][1], param[0][2]), Scalar(param[1][0], param[1][1], param[1][2]), dst);

	if (drawparam) {
		snprintf(str, 13, "%3d,%3d,%3d", param[0][0], param[0][1], param[0][2]);
		putText(dst, str, Point(dst.cols / 5, dst.rows * 3 / 5), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255));
		memset(str, '\0', sizeof(str) / sizeof(str[0]));
		snprintf(str, 13, "%3d,%3d,%3d", param[1][0], param[1][1], param[1][2]);
		putText(dst, str, Point(dst.cols / 5, dst.rows * 4 / 5), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255));
		memset(str, '\0', sizeof(str) / sizeof(str[0]));
	}
	return dst;
}

void moveDirection(Mat src,int cols, int* x, int* y) {
	vector<vector<uchar>> pickRows;
	pickRows = vector<vector<uchar>>(9, vector<uchar>(cols, 0));
	int avg[9] = { 0 }, sum = 0, sumval = 0;

	for (int i = 0; i < 9; i++) {
		sum = 0, sumval = 0;
		if (i == 8)pickRows[i] = src.row(0);
		else pickRows[i] = src.row((int)(src.rows * (8 - i) / 8) - 1);

		for (int j = 0; j < cols; j++) {
			int temp = pickRows[i][j]==0?0:1;
			cout << temp;
			sum += temp;
			sumval += j * temp;
		}cout << endl;
		if (sum == 0)avg[i] = 0;
		else if(sum>40)avg[i] = 0;
		else avg[i] = sumval / sum;
	}

	sum = 0; *y = 9;
	for (int i = 0; i < 9; i++) {
		if (avg[i] == 0) {*y = i;continue;}
		avg[i] -= cols / 2;
		sum += avg[i] * 8 / (i + 8);
	}
	if (*y == 0) *x = -1;
	else *x = sum / *y;
	cout << "x:" << *x << "  y:" << *y << endl;
}
