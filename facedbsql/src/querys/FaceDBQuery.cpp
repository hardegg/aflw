/*
 * FaceDBQuery.cpp
 *
 *  Created on: 26.07.2010
 *      Author: pwohlhart
 */

#include "FaceDBQuery.h"

FaceDBQuery::FaceDBQuery()
{
}

FaceDBQuery::~FaceDBQuery()
{
}

void FaceDBQuery::clear()
{
	dbs.clear();
}

bool FaceDBQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();

	std::stringstream sqlQuerySStr;
	sqlQuerySStr << "SELECT db_id, path, description FROM Databases";
	std::string query = sqlQuerySStr.str();
	
	int res = sqlConn->exec(query,this);
	return res == SQLITE_OK;
}

int FaceDBQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	bool allOK = true;
	FaceDBData db;
	allOK = allOK && sqlConn->readStringColumn(0,db.db_id);
	allOK = allOK && sqlConn->readStringColumn(1,db.path);
	sqlConn->readStringColumn(2,db.description);
	
	if (allOK)
	{
		dbs.push_back(db);
	}
	return 0;
}

//



