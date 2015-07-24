/*
 * SQLiteDBConnection.cpp
 *
 *  Created on: 23.07.2010
 *      Author: pwohlhart
 */


#include <iostream>
#include <string>

#include "SQLiteDBConnection.h"

using namespace std;

SQLiteDBConnection::SQLiteDBConnection() : _db(0),_sqlStmt(0),_callbackObj(0) {

}

SQLiteDBConnection::~SQLiteDBConnection() {

}

bool
SQLiteDBConnection::open(string dbFilename) {

	int rc;

	rc = sqlite3_open(dbFilename.c_str(), &_db);
	if (rc)
	{
		cout << "Cannot open database '" << dbFilename << "':" << endl;
		cout << " " << sqlite3_errmsg(_db) << endl;
		return false;
	}

	return true;
}

void
SQLiteDBConnection::close()
{
	sqlite3_close(_db);
}

int
SQLiteDBConnection::exec(string queryStr, IDataCallbackObj *cbObj)
{
	_callbackObj = cbObj;
	char *zErrMsg = 0;

	if (cbObj == 0)
	{
		int rc = sqlite3_exec(_db, queryStr.c_str(), 0, 0, &zErrMsg);
		if( rc != SQLITE_OK ) {
			sqlite3_free(zErrMsg);
			cout << "SQL error: " << zErrMsg << endl;
		}
	}
	else
	{
		int qsl = queryStr.length();
		//int rc = sqlite3_prepare_v2(_db,queryStr.c_str(),qsl,&_sqlStmt,NULL);
		_sqlStmt = prepare(queryStr);
		if (_sqlStmt)
		{
			int rc = 0;
			do
			{
				rc = step(_sqlStmt);
				if (rc == SQLITE_ROW)
					cbObj->dataRowReady(this);
			} while (rc == SQLITE_ROW);

			if (rc != SQLITE_DONE)
				cout << "ERROR: " << rc << "; " << sqlite3_errmsg(_db) << endl;

			//sqlite3_finalize(_sqlStmt);
			finalize(_sqlStmt);
			delete _sqlStmt;
		}
		else
			cout << "ERROR: " << sqlite3_errmsg(_db) << endl;
	}

	_callbackObj = 0;
	return 0;
}

int SQLiteDBConnection::getLastInsertRowid()
{
	return sqlite3_last_insert_rowid(_db);
};

//Count The Number Of Rows Modified e.g. INSERT, UPDATE,DELETE
int SQLiteDBConnection::changes()
{
	return sqlite3_changes(_db);
}

SQLiteStmt *SQLiteDBConnection::prepare(string queryStr)
{
	SQLiteStmt *sqlStmt = 0;

	sqlite3_stmt *sqlite3Stmt;
	int rc = sqlite3_prepare_v2(_db,queryStr.c_str(),static_cast<int>(queryStr.length()),&sqlite3Stmt,0);
	if (rc == SQLITE_OK)
		sqlStmt = new SQLiteStmt(this,sqlite3Stmt);
	else
		cout << "Error: " << sqlite3_errmsg(_db) << endl;

	return sqlStmt;
}

int SQLiteDBConnection::step(SQLiteStmt *stmt)
{
	int res = sqlite3_step(stmt->getStmt());
	if ((res != SQLITE_DONE) && (res != SQLITE_ROW))
	{
		cout << sqlite3_errmsg(_db) << endl;
	}
	return res;
}

string SQLiteDBConnection::getLastError()
{
	 return sqlite3_errmsg(_db);
}

int SQLiteDBConnection::finalize(SQLiteStmt *stmt)
{
	int res = sqlite3_finalize(stmt->getStmt());
	stmt->setFinalized();
	return res;
}

bool SQLiteDBConnection::readStringColumn(int columnIdx, string &value)
{
	return _sqlStmt->readStringColumn(columnIdx,value);
}

bool SQLiteDBConnection::readIntColumn(int columnIdx, int &value)
{
	return _sqlStmt->readIntColumn(columnIdx,value);
}
bool SQLiteDBConnection::readDoubleColumn(int columnIdx, double &value)
{
	return _sqlStmt->readDoubleColumn(columnIdx,value);
}
bool SQLiteDBConnection::readFloatColumn(int columnIdx, float &value)
{
	return _sqlStmt->readFloatColumn(columnIdx,value);
}
bool SQLiteDBConnection::readBoolColumn(int columnIdx, bool &value)
{
	return _sqlStmt->readBoolColumn(columnIdx,value);
}

int SQLiteDBConnection::getResultNumColumns()
{
	//return sqlite3_column_count(_sqlStmt);
	return _sqlStmt->getResultNumColumns();
}

const char *SQLiteDBConnection::getResultColumnName(int columnIdx)
{
	//return sqlite3_column_name(_sqlStmt,columnIdx);
	return _sqlStmt->getResultColumnName(columnIdx);
}

void SQLiteDBConnection::beginTransaction()
{
	exec("PRAGMA synchronous = OFF;");
	exec("BEGIN TRANSACTION;");

}

void SQLiteDBConnection::commitTransaction()
{
	exec("COMMIT;");
	exec("PRAGMA synchronous = NORMAL;");
}

void SQLiteDBConnection::rollbackTransaction()
{
	exec("ROLLBACK TRANSACTION;");
	exec("PRAGMA synchronous = NORMAL;");
}



/*
int
SQLiteDBConnection::recvData(int argc, char **argv, char **azColName)
{
	if (_callbackObj)
		return _callbackObj->recvData(argc,argv,azColName);
	else
	{
		cout << "num cols: " << argc << endl;
		return 0;
	}
}

int SQLiteDBConnection::conn_callback(void *obj, int argc, char **argv, char **azColName)
{
	SQLiteDBConnection *sqliteDbConn = reinterpret_cast<SQLiteDBConnection*>(obj);
	return sqliteDbConn->recvData(argc, argv, azColName);
}
*/


//-------------------------------------------------------------


SQLiteStmt::SQLiteStmt(SQLiteDBConnection *sqlConn, sqlite3_stmt *stmt) : _sqlConn(sqlConn), _stmt(stmt), _finalized(false)
{

}

SQLiteStmt::~SQLiteStmt()
{
	if (!_finalized)
		_sqlConn->finalize(this);
}

bool SQLiteStmt::reset()
{
	return sqlite3_reset(_stmt) == SQLITE_OK;
}

bool SQLiteStmt::bind(int paramIdx, int value)
{
	return sqlite3_bind_int(_stmt,paramIdx,value) == SQLITE_OK;
}

bool SQLiteStmt::bind(int paramIdx, string value)
{
	return sqlite3_bind_text(_stmt,paramIdx,value.c_str(), static_cast<int>(value.length()), SQLITE_TRANSIENT) == SQLITE_OK;
}

bool SQLiteStmt::bind(int paramIdx, float value)
{
	return sqlite3_bind_double(_stmt,paramIdx,value) == SQLITE_OK;
}

int SQLiteStmt::getResultNumColumns()
{
	return sqlite3_column_count(_stmt);
}

const char *SQLiteStmt::getResultColumnName(int columnIdx)
{
	return sqlite3_column_name(_stmt,columnIdx);
}

bool SQLiteStmt::readStringColumn(int columnIdx, string &value)
{
	if (sqlite3_column_type(_stmt, columnIdx) == SQLITE_TEXT)
	{
		const char* res = reinterpret_cast<const char*>(sqlite3_column_text(_stmt,columnIdx));
		value = res;
		return true;
	}
	else
		return false;
}

bool SQLiteStmt::readIntColumn(int columnIdx, int &value)
{
	int ct = sqlite3_column_type(_stmt, columnIdx);
	if (ct == SQLITE_INTEGER)
	{
		value = sqlite3_column_int(_stmt,columnIdx);
		return true;
	}
	else
		return false;
}

bool SQLiteStmt::readDoubleColumn(int columnIdx, double &value)
{
	int ct = sqlite3_column_type(_stmt, columnIdx);
	if (ct == SQLITE_FLOAT)
	{
		value = sqlite3_column_double(_stmt,columnIdx);
		return true;
	}
	else
		return false;
}

bool SQLiteStmt::readFloatColumn(int columnIdx, float &value)
{
	double dval;
	bool res = readDoubleColumn(columnIdx,dval);
	value = dval;
	return res;
}

bool SQLiteStmt::readBoolColumn(int columnIdx, bool &value)
{
	int ct = sqlite3_column_type(_stmt, columnIdx);
	if (ct == SQLITE_INTEGER)
	{
		value = sqlite3_column_int(_stmt,columnIdx) != 0;
		return true;
	}
	else
		return false;
}
