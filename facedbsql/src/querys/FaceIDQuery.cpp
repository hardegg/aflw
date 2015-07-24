/*
 * FaceIDQuery.cpp
 *
 *  Created on: 14.10.2010
 *      Author: pwohlhart
 */

#include "FaceIDQuery.h"

FaceIDQuery::FaceIDQuery() {
}

FaceIDQuery::~FaceIDQuery() {
}

void FaceIDQuery::clear()
{
	face_ids.clear();
}

bool FaceIDQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();
	stringstream querySStr;
	prepareQuery(sqlConn,querySStr);
	_query = querySStr.str();
	sqlConn->exec(_query,this);
	return true;
}


int FaceIDQuery::dataRowReady(SQLiteDBConnection *sqlConn)
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

void FaceIDQuery::setQueryString(std::string queryString)
{
	_userDefinedQueryString = queryString;
}

void FaceIDQuery::prepareQuery(SQLiteDBConnection *sqlConn, stringstream &querySStr)
{
	querySStr << _userDefinedQueryString;
}

void FaceIDQuery::debugPrint()
{
	cout << "Query: '" << _query << "'" << endl;
	cout << "Result: ";
	for (unsigned int i = 0; i < face_ids.size() ; ++i)
		cout << face_ids[i] << ", ";
	cout << endl;
}

int FaceIDQuery::recvData(int argc, char **argv, char **azColName) { return 0; }  // not needed

