/*
 * ImageByFaceIDQuery.cpp
 *
 *  Created on: 02.08.2010
 *      Author: pwohlhart
 */

#include "ImageByFileIDQuery.h"

ImageByFileIDQuery::ImageByFileIDQuery() {

}

ImageByFileIDQuery::~ImageByFileIDQuery() {
}


bool ImageByFileIDQuery::exec(SQLiteDBConnection *sqlConn) {

	std::stringstream sqlQuerySStr;
	sqlQuerySStr << "SELECT image_id,faceimages.db_id as db_id,faceimages.file_id as file_id,filepath,bw,width,height,databases.path as db_path";
	sqlQuerySStr << " FROM faceimages,databases";
	sqlQuerySStr << " WHERE faceimages.file_id = \"" << image_id << "\""; 
	sqlQuerySStr << " AND faceimages.db_id = \"" << db_id << "\""; 
	sqlQuerySStr << " AND databases.db_id = faceimages.db_id";

	std::string query = sqlQuerySStr.str();
	//cout << "QUERY: " << query << endl;

	int res = sqlConn->exec(query,this);
	return res == SQLITE_OK;
}

int ImageByFileIDQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	// image_id,db_id,file_id,filepath,bw,width,height,db.path as db_path ";
	int imageID;
	std::string dbID;
	std::string fileID;
	std::string filePath;
	int bw;
	int width;
	int height;
	std::string dbPath;

	sqlConn->readIntColumn(0,imageID);
	sqlConn->readStringColumn(1,dbID);
	sqlConn->readStringColumn(2,fileID);
	sqlConn->readStringColumn(3,filePath);
	sqlConn->readIntColumn(4,bw);
	sqlConn->readIntColumn(5,width);
	sqlConn->readIntColumn(6,height);
	sqlConn->readStringColumn(7,dbPath);

	resultImage.image_id = imageID;
	resultImage.db_id = dbID;
	resultImage.file_id = fileID;
	resultImage.filepath = filePath;
	resultImage.bw = (bw == 1);
	resultImage.width = width;
	resultImage.height = height;
	resultImage.dbpath = dbPath;

	return 0;
}

void ImageByFileIDQuery::clear()
{

}
