/*
 * DeleteFacesByIdsQuery.cpp
 *
 *  Created on: 30.07.2010
 *      Author: koestinger
 */

#include "DeleteFacesByIdsQuery.h"

#include <sstream>
#include <string>

DeleteFacesByIdsQuery::DeleteFacesByIdsQuery()
{
}

DeleteFacesByIdsQuery::~DeleteFacesByIdsQuery() {
}

void DeleteFacesByIdsQuery::clear()
{
	row_affected.clear();
}

bool DeleteFacesByIdsQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();

	bool allOK = true;
	row_affected = std::vector<bool>(face_ids.size(), false);

	sqlConn->exec("PRAGMA synchronous = OFF;");
	sqlConn->exec("BEGIN TRANSACTION;");

	std::string delFaceMetaDataSqlStmtStr = "DELETE FROM FaceMetadata WHERE face_id = ?1";
	std::string delFacePoseSqlStmtStr = "DELETE FROM FacePose WHERE face_id = ?1";
	std::string delFacesSqlStmtStr = "DELETE FROM Faces WHERE face_id = ?1";
	std::string delFeatureCoordsSqlStmtStr = "DELETE FROM FeatureCoords WHERE face_id = ?1";
	std::string delEllipseSqlStmtStr = "DELETE FROM FaceEllipse WHERE face_id = ?1";
	std::string delRectSqlStmtStr = "DELETE FROM FaceEllipse WHERE face_id = ?1";

	SQLiteStmt *delFaceMetaDataSqlStmt = sqlConn->prepare(delFaceMetaDataSqlStmtStr);
	SQLiteStmt *delFacePoseSqlStmt = sqlConn->prepare(delFacePoseSqlStmtStr);
	SQLiteStmt *delFacesSqlStmt = sqlConn->prepare(delFacesSqlStmtStr);
	SQLiteStmt *delFeatureCoordsSqlStmt = sqlConn->prepare(delFeatureCoordsSqlStmtStr);
	SQLiteStmt *delEllipseSqlStmt = sqlConn->prepare(delEllipseSqlStmtStr);
	SQLiteStmt *delRectSqlStmt = sqlConn->prepare(delEllipseSqlStmtStr);

	int changes;

	for(int counter = 0; counter < face_ids.size(); ++counter)
	{
		delFaceMetaDataSqlStmt->reset();
		delFaceMetaDataSqlStmt->bind(1,face_ids.at(counter));
		allOK = allOK && (sqlConn->step(delFaceMetaDataSqlStmt) == SQLITE_DONE);

		delFacePoseSqlStmt->reset();
		delFacePoseSqlStmt->bind(1,face_ids.at(counter));
		allOK = allOK && (sqlConn->step(delFacePoseSqlStmt) == SQLITE_DONE);

		delEllipseSqlStmt->reset();
		delEllipseSqlStmt->bind(1,face_ids.at(counter));
		allOK = allOK && (sqlConn->step(delEllipseSqlStmt) == SQLITE_DONE);
		changes = sqlConn->changes();

		delRectSqlStmt->reset();
		delRectSqlStmt->bind(1,face_ids.at(counter));
		allOK = allOK && (sqlConn->step(delRectSqlStmt) == SQLITE_DONE);
		changes = sqlConn->changes();

		delFacesSqlStmt->reset();
		delFacesSqlStmt->bind(1,face_ids.at(counter));
		allOK = allOK && (sqlConn->step(delFacesSqlStmt) == SQLITE_DONE);
		changes = sqlConn->changes();

		if(changes == 1)
		{
			row_affected.at(counter) = true;
		}
		else if(changes > 1)
		{
			std::cout << "ERROR: MORE THAN ONE ROW AFFECTED DELETING A FACE" << std::endl;
			allOK = false;
			break;
		}
		else
			row_affected.at(counter) = false;

		delFeatureCoordsSqlStmt->reset();
		delFeatureCoordsSqlStmt->bind(1,face_ids.at(counter));
		allOK = allOK && (sqlConn->step(delFeatureCoordsSqlStmt) == SQLITE_DONE);
	}

	if (allOK)
		sqlConn->exec("COMMIT;");
	else
		sqlConn->exec("ROLLBACK TRANSACTION;");

	sqlConn->exec("PRAGMA synchronous = NORMAL;");

	delFaceMetaDataSqlStmt->setFinalized();
	delete delFaceMetaDataSqlStmt;
	delFaceMetaDataSqlStmt = 0;

	delFacePoseSqlStmt->setFinalized();
	delete delFacePoseSqlStmt;
	delFacePoseSqlStmt = 0;

	delFacesSqlStmt->setFinalized();
	delete delFacesSqlStmt;
	delFacesSqlStmt = 0;

	delFeatureCoordsSqlStmt->setFinalized();
	delete delFeatureCoordsSqlStmt;
	delFeatureCoordsSqlStmt = 0;

	delEllipseSqlStmt->setFinalized();
	delete delEllipseSqlStmt;
	delEllipseSqlStmt = 0;

	delRectSqlStmt->setFinalized();
	delete delRectSqlStmt;
	delRectSqlStmt = 0;

	return allOK;
}

int DeleteFacesByIdsQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	return 0;
}

void DeleteFacesByIdsQuery::debugPrint()
{

}
