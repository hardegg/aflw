/*
 * FacePoseCalculator.h
 *
 *  Created on: 15.12.2011
 *      Author: pwohlhart
 */

#ifndef FACEPOSECALCULATOR_H_
#define FACEPOSECALCULATOR_H_

#include "FaceStuffCalculator.h"

class FacePoseCalculator: public FaceStuffCalculator {
public:
	FacePoseCalculator();
	virtual ~FacePoseCalculator();

protected:
	void prepareSQLStuff();
	bool calcStuff(FaceData *faceData);
	void prepareQueryString();
	FacePose calcPose(FaceData *faceData);

	SQLiteStmt *_insertPoseSqlStmt;
};

#endif /* FACEPOSECALCULATOR_H_ */
