/*
 * FaceRectsByFaceIDQuery.cpp
 *
 *  Created on: 06.12.2011
 *      Author: pwohlhart
 */

#include "FaceRectsByFaceIDQuery.h"

FaceRectsByFaceIDQuery::FaceRectsByFaceIDQuery() :_faceID(-1) {

}

FaceRectsByFaceIDQuery::~FaceRectsByFaceIDQuery() {

}

bool FaceRectsByFaceIDQuery::exec(SQLiteDBConnection *sqlConn)
{
	stringstream sqlStmtStrm;
	sqlStmtStrm << "SELECT x,y,w,h,annot_type_id FROM FaceRect WHERE face_id = ";
	sqlStmtStrm << _faceID;

	sqlConn->exec(sqlStmtStrm.str(),this);
	return true;
}

void FaceRectsByFaceIDQuery::clear()
{
	faceRects.clear();
}

int FaceRectsByFaceIDQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	bool allOK = true;
	FaceRect r;
	allOK = allOK && sqlConn->readIntColumn(0,r.x);
	allOK = allOK && sqlConn->readIntColumn(1,r.y);
	allOK = allOK && sqlConn->readIntColumn(2,r.w);
	allOK = allOK && sqlConn->readIntColumn(3,r.h);
	allOK = allOK && sqlConn->readIntColumn(4,r.annotType);
	if (allOK)
		faceRects.push_back(r);
	return (allOK ? 0 : -1);
}

void FaceRectsByFaceIDQuery::setFaceID(int face_id)
{
	_faceID = face_id;
}





