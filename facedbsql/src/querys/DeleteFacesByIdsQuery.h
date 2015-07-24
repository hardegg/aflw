/*
 * DeleteFacesByIdsQuery.h
 *
 *  Created on: 30.07.2010
 *      Author: koestinger
 */

#ifndef DELETEFACESBYIDQUERY_H_
#define DELETEFACESBYIDQUERY_H_

#include "FaceDBQuery.h"

class DeleteFacesByIdsQuery : public FaceDBQuery {
public:
	DeleteFacesByIdsQuery();
	virtual ~DeleteFacesByIdsQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int dataRowReady(SQLiteDBConnection *sqlConn);

	// IN
	std::vector<int> face_ids;

	// OUT
	std::vector<bool> row_affected;

	void debugPrint();
};

#endif /* DELETEFACESBYIDQUERY_H_ */
