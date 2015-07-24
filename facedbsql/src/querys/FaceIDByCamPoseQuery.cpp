/*
 * FaceIDByCamPoseQuery.cpp
 *
 *  Created on: 04.03.2013
 *      Author: pwohlhart
 */

#include "FaceIDByCamPoseQuery.h"

FaceIDByCamPoseQuery::FaceIDByCamPoseQuery() {

}

FaceIDByCamPoseQuery::~FaceIDByCamPoseQuery() {
}

bool FaceIDByCamPoseQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();
	stringstream querySStr;
	querySStr << "SELECT faces.face_id FROM faces,campose WHERE faces.face_id = campose.face_id AND ";
	querySStr << "camRoll >= " << _minRoll << " AND " << " camRoll <= " << _maxRoll << " AND ";
	querySStr << "camPitch >= " << _minPitch << " AND " << " camPitch <= " << _maxPitch << " AND ";
	querySStr << "camYaw >= " << _minYaw << " AND " << " camYaw <= " << _maxYaw;
	_query = querySStr.str();
	sqlConn->exec(_query,this);
	return true;
}

int FaceIDByCamPoseQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	int faceId;
	if (sqlConn->readIntColumn(0,faceId))
	{
		face_ids.push_back(faceId);
		return 0;
	}
	else
		return 1;
}

/*int FaceIDByPoseQuery::recvData(int argc, char **argv, char **azColName) {}*/

void FaceIDByCamPoseQuery::clear()
{
	face_ids.clear();
}

void FaceIDByCamPoseQuery::setAngles(float minYaw, float maxYaw, float minPitch, float maxPitch, float minRoll, float maxRoll)
{
	_minYaw = minYaw;
	_maxYaw = maxYaw;
	_minRoll = minRoll;
	_maxRoll = maxRoll;
	_minPitch = minPitch;
	_maxPitch = maxPitch;
}

void FaceIDByCamPoseQuery::debugPrint()
{
	cout << "Query: '" << _query << "'" << endl;
	cout << "Result: ";
	for (unsigned int i = 0; i < face_ids.size() ; ++i)
		cout << face_ids[i] << ", ";
	cout << endl;
}

