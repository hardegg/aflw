/*
 * FaceIDByAnnotatedFeature.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEIDBYANNOTATEDFEATUREQUERY_H_
#define FACEIDBYANNOTATEDFEATUREQUERY_H_

#include "FaceDBQuery.h"

// TODO: convert to FaceIDQuery

class FaceIDByAnnotatedFeatureQuery : public FaceDBQuery {
public:
	FaceIDByAnnotatedFeatureQuery();
	virtual ~FaceIDByAnnotatedFeatureQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int recvData(int argc, char **argv, char **azColName);
	int dataRowReady(SQLiteDBConnection *sqlConn);

	vector<int> queryFeatureIDs;
	vector<int> resultFaceIds;

	void debugPrint();
};

#endif /* FACEIDBYANNOTATEDFEATUREQUERY_H_ */
