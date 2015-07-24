/*
 * FaceIDQuery.h
 *
 *      Base Class for all queries that just report a face id
 *
 *
 *  Created on: 14.10.2010
 *      Author: pwohlhart
 *
 */

#ifndef FACEIDQUERY_H_
#define FACEIDQUERY_H_

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

#include "FaceDBQuery.h"

class FaceIDQuery : public FaceDBQuery {
public:
	FaceIDQuery();
	virtual ~FaceIDQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int recvData(int argc, char **argv, char **azColName);
	int dataRowReady(SQLiteDBConnection *sqlConn);

	void setQueryString(std::string queryString);

	vector<int> face_ids;

	void debugPrint();

protected:
	virtual void prepareQuery(SQLiteDBConnection *sqlConn, stringstream &querySStr);

	std::string _userDefinedQueryString;
};

#endif /* FACEIDQUERY_H_ */
