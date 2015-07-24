/*
 * FaceIDByImageQuery.cpp
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#include "FaceIDsByImageQuery.h"

#include <sstream>
#include <string>

FaceIDsByImageQuery::FaceIDsByImageQuery() : img_db_id(""),img_file_id("")
{
}

FaceIDsByImageQuery::~FaceIDsByImageQuery()
{
}


void FaceIDsByImageQuery::clear()
{
	resultFaceIds.clear();
}


bool FaceIDsByImageQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();
	std::stringstream sqlQuerySStr;
	sqlQuerySStr << "SELECT face_id FROM faces WHERE file_id = '" << img_file_id << "' AND db_id = '" << img_db_id << "'";

	_query = sqlQuerySStr.str();
	//cout << "FaceIDsByImageQuery: " << _query << endl;
	bool ok = (sqlConn->exec(_query,this) == SQLITE_OK);
	return ok;
}


int FaceIDsByImageQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	int face_id;
	if (sqlConn->readIntColumn(0,face_id))
		resultFaceIds.push_back(face_id);
	return 0;
}


void FaceIDsByImageQuery::debugPrint()
{

}


