/*
 * SQLiteDBConnection.h
 *
 *  Created on: 23.07.2010
 *      Author: pwohlhart
 */

#ifndef SQLITEDBCONNECTION_H_
#define SQLITEDBCONNECTION_H_

#include <stdio.h>
#include <iostream>
#include <sqlite3.h>

using namespace std;

class SQLiteDBConnection;


class IDataCallbackObj {
public:
	virtual ~IDataCallbackObj() {};
	//virtual int recvData(int argc, char **argv, char **azColName) = 0;
	virtual int dataRowReady(SQLiteDBConnection *sqlConn) = 0;
};

class SQLiteStmt {
public:
	SQLiteStmt(SQLiteDBConnection *sqlConn, sqlite3_stmt *stmt);
	virtual ~SQLiteStmt();

	bool bind(int paramIdx, int value);
	bool bind(int paramIdx, string value);
	bool bind(int paramIdx, float value);
	bool reset();

	sqlite3_stmt *getStmt() { return _stmt; }

	bool readStringColumn(int columnIdx, string &value);
	bool readIntColumn(int columnIdx, int &value);
	bool readDoubleColumn(int columnIdx, double &value);
	bool readFloatColumn(int columnIdx, float &value);
	bool readBoolColumn(int columnIdx, bool &value);
	int getResultNumColumns();
	const char *getResultColumnName(int columnIdx);

	void setFinalized() { _finalized = true; };
private:
	SQLiteDBConnection *_sqlConn;
	sqlite3_stmt *_stmt;

	bool _finalized;
};

class SQLiteDBConnection {
public:
	SQLiteDBConnection();
	virtual ~SQLiteDBConnection();

	bool open(string dbFilename);
	int exec(string queryStr, IDataCallbackObj *cbObj = 0);
	int getLastInsertRowid();
	int changes();
	SQLiteStmt *prepare(string queryStr);
	int step(SQLiteStmt *stmt);
	int finalize(SQLiteStmt *stmt);
	void close();

	void beginTransaction();
	void commitTransaction();
	void rollbackTransaction();

	string getLastError();
	//int recvData(int argc, char **argv, char **azColName); // internal callback, called from static callback function

	bool readStringColumn(int columnIdx, string &value);
	bool readIntColumn(int columnIdx, int &value);
	bool readDoubleColumn(int columnIdx, double &value);
	bool readFloatColumn(int columnIdx, float &value);
	bool readBoolColumn(int columnIdx, bool &value);
	int getResultNumColumns();
	const char *getResultColumnName(int columnIdx);
private:
	sqlite3 *_db;
	//sqlite3_stmt *_sqlStmt;
	SQLiteStmt *_sqlStmt;

	IDataCallbackObj *_callbackObj;

	//static int conn_callback(void *obj, int argc, char **argv, char **azColName);
};



#endif /* SQLITEDBCONNECTION_H_ */
