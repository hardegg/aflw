/*
 * FaceStuffCalculator.h
 *
 *  Created on: 07.12.2010
 *      Author: pwohlhart
 */

#ifndef FACESTUFFCALCULATOR_H_
#define FACESTUFFCALCULATOR_H_

#include "boost/program_options.hpp"
#include "cv.h"

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"
#include "../querys/FaceIDQuery.h"
#include "../querys/FaceDataByIDsQuery.h"
#include "../facedata/FeatureCoordTypes.h"
#include "../util/MeanFace3DModel.h"
#include "../util/ModernPosit.h"

#include "FaceStuffCalculator.h"

namespace po = boost::program_options;

class FaceStuffCalculator {
public:
	FaceStuffCalculator();
	virtual ~FaceStuffCalculator();

	virtual void run(po::variables_map &vm, SQLiteDBConnection *sqlConn);
protected:
	virtual void prepareSQLStuff() = 0;
	virtual bool calcStuff(FaceData *faceData) = 0;
	virtual void prepareQueryString() = 0;

	bool calcCenterScaleAndUp(FaceData *faceData, double normEyeDist, cv::Point2f &center, double &scale, cv::Point2f &upv, FaceCenterTypes center_type);
	bool getPose(FaceData *faceData, double focalLength, cv::Point2f imgCenter, cv::Mat &rot, cv::Point3f &trans, bool ogl = false);
	bool calcUpvFromEyes(FaceData *faceData, cv::Point2f &upv);
	void splitRotationMatrixToRollPitchYaw(cv::Mat &rot_matrix, double& roll, double& pitch, double& yaw);

	cv::Point2f project(cv::Point3f pt, double focalLength, cv::Point2f imgCenter);
	cv::Point3f transform(cv::Point3f pt, cv::Mat rot, cv::Point3f trans);

	cv::Mat rotMatX(double alpha);
	cv::Mat rotMatY(double alpha);
	cv::Mat rotMatZ(double alpha);

	int getCalcFromManuallyLabeledFeaturePointsAnnotationTypeID();
	int getAnnotationTypeIDByCode(string code);

	int _lelcID;
	int _lercID;
	int _rercID;
	int _relcID;
	int _mcID;

	std::string _basePath;
	std::string _faceQueryString;
	FeatureCoordTypes _fcTypes;
	SQLiteDBConnection *_sqlConn;
	MeanFace3DModel _meanFace3DModel;

	bool _useTransaction;
	int manAnnotID;
};

#endif /* FACESTUFFCALCULATOR_H_ */
