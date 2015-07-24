/*
 * ImagesWithFacesQuery.h
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#ifndef IMAGESWITHFACESQUERY_H_
#define IMAGESWITHFACESQUERY_H_

#include "FaceDBQuery.h"
#include "../facedata/FaceDbImage.h"

class ImagesWithFacesQuery : public FaceDBQuery {
public:
	ImagesWithFacesQuery();
	virtual ~ImagesWithFacesQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int dataRowReady(SQLiteDBConnection *sqlConn);

	// IN
	std::string db_id;
	// OUT
	std::vector<FaceDbImage> resultImages;

	void debugPrint();
};

#endif /* IMAGESWITHFACESQUERY_H_ */
