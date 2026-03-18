#ifndef _IMAGE_ANALYZE_H_
#define _IMAGE_ANALYZE_H_

#include <opencv2/opencv.hpp>

extern CarInfo carinfo;

int Image_Car_detect(cv::Mat input_image);

#endif
