/*
 * FaceFeaturesDisplay.h
 *
 *  Created on: 02.08.2010
 *      Author: pwohlhart
 */

#ifndef FACEFEATURESDISPLAY_H_
#define FACEFEATURESDISPLAY_H_

#define BOOST_FILESYSTEM_VERSION 3

#include "boost/program_options.hpp"
#include "cv.h"

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"



namespace po = boost::program_options;


class FaceFeaturesDisplay {
public:
	FaceFeaturesDisplay();
	virtual ~FaceFeaturesDisplay();

	void run(po::variables_map &vm, SQLiteDBConnection *sqlConn);
private:
	void paintFaceData(FaceData *faceData, cv::Mat &img);
	void paintEllipse(FaceData *faceData, cv::Mat &img);
	void paintRectangles(FaceData *faceData, cv::Mat &img);
};

#endif /* FACEFEATURESDISPLAY_H_ */
