/*
 * CamPoseCalculator.h
 *
 *  Created on: 06.12.2012
 *      Author: pwohlhart
 */

#ifndef CAMPOSECALCULATOR_H_
#define CAMPOSECALCULATOR_H_

#include "FaceStuffCalculator.h"

class CamPoseCalculator: public FaceStuffCalculator {
public:
	CamPoseCalculator();
	virtual ~CamPoseCalculator();

protected:
	void prepareSQLStuff();
	bool calcStuff(FaceData *faceData);
	void prepareQueryString();
	Pose calcCamPose(FaceData *faceData);

	SQLiteStmt *_insertPoseSqlStmt;
};

#endif /* CAMPOSECALCULATOR_H_ */
