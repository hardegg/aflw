/*
 * FaceIDByPoseQuery.cpp
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#include "FaceIDByPoseQuery.h"

FaceIDByPoseQuery::FaceIDByPoseQuery()
{
}

FaceIDByPoseQuery::~FaceIDByPoseQuery()
{
}

bool FaceIDByPoseQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();
	stringstream querySStr;
	querySStr << "SELECT faces.face_id FROM faces,facepose WHERE faces.face_id = facepose.face_id AND ";
	querySStr << "roll >= " << _minRoll << " AND " << " roll <= " << _maxRoll << " AND ";
	querySStr << "pitch >= " << _minPitch << " AND " << " pitch <= " << _maxPitch << " AND ";
	querySStr << "yaw >= " << _minYaw << " AND " << " yaw <= " << _maxYaw;
	_query = querySStr.str();
	sqlConn->exec(_query,this);
	return true;
}

int FaceIDByPoseQuery::dataRowReady(SQLiteDBConnection *sqlConn)
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

void FaceIDByPoseQuery::clear()
{
	face_ids.clear();
}

void FaceIDByPoseQuery::setAngles(float minYaw, float maxYaw, float minPitch, float maxPitch, float minRoll, float maxRoll)
{
	_minYaw = minYaw;
	_maxYaw = maxYaw;
	_minRoll = minRoll;
	_maxRoll = maxRoll;
	_minPitch = minPitch;
	_maxPitch = maxPitch;
}

void FaceIDByPoseQuery::debugPrint()
{
	cout << "Query: '" << _query << "'" << endl;
	cout << "Result: ";
	for (unsigned int i = 0; i < face_ids.size() ; ++i)
		cout << face_ids[i] << ", ";
	cout << endl;
}

