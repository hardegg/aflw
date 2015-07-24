/*
 * FaceEllipsesCalculator.h
 *
 *  Created on: 07.12.2010
 *      Author: pwohlhart
 */

#ifndef FACEELLIPSESCALCULATOR_H_
#define FACEELLIPSESCALCULATOR_H_

#include "boost/program_options.hpp"
#include "cv.h"
#include "highgui.h"

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"
#include "../facedata/FeatureCoordTypes.h"
#include "../util/MeanFace3DModel.h"
#include "../util/ModernPosit.h"

#include "FaceStuffCalculator.h"

namespace po = boost::program_options;

class FaceEllipsesCalculator : public FaceStuffCalculator {
public:
	FaceEllipsesCalculator();
	virtual ~FaceEllipsesCalculator();

protected:
	void prepareSQLStuff();
	bool calcStuff(FaceData *faceData);
	void prepareQueryString();

	bool calcEllipse(FaceData *faceData, FaceEllipse &ellipse);
	SQLiteStmt *_insertEllipseSqlStmt;

private:
	void showEllipse(FaceData *faceData, FaceEllipse ellipse);
};

#endif /* FACEELLIPSESCALCULATOR_H_ */
