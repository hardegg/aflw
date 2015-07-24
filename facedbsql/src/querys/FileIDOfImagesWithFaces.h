/*
 * FaceIDByPoseQuery.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FILEIDOFIMAGESWITHFACES_H_
#define FILEIDOFIMAGESWITHFACES_H_

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"

#include "FaceDBQuery.h"

class FileIDOfImagesWithFaces: public FaceDBQuery {
public:
	FileIDOfImagesWithFaces();
	virtual ~FileIDOfImagesWithFaces();

	bool exec(SQLiteDBConnection *sqlConn);
	void clear();

	int recvData(int argc, char **argv, char **azColName);
	int dataRowReady(SQLiteDBConnection *sqlConn);

	std::map<std::string,int> image_face_ids;

	std::string img_db_id;

	void debugPrint();
private:

};

#endif /* FACEIDBYPOSEQUERY_H_ */
