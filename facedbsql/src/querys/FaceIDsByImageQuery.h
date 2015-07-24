/*
 * FaceIDByImageQuery.h
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEIDSBYIMAGEQUERY_H_
#define FACEIDSBYIMAGEQUERY_H_

#include "FaceDBQuery.h"

// TODO: convert to FaceIDQuery

class FaceIDsByImageQuery : public FaceDBQuery {
public:
	FaceIDsByImageQuery();
	virtual ~FaceIDsByImageQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int dataRowReady(SQLiteDBConnection *sqlConn);

	// IN
	string img_file_id;
	string img_db_id;

	// OUT
	vector<int> resultFaceIds;

	void debugPrint();
};

#endif /* FACEIDSBYIMAGEQUERY_H_ */
