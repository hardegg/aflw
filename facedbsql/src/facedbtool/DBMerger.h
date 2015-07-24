/*
 * DBMerger.h
 *
 *  Created on: 29.11.2011
 *      Author: pwohlhart
 */

#ifndef DBMERGER_H_
#define DBMERGER_H_

#include <boost/program_options.hpp>
#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

namespace po = boost::program_options;


class FaceIDsLoader : IDataCallbackObj
{
public:
	FaceIDsLoader();
	virtual ~FaceIDsLoader();
	vector<int> loadFaceIDs(SQLiteDBConnection *sqlConn);
	int dataRowReady(SQLiteDBConnection *sqlConn);
private:
	vector<int> _faceIDs;
};

class DBMerger {
public:
	DBMerger();
	virtual ~DBMerger();

	void run(po::variables_map &vm, SQLiteDBConnection *sqlConn);
private:
	SQLiteDBConnection *_sourceSqlConn;
	SQLiteDBConnection *_targetSqlConn;

	SQLiteStmt *_insertFeatureCoordsSqlStmt;

	//void transferFace(FaceData &faceData);
};

#endif /* DBMERGER_H_ */
