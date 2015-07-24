/*
 * ImageByFaceIDQuery.h
 *
 *  Created on: 02.08.2010
 *      Author: pwohlhart
 */

#ifndef IMAGEBYFACEIDQUERY_H_
#define IMAGEBYFACEIDQUERY_H_

#include "FaceDBQuery.h"
#include "../facedata/FaceDbImage.h"

class ImageByFaceIDQuery : public FaceDBQuery {
public:
	ImageByFaceIDQuery();
	virtual ~ImageByFaceIDQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int dataRowReady(SQLiteDBConnection *sqlConn);

	// IN
	int face_id;
	// OUT
	FaceDbImage resultImage;
};

#endif /* IMAGEBYFACEIDQUERY_H_ */
