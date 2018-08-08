#pragma warning(disable : 4819)
#include<opencv2/highgui.hpp>  //openCV
#include<opencv2/imgproc.hpp>  //openCV
#pragma warning(default : 4819)
#include <vector>
#include <iostream>

#ifdef _DEBUG //Debug
#pragma comment(lib,"opencv_world320d.lib")
#else//Release
#pragma comment(lib,"opencv_world320.lib")
#endif
