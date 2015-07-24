/*
 * FaceRectsByFaceIDQuery.h
 *
 *  Created on: 06.12.2011
 *      Author: pwohlhart
 */

#ifndef FACERECTSBYFACEIDQUERY_H_
#define FACERECTSBYFACEIDQUERY_H_

#include <vector>
#include <string>

#include "FaceDBQuery.h"
#include "../facedata/FaceRect.h"

class FaceRectsByFaceIDQuery: public FaceDBQuery {
public:
	FaceRectsByFaceIDQuery();
	virtual ~FaceRectsByFaceIDQuery();

	void setFaceID(int face_id);
	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int dataRowReady(SQLiteDBConnection *sqlConn);

	vector<FaceRect> faceRects;

	inline int getFaceID() { return _faceID; }
protected:
	int _faceID;
};

#endif /* FACERECTSBYFACEIDQUERY_H_ */
