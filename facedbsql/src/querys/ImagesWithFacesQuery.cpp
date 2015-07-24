/*
 * ImagesWithFacesQuery.cpp
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#include "ImagesWithFacesQuery.h"

#include <sstream>
#include <string>

ImagesWithFacesQuery::ImagesWithFacesQuery() : db_id("")
{
}

ImagesWithFacesQuery::~ImagesWithFacesQuery() {
}

void ImagesWithFacesQuery::clear()
{
	resultImages.clear();
}

bool ImagesWithFacesQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();

	std::stringstream sqlQuerySStr;
	sqlQuerySStr << "SELECT Faces.file_id,Faces.db_id,image_id,filepath,bw,width,height,databases.path as db_path";
	sqlQuerySStr << " FROM Faces,FaceImages,databases ";
	sqlQuerySStr << " WHERE Faces.db_id = FaceImages.db_id AND Faces.file_id = FaceImages.file_id AND databases.db_id = faces.db_id ";
	if (db_id == "")
		sqlQuerySStr <<  "GROUP BY Faces.file_id,Faces.db_id HAVING count(face_id) > 0";
	else
		sqlQuerySStr << " AND Faces.db_id = \"" << db_id << "\" GROUP BY Faces.file_id HAVING count(face_id) > 0";

	std::string query = sqlQuerySStr.str();
	cout << "query: " << query << endl;

	int res = sqlConn->exec(query,this);
	return res == SQLITE_OK;
}

int ImagesWithFacesQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	bool allOK = true;
	FaceDbImage img;
	allOK = allOK && sqlConn->readStringColumn(0,img.file_id);
	allOK = allOK && sqlConn->readStringColumn(1,img.db_id);
	allOK = allOK && sqlConn->readIntColumn(2,img.image_id);
	allOK = allOK && sqlConn->readStringColumn(3,img.filepath);
	allOK = allOK && sqlConn->readBoolColumn(4,img.bw);
	allOK = allOK && sqlConn->readIntColumn(5,img.width);
	allOK = allOK && sqlConn->readIntColumn(6,img.height);
	allOK = allOK && sqlConn->readStringColumn(7,img.dbpath);
	if (allOK)
		resultImages.push_back(img);
	return 0;
}

void ImagesWithFacesQuery::debugPrint()
{

}
