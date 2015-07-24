/*
 * FaceIDByImageQuery.cpp
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#include "FileIDOfImagesWithFaces.h"

#include <sstream>
#include <string>

FileIDOfImagesWithFaces::FileIDOfImagesWithFaces() : img_db_id("")
{
}

FileIDOfImagesWithFaces::~FileIDOfImagesWithFaces()
{
}


void FileIDOfImagesWithFaces::clear()
{
	image_face_ids.clear();
}


bool FileIDOfImagesWithFaces::exec(SQLiteDBConnection *sqlConn)
{
	clear();
	std::stringstream sqlQuerySStr;
	sqlQuerySStr << "SELECT face_id,file_id FROM faces WHERE db_id = '" << img_db_id << "' GROUP BY face_id";
	//SELECT face_id,file_id FROM Faces GROUP BY face_id HAVING count(face_id) > 0

	_query = sqlQuerySStr.str();
	//cout << "FileIDOfImagesWithFaces: " << _query << endl;
	bool ok = (sqlConn->exec(_query,this) == SQLITE_OK);
	return ok;
}


int FileIDOfImagesWithFaces::dataRowReady(SQLiteDBConnection *sqlConn)
{
	int face_id;
	std::string file_id;
	if (sqlConn->readIntColumn(0,face_id) && sqlConn->readStringColumn(1,file_id))
		image_face_ids[file_id] = face_id;
	return 0;
}


void FileIDOfImagesWithFaces::debugPrint()
{

}


