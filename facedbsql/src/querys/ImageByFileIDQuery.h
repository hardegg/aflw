/*
 * ImageByFaceIDQuery.h
 *
 *  Created on: 02.08.2010
 *      Author: koestinger
 */

#ifndef IMAGEBYFILEIDQUERY_H_
#define IMAGEBYFILEIDQUERY_H_

#include "FaceDBQuery.h"
#include "../facedata/FaceDbImage.h"

class ImageByFileIDQuery : public FaceDBQuery {
public:
	ImageByFileIDQuery();
	virtual ~ImageByFileIDQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int dataRowReady(SQLiteDBConnection *sqlConn);

	// IN
	std::string image_id;
	std::string db_id;
	
	// OUT
	FaceDbImage resultImage;
};

#endif /* IMAGEBYFILEIDQUERY_H_ */
