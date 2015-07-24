/*
 * FaceDBQuery.h
 *
 *  Created on: 26.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEDBQUERY_H_
#define FACEDBQUERY_H_

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

#include <sstream>

using namespace std;

class FaceDBData
{
	public:
		std::string db_id;
		std::string path;
		std::string description;
}; 

class FaceDBQuery : public IDataCallbackObj {
public:
	FaceDBQuery();
	virtual ~FaceDBQuery();

	virtual bool exec(SQLiteDBConnection *sqlConn);
	virtual void clear();

	int dataRowReady(SQLiteDBConnection *sqlConn);

	std::vector<FaceDBData> dbs;

protected:
	string _query;
};









#endif /* FACEDBQUERY_H_ */
