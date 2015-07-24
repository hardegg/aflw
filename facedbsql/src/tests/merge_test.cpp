/*
 * main.cpp
 *
 *  Created on: 29.11.2011
 *      Author: pwohlhart
 */

#include <vector>
#include <iostream>
#include <sstream>
#include <sqlite3.h>

#include "../dbconn/SQLiteDBConnection.h"


using namespace std;


/*********************************************
 *   Lister
 *********************************************/

class Lister : public IDataCallbackObj
{
public:
	void list(SQLiteDBConnection *sqlConn);
	int dataRowReady(SQLiteDBConnection *sqlConn);
private:
	vector<string> _names;
	vector<int> _ids;
};

void Lister::list(SQLiteDBConnection *sqlConn)
{
	_names.clear();
	_ids.clear();
	sqlConn->exec("SELECT id,name FROM names",this);
	for (int i = 0; i < _names.size(); ++i)
	{
		cout << _ids[i] << ": " << _names[i] << endl;
	}
}

int Lister::dataRowReady(SQLiteDBConnection *sqlConn)
{
	int id;
	if (sqlConn->readIntColumn(0,id))
		_ids.push_back(id);
	string name;
	if (sqlConn->readStringColumn(1,name))
		_names.push_back(name);
	return 0;
}

/*********************************************
 *   Merger
 *********************************************/


class Merger : public IDataCallbackObj
{
public:
	void merge(SQLiteDBConnection *sqlConnSource,SQLiteDBConnection *sqlConnTarget);
	int dataRowReady(SQLiteDBConnection *sqlConn);
private:
	SQLiteDBConnection *_currentTarget;
};

void Merger::merge(SQLiteDBConnection *sqlConnSource,SQLiteDBConnection *sqlConnTarget)
{
	_currentTarget = sqlConnTarget;
	sqlConnSource->exec("SELECT id,name FROM names",this);
	_currentTarget = 0;
}

int Merger::dataRowReady(SQLiteDBConnection *sqlConn)
{
	string name;
	if (sqlConn->readStringColumn(1,name))
	{
		// push it to the target
		stringstream insertStmtStrm;
		insertStmtStrm << "INSERT INTO names (name) VALUES (\"" << name << "\")";
		_currentTarget->exec(insertStmtStrm.str(),0);
	}

	return 0;
}

/*********************************************
 *   main
 *********************************************/

int main (int argc, char **argv)
{
	string dbFilename1 = "/home/pwohlhart/work/data/facedb/test1.sqlite";
	string dbFilename2 = "/home/pwohlhart/work/data/facedb/test2.sqlite";

	SQLiteDBConnection *sqlConn1 = new SQLiteDBConnection();
	SQLiteDBConnection *sqlConn2 = new SQLiteDBConnection();

	bool res1 = sqlConn1->open(dbFilename1);
	bool res2 = sqlConn2->open(dbFilename2);

	Lister lister;
	Merger merger;

	if (res1 && res2)
	{
		lister.list(sqlConn1);
		cout << "-----"  << endl;
		lister.list(sqlConn2);

		merger.merge(sqlConn1,sqlConn2);

		cout << "-----"  << endl;
		lister.list(sqlConn2);
	}

	delete sqlConn1;
	delete sqlConn2;
}
