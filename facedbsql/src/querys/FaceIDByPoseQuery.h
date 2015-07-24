/*
 * FaceIDByPoseQuery.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEIDBYPOSEQUERY_H_
#define FACEIDBYPOSEQUERY_H_

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

#include "FaceDBQuery.h"

// TODO: convert to FaceIDQuery

class FaceIDByPoseQuery: public FaceDBQuery {
public:
	FaceIDByPoseQuery();
	virtual ~FaceIDByPoseQuery();

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

#endif /* FACEIDBYPOSEQUERY_H_ */
