/*
 * FaceRectViolaJonesCalculator.h
 *
 *  Created on: 18.08.2011
 *      Author: pwohlhart
 */

#ifndef FACERECTVIOLAJONESCALCULATOR_H_
#define FACERECTVIOLAJONESCALCULATOR_H_

#include <set>
#include <string>

#include "FaceStuffCalculator.h"
#include <cv.h>

using namespace std;


class FaceRectViolaJonesCalculator : public FaceStuffCalculator {
public:
	FaceRectViolaJonesCalculator();
	virtual ~FaceRectViolaJonesCalculator();

protected:
	void prepareSQLStuff();
	void prepareQueryString();
	bool calcStuff(FaceData *faceData);
	cv::Rect calcRect(FaceData *faceData);

	SQLiteStmt *_insertRectSqlStmt;
	cv::CascadeClassifier _detector;

	map<string,vector<cv::Rect> > _processedFiles;

	int _mcID;
	int _faceRectTypeVJ;

	vector<cv::Rect> getDetections(string fileName);
	bool getCorrectRect(FaceData *faceData,vector<cv::Rect> rects, cv::Rect &correctRect);
};

#endif /* FACERECTVIOLAJONESCALCULATOR_H_ */
