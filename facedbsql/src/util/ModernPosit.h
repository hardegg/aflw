/*
 * ModernPosit.h
 *
 *  Created on: 22.10.2010
 *      Author: pwohlhart
 */

#ifndef MODERNPOSIT_H_
#define MODERNPOSIT_H_

#include <vector>
#include "cv.h"

class ModernPosit {
public:
	ModernPosit();
	virtual ~ModernPosit();

	void run(cv::Mat &rot, cv::Point3f &trans, std::vector<cv::Point2f> imagePts, std::vector<cv::Point3f> worldPts, double focalLength, cv::Point2f center = cv::Point2f(0.0,0.0), int maxIterations = 100);
};

#endif /* MODERNPOSIT_H_ */
