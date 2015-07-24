/*
 * FaceIDByAnnotatedFeature.cpp
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#include "FaceIDByAnnotatedFeatureQuery.h"

FaceIDByAnnotatedFeatureQuery::FaceIDByAnnotatedFeatureQuery()
{
}

FaceIDByAnnotatedFeatureQuery::~FaceIDByAnnotatedFeatureQuery()
{
}

void FaceIDByAnnotatedFeatureQuery::clear()
{
	resultFaceIds.clear();
}

bool FaceIDByAnnotatedFeatureQuery::exec(SQLiteDBConnection *sqlConn)
{
	if (queryFeatureIDs.size() == 0)
		return false;

	clear();

	int numFeatures = queryFeatureIDs.size();

	stringstream feature_idsSStr;
	feature_idsSStr << queryFeatureIDs[0];
	for (int i = 1; i < numFeatures; ++i)
		feature_idsSStr << "," << queryFeatureIDs[i];

	stringstream querySStr;
	querySStr << "SELECT faces.face_id FROM faces,featurecoords ";
	querySStr << "WHERE faces.face_id = featurecoords.face_id AND feature_id IN (" << feature_idsSStr.str() << ")";
	querySStr << "GROUP BY faces.face_id HAVING count(feature_id) = " << numFeatures;

	_query = querySStr.str();
	int res = sqlConn->exec(_query,this);

	return res == SQLITE_OK;
}

int FaceIDByAnnotatedFeatureQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	int faceId;
	if (sqlConn->readIntColumn(0,faceId))
	{
		resultFaceIds.push_back(faceId);
		return 0;
	}
	else
		return 1;
}

void FaceIDByAnnotatedFeatureQuery::debugPrint()
{

}
