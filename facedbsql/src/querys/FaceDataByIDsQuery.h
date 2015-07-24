/*
 * FaceDataByIDsQuery.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEDATABYIDSQUERY_H_
#define FACEDATABYIDSQUERY_H_

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

#include "FaceDBQuery.h"

class FaceDataByIDsQuery : public FaceDBQuery {
public:
	FaceDataByIDsQuery();
	virtual ~FaceDataByIDsQuery();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();
	//int recvData(int argc, char **argv, char **azColName);
	int dataRowReady(SQLiteDBConnection *sqlConn);

	vector<int> queryIds;
	map<int,FaceData*> data;
private:
	static const int FACEID_COLIDX;
	static const int DBID_COLIDX;
	static const int FILEID_COLIDX;
	static const int IMAGEID_COLIDX;
	static const int SEX_COLIDX;
	static const int OCCLUDED_COLIDX;
	static const int FILEPATH_COLIDX;
	static const int BW_COLIDX;
	static const int WIDTH_COLIDX;
	static const int HEIGHT_COLIDX;
	static const int YAW_COLIDX;
	static const int PITCH_COLIDX;
	static const int ROLL_COLIDX;
	static const int CAMYAW_COLIDX;
	static const int CAMPITCH_COLIDX;
	static const int CAMROLL_COLIDX;
	static const int GLASSES_COLIDX;
	static const int MDBW_COLIDX;

	static const int DBPATH_COLIDX;

	static const int ELLIPSE_X_COLIDX;
	static const int ELLIPSE_Y_COLIDX;
	static const int ELLIPSE_RA_COLIDX;
	static const int ELLIPSE_RB_COLIDX;
	static const int ELLIPSE_THETA_COLIDX;
	static const int ELLIPSE_ANNOTTYPE_COLIDX;
	static const int ELLIPSE_UPSIDEDOWN_COLIDX;

	static const int METADATA_ANNOTTYPE_COLIDX;
	static const int POSE_ANNOTTYPE_COLIDX;

	static const int MIN_COLIDX;
	static const int MAX_COLIDX;
};


#endif /* FACEDATABYIDSQUERY_H_ */
