/*
 * FacesWithPoseQuery.cpp
 *
 *  Created on: 14.10.2010
 *      Author: pwohlhart
 */

#include "FacesWithPoseQuery.h"

FacesWithPoseQuery::FacesWithPoseQuery() {
}

FacesWithPoseQuery::~FacesWithPoseQuery() {
}

void FacesWithPoseQuery::prepareQuery(SQLiteDBConnection *sqlConn, stringstream &querySStr)
{
	querySStr << "SELECT faces.face_id FROM faces,facepose WHERE faces.face_id = facepose.face_id";
}

