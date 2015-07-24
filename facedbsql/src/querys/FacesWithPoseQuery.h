/*
 * FacesWithPoseQuery.h
 *
 *  Created on: 14.10.2010
 *      Author: pwohlhart
 */

#ifndef FACESWITHPOSEQUERY_H_
#define FACESWITHPOSEQUERY_H_

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

#include "FaceIDQuery.h"

class FacesWithPoseQuery : public FaceIDQuery  {
public:
	FacesWithPoseQuery();
	virtual ~FacesWithPoseQuery();
protected:
	void prepareQuery(SQLiteDBConnection *sqlConn, stringstream &querySStr);
};

#endif /* FACESWITHPOSEQUERY_H_ */
