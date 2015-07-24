/*
 * FaceIDByCamPoseQuery.h
 *
 *  Created on: 04.03.2013
 *      Author: pwohlhart
 */

#ifndef FACEIDBYCAMPOSEQUERY_H_
#define FACEIDBYCAMPOSEQUERY_H_

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

#include "FaceDBQuery.h"

class FaceIDByCamPoseQuery: public FaceDBQuery {
public:
	FaceIDByCamPoseQuery();
	virtual ~FaceIDByCamPoseQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	void setAngles(float minYaw,float maxYaw,float minPitch,float maxPitch,float minRoll,float maxRoll);

	int recvData(int argc, char **argv, char **azColName);
	int dataRowReady(SQLiteDBConnection *sqlConn);

	vector<int> face_ids;

	void debugPrint();
private:
	float _minYaw;
	float _maxYaw;
	float _minPitch;
	float _maxPitch;
	float _minRoll;
	float _maxRoll;
};

#endif /* FACEIDBYCAMPOSEQUERY_H_ */
