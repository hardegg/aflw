/*
 * MeasureTime.h
 *
 *  Created on: 14.07.2011
 *      Author: pwohlhart
 */

#ifndef MEASURETIME_H_
#define MEASURETIME_H_

#include <cv.h>

#include <iostream>

using namespace std;


#define TIC(var) int64 var = cv::getTickCount();
#define TOC(var,msg) cout << msg << (cv::getTickCount()-var)/cv::getTickFrequency() << endl;

#endif /* MEASURETIME_H_ */
