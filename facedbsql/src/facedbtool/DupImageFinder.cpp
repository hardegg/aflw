/*
 * DupImageFinder.cpp
 *
 *  Created on: 05.12.2011
 *      Author: pwohlhart
 */

#include "DupImageFinder.h"

DupImageFinder::DupImageFinder() {

}

DupImageFinder::~DupImageFinder() {
}

void DupImageFinder::run(po::variables_map &vm, SQLiteDBConnection *sqlConn)
{
	_sqlConn = sqlConn;
	bool doDelete = false;
	if (vm.count("dup-action"))
		doDelete = vm["dup-action"].as<string>().compare("delete") == 0;

	findDupImages(doDelete);
}

void DupImageFinder::findDupImages(bool doDelete)
{
	// get (db_id,file_id) combos that appear multiple times (which should never be the case)
	string findStmtStr = "SELECT db_id,file_id FROM FaceImages GROUP BY db_id,file_id HAVING count(image_id) > 1";
	SQLiteStmt *findStmt = _sqlConn->prepare(findStmtStr);
	findStmt->reset();

	vector<string> db_ids;
	vector<string> file_ids;
	vector<vector<int> > image_ids;
	int rc = 0;
	do
	{
		rc = _sqlConn->step(findStmt);
		if (rc == SQLITE_ROW)
		{
			std::string db_id;
			findStmt->readStringColumn(0,db_id);
			db_ids.push_back(db_id);

			std::string file_id;
			findStmt->readStringColumn(1,file_id);
			file_ids.push_back(file_id);

			image_ids.push_back(vector<int>(0,0));
		}
	} while (rc == SQLITE_ROW);
	_sqlConn->finalize(findStmt);
	delete findStmt;

	if (file_ids.size() == 0)
	{
		cout << "No duplicate images" << endl;
		return;
	}

	// get list of image_ids for each multiple (db_id,file_id)
	string findStmtStr2 = "SELECT image_id FROM FaceImages WHERE db_id = ?1 AND file_id = ?2";
	SQLiteStmt *findStmt2 = _sqlConn->prepare(findStmtStr2);
	for (int i = 0; i < file_ids.size(); ++i)
	{
		findStmt2->reset();
		if (!findStmt2->bind(1,db_ids[i]))
		{
			cout << "fail: " << _sqlConn->getLastError() << endl;
			throw runtime_error("");
		}
		findStmt2->bind(2,file_ids[i]);
		while (true)
		{
			rc = _sqlConn->step(findStmt2);
			if (rc == SQLITE_ROW)
			{
				int image_id;
				findStmt2->readIntColumn(0,image_id);
				image_ids[i].push_back(image_id);
			}
			else
				break;
		}
	}
	_sqlConn->finalize(findStmt2);
	delete findStmt2;

	if (doDelete)
	{
		// for each (db_id,file_id) delete all but one
		cout << "Delete multiple entries for " << file_ids.size() << " images" << endl;
		string delStmtStr = "DELETE FROM FaceImages WHERE image_id = ?1";
		SQLiteStmt *delStmt = _sqlConn->prepare(delStmtStr);
		for (int i = 0; i < image_ids.size(); ++i)
		{
			cout << "Image " << db_ids[i] << "," << file_ids[i] << " has " << image_ids[i].size() << " entries";
			cout << " -> removing " << image_ids[i].size()-1 << endl;
			for (int j = 1; j < image_ids[i].size(); ++j)
			{
				delStmt->reset();
				delStmt->bind(1,image_ids[i][j]);
				rc = _sqlConn->step(delStmt);
				if (rc != SQLITE_DONE)
					cout << "Problem deleting image_id = " << image_ids[i][j] << endl;
			}
		}
		_sqlConn->finalize(delStmt);
		delete delStmt;
	}
}
