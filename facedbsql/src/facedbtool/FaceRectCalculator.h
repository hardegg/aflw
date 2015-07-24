/*
 * FaceRectCalculator.h
 *
 *  Created on: 17.11.2010
 *      Author: pwohlhart
 */

#ifndef FACERECTCALCULATOR_H_
#define FACERECTCALCULATOR_H_

#include "boost/program_options.hpp"
#include "cv.h"

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"
#include "../facedata/FeatureCoordTypes.h"
#include "../util/MeanFace3DModel.h"
#include "../util/ModernPosit.h"

#include "FaceStuffCalculator.h"

namespace po = boost::program_options;

class FaceRectCalculator : public FaceStuffCalculator {
public:
	FaceRectCalculator();
	virtual ~FaceRectCalculator();

	//void run(po::variables_map &vm, SQLiteDBConnection *sqlConn);
	
	cv::Rect calcSingleRect(FaceData *faceData,SQLiteDBConnection *sqlConn);

protected:
	void prepareSQLStuff();
	bool calcStuff(FaceData *faceData);
	void prepareQueryString();

	cv::Rect calcRect(FaceData *faceData);

	SQLiteStmt *_insertRectSqlStmt;
};

#endif /* FACERECTCALCULATOR_H_ */
