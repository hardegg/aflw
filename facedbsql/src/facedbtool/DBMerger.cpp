/*
 * DBMerger.cpp
 *
 *  Created on: 29.11.2011
 *      Author: pwohlhart
 */

#include "DBMerger.h"

#include "../querys/FaceDataByIDsQuery.h"

DBMerger::DBMerger() {

}

DBMerger::~DBMerger() {
}

void DBMerger::run(po::variables_map &vm, SQLiteDBConnection *sqlConn)
{
	if (vm.count("sqldb-file-source") < 1)
	{
		cout << "error: you need to specify a --sqldb-file-source to merge into --sqldb-file" << endl;
		return;
	}

	string sourceDBFile = vm["sqldb-file-source"].as<string>();

	FaceIDsLoader faceIDsLoader;
	_targetSqlConn = sqlConn;
	_sourceSqlConn = new SQLiteDBConnection();
	if (_sourceSqlConn->open(sourceDBFile))
	{
		// read faces from source
		FaceDataByIDsQuery faceDataQuery;
		faceDataQuery.queryIds = faceIDsLoader.loadFaceIDs(_sourceSqlConn);

		cout << "loading data for " << faceDataQuery.queryIds.size() << " faces" << endl;

		if (faceDataQuery.exec(_sourceSqlConn))
		{
			for (int i = 0; i < faceDataQuery.queryIds.size() ; ++i)
			{
				int faceID = faceDataQuery.queryIds[i];
				FaceData *faceData = faceDataQuery.data[faceID];
				// for each face
				//   * push feature coords into target db
				//   * check if image is already in target db
				//       -> if not, add it and check if there is an entry for the db
				//          -> if not add an entry for the db, with empty path and description (cause wee dont know it; maybe display a warning at the end?)
				cout << "face: " << faceData->ID << ", " << faceData->dbID << ", " << faceData->fileID << endl;
				cout << "  image: " << faceData->getDbImg()->db_id << ", " << faceData->getDbImg()->file_id << endl;
				cout << "  hasEllipse: " << (faceData->hasEllipse() ? "yes" : "no") << endl;
				cout << "  metadata: " << (faceData->getMetadata() ? "yes" : "no") << endl;
				cout << "  pose: " << (faceData->getPose() ? "yes" : "no") << endl;

				faceData->loadFeatureCoords(_sourceSqlConn);

				faceData->ID = -1;
				faceData->save(_targetSqlConn);
			}
		}

		_sourceSqlConn->close();
	}
	_sourceSqlConn = 0;
	_targetSqlConn = 0;
}


/*
void DBMerger::transferFace(FaceData &faceData)
{
	// insert features
	_insertFeatureCoordsSqlStmt->bind(1,face_id);
	for (unsigned int i = 0; i < featIt->coords.size(); ++i)
	{
		if ((featIt->coords[i].first > 0) && (featIt->coords[i].second > 0))
		{
			insertFeatureCoordsSqlStmt->bind(2,featIDs[i]);
			insertFeatureCoordsSqlStmt->bind(3,featIt->coords[i].first);
			insertFeatureCoordsSqlStmt->bind(4,featIt->coords[i].second);
			allOK = allOK && (sqlConn.step(insertFeatureCoordsSqlStmt) == SQLITE_DONE);
			if (!allOK)
				break;
			insertFeatureCoordsSqlStmt->reset();
		}
	}
}*/

/**************************************************************
 *
 *           FacesLoader
 *
 **************************************************************/

FaceIDsLoader::FaceIDsLoader() {

}

FaceIDsLoader::~FaceIDsLoader() {

}

vector<int> FaceIDsLoader::loadFaceIDs(SQLiteDBConnection *sqlConn)
{
	_faceIDs.clear();
	sqlConn->exec("SELECT face_id FROM Faces",this);
	return _faceIDs;
}
int FaceIDsLoader::dataRowReady(SQLiteDBConnection *sqlConn)
{
	//FaceData fd;
	int id;
	sqlConn->readIntColumn(0,id);
	//sqlConn->readStringColumn(1,fd.fileID);
	//sqlConn->readStringColumn(2,fd.dbID);
	_faceIDs.push_back(id);
	return 0;
}
