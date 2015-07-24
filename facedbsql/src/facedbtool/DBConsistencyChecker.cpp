/*
 * DBConsistencyChecker.cpp
 *
 *  Created on: 05.12.2011
 *      Author: pwohlhart
 */

#include "DBConsistencyChecker.h"

DBConsistencyChecker::DBConsistencyChecker() {

}

DBConsistencyChecker::~DBConsistencyChecker() {

}

void DBConsistencyChecker::run(po::variables_map &vm, SQLiteDBConnection *sqlConn)
{
	_sqlConn = sqlConn;

	_doRepair = false;
	if (vm.count("inconsistency-action"))
		_doRepair = vm["inconsistency-action"].as<string>().compare("repair") == 0;

	// find (and erase) duplicate image entries
	DupImageFinder dupImgFinder;
	dupImgFinder.run(vm,sqlConn);

	findImgsWOFaces();

	_sqlConn = 0;
}

void DBConsistencyChecker::findImgsWOFaces()
{
	// TEST - find number of faces for each image
	cout << "testing "<< endl;
	string testStmtStr = "SELECT FaceImages.file_id,FaceImages.db_id,image_id,count(face_id) FROM FaceImages LEFT JOIN Faces ON Faces.file_id = FaceImages.file_id AND Faces.db_id = FaceImages.db_id GROUP BY FaceImages.db_id,FaceImages.file_id";
	//_sqlConn->exec(testStmtStr,0);
	cout << "  preparing statement";
	SQLiteStmt *testStmt = _sqlConn->prepare(testStmtStr);
	cout << "  done" << endl;
	std::vector<int> facelessImgs;
	int res = 0;
	int count = 0;
	do {
		res = _sqlConn->step(testStmt);
		//cout << "step "<< count << endl;
		string file_id;
		testStmt->readStringColumn(0,file_id);
		string db_id;
		testStmt->readStringColumn(1,db_id);
		int image_id;
		testStmt->readIntColumn(0,image_id);
		int face_count;
		testStmt->readIntColumn(3,face_count);
		//cout << count << ": " << db_id << "," << file_id  << " " << face_count << endl;
		if (face_count == 0)
		{
			cout << "image without face: " << db_id << "," << file_id << endl;
			facelessImgs.push_back(image_id);
		}
		++count;
	} while(res == SQLITE_ROW);
	delete testStmt;
	cout << "testing done"<< endl;

	if (_doRepair)
	{
		bool allOK = true;
		cout << "Deleting Images without faces" << endl;
		SQLiteStmt *delStmt = _sqlConn->prepare("DELETE FROM FaceImages WHERE image_id = ?1");
		_sqlConn->beginTransaction();
		for (int i = 0; i < facelessImgs.size(); ++i)
		{
			int image_id = facelessImgs[i];
			delStmt->reset();
			delStmt->bind(1,image_id);
			allOK = allOK && (_sqlConn->step(delStmt) == SQLITE_DONE);
			if (!allOK)
				break;
		}
		if (allOK)
			_sqlConn->commitTransaction();
		else
			_sqlConn->rollbackTransaction();

		cout << "done" << endl;
		delete delStmt;
	}
}
