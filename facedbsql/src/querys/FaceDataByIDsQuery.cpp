/*
 * FaceDataByIDsQuery.cpp
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#include "FaceDataByIDsQuery.h"

const int FaceDataByIDsQuery::FACEID_COLIDX = 0;
const int FaceDataByIDsQuery::DBID_COLIDX = 1;
const int FaceDataByIDsQuery::FILEID_COLIDX = 2;
const int FaceDataByIDsQuery::IMAGEID_COLIDX = 3;
const int FaceDataByIDsQuery::SEX_COLIDX = 4;
const int FaceDataByIDsQuery::OCCLUDED_COLIDX = 5;
const int FaceDataByIDsQuery::FILEPATH_COLIDX = 6;
const int FaceDataByIDsQuery::BW_COLIDX = 7;
const int FaceDataByIDsQuery::WIDTH_COLIDX = 8;
const int FaceDataByIDsQuery::HEIGHT_COLIDX = 9;
const int FaceDataByIDsQuery::YAW_COLIDX = 10;
const int FaceDataByIDsQuery::PITCH_COLIDX = 11;
const int FaceDataByIDsQuery::ROLL_COLIDX = 12;
const int FaceDataByIDsQuery::GLASSES_COLIDX = 13;
const int FaceDataByIDsQuery::MDBW_COLIDX = 14;
const int FaceDataByIDsQuery::DBPATH_COLIDX = 15;
const int FaceDataByIDsQuery::ELLIPSE_X_COLIDX = 16;
const int FaceDataByIDsQuery::ELLIPSE_Y_COLIDX = 17;
const int FaceDataByIDsQuery::ELLIPSE_RA_COLIDX = 18;
const int FaceDataByIDsQuery::ELLIPSE_RB_COLIDX = 19;
const int FaceDataByIDsQuery::ELLIPSE_THETA_COLIDX = 20;
const int FaceDataByIDsQuery::ELLIPSE_ANNOTTYPE_COLIDX = 21;
const int FaceDataByIDsQuery::ELLIPSE_UPSIDEDOWN_COLIDX = 22;
const int FaceDataByIDsQuery::METADATA_ANNOTTYPE_COLIDX = 23;
const int FaceDataByIDsQuery::POSE_ANNOTTYPE_COLIDX = 24;
const int FaceDataByIDsQuery::CAMYAW_COLIDX = 25;
const int FaceDataByIDsQuery::CAMPITCH_COLIDX = 26;
const int FaceDataByIDsQuery::CAMROLL_COLIDX = 27;

const int FaceDataByIDsQuery::MIN_COLIDX = 0;
const int FaceDataByIDsQuery::MAX_COLIDX = 27;


FaceDataByIDsQuery::FaceDataByIDsQuery()
{
}

FaceDataByIDsQuery::~FaceDataByIDsQuery()
{
	clear();
}


bool FaceDataByIDsQuery::exec(SQLiteDBConnection *sqlConn)
{
	clear();

	if (queryIds.size() == 0)
		return true;

	stringstream faceidsSStr;
	faceidsSStr << queryIds[0];
	for (unsigned int i = 1; i < queryIds.size(); ++i)
		faceidsSStr << "," << queryIds[i];

	map<int,string> colname;
	colname[FACEID_COLIDX] = "faces.face_id";
	colname[DBID_COLIDX] = "faces.db_id";
	colname[FILEID_COLIDX] = "faces.file_id";
	colname[IMAGEID_COLIDX] = "image_id";
	colname[SEX_COLIDX] = "facemetadata.sex";
	colname[OCCLUDED_COLIDX] = "facemetadata.occluded";
	colname[FILEPATH_COLIDX] = "filepath";
	colname[BW_COLIDX] = "faceimages.bw";
	colname[WIDTH_COLIDX] = "faceimages.width";
	colname[HEIGHT_COLIDX] = "faceimages.height";
	colname[YAW_COLIDX] = "yaw";
	colname[PITCH_COLIDX] = "pitch";
	colname[ROLL_COLIDX] = "roll";
	colname[CAMYAW_COLIDX] = "camyaw";
	colname[CAMPITCH_COLIDX] = "campitch";
	colname[CAMROLL_COLIDX] = "camroll";
	colname[GLASSES_COLIDX] = "facemetadata.glasses";
	colname[MDBW_COLIDX] = "facemetadata.bw";
	colname[DBPATH_COLIDX] = "databases.path";
	colname[ELLIPSE_X_COLIDX] = "faceellipse.x";
	colname[ELLIPSE_Y_COLIDX] = "faceellipse.y";
	colname[ELLIPSE_RA_COLIDX] = "faceellipse.ra";
	colname[ELLIPSE_RB_COLIDX] = "faceellipse.rb";
	colname[ELLIPSE_THETA_COLIDX] = "faceellipse.theta";
	colname[ELLIPSE_ANNOTTYPE_COLIDX] = "faceellipse.annot_type_id";
	colname[ELLIPSE_UPSIDEDOWN_COLIDX] = "faceellipse.upsidedown";
	colname[METADATA_ANNOTTYPE_COLIDX] = "facemetadata.annot_type_id";
	colname[POSE_ANNOTTYPE_COLIDX] = "facepose.annot_type_id";


	//string fields = "faces.face_id, faces.db_id, faces.file_id, image_id, sex, occluded, filepath, bw, width, height, yaw, pitch, roll";
	stringstream fieldsSStr;
	for (int i = MIN_COLIDX; i < MAX_COLIDX; ++i)
		fieldsSStr << colname[i] << ", ";
	fieldsSStr << colname[MAX_COLIDX];
	string fields = fieldsSStr.str();

	stringstream querySStr;
	querySStr << "SELECT "<< fields <<" FROM ((((((faces LEFT JOIN facemetadata on faces.face_id = facemetadata.face_id) ";
	querySStr <<   "LEFT JOIN faceimages ON faces.file_id = faceimages.file_id AND faces.db_id = faceimages.db_id) ";
	querySStr <<   "LEFT JOIN facepose ON faces.face_id = facepose.face_id) ";
	querySStr <<   "LEFT JOIN databases ON faces.db_id = databases.db_id) ";
	querySStr <<   "LEFT JOIN faceellipse ON faces.face_id = faceellipse.face_id) ";
	querySStr <<   "LEFT JOIN campose ON faces.face_id = campose.face_id) ";
	querySStr <<   "WHERE faces.face_id in (" << faceidsSStr.str() << ")";

	_query = querySStr.str();
	//cout << "QUERY: " << _query << endl;
	bool ok = (sqlConn->exec(_query,this) == SQLITE_OK);
	return ok;
}

void FaceDataByIDsQuery::clear()
{
	map<int,FaceData*>::iterator it;
	for (it = data.begin(); it != data.end(); ++it)
		delete it->second;
	data.clear();
}

//int FaceDataByIDsQuery::recvData(int argc, char **argv, char **azColName) { }

int FaceDataByIDsQuery::dataRowReady(SQLiteDBConnection *sqlConn)
{
	//cout << "row!" << endl;

	// DEBUG
	/*
	cout << "result columns: ";
	for (int i=0; i < sqlConn->getResultNumColumns(); ++i)
	{
		const char *colname = sqlConn->getResultColumnName(i);
		cout << colname << ", ";
	}
	cout << endl;
	*/

	int faceId;
	int imageId;
	string fileId;
	string dbId;
	string sex;
	int occluded;
	int glasses;
	int metaDataBw;
	int metaDataAnnotType;

	double yaw;
	double pitch;
	double roll;
	int poseAnnotType;

	double camYaw;
	double camPitch;
	double camRoll;

	int bw;
	int width;
	int height;
	string filepath;
	string dbpath;

	bool res;
	sqlConn->readIntColumn(FACEID_COLIDX,faceId);
	sqlConn->readStringColumn(DBID_COLIDX,dbId);
	sqlConn->readStringColumn(FILEID_COLIDX,fileId);

	FaceData *fd = new FaceData();
	fd->ID = faceId;
	fd->dbID = dbId;
	fd->fileID = fileId;

	res = sqlConn->readStringColumn(SEX_COLIDX,sex);
	res = res && sqlConn->readIntColumn(OCCLUDED_COLIDX,occluded);
	res = res && sqlConn->readIntColumn(GLASSES_COLIDX,glasses);
	res = res && sqlConn->readIntColumn(MDBW_COLIDX,metaDataBw);
	res = res && sqlConn->readIntColumn(METADATA_ANNOTTYPE_COLIDX,metaDataAnnotType);
	if (res)
	{
		FaceMetadata *metadata = new FaceMetadata(faceId);
		metadata->sex = sex;
		metadata->occluded = occluded;
		metadata->glasses = glasses;
		metadata->bw = metaDataBw;
		metadata->annot_type_id = metaDataAnnotType;

		fd->setMetadata(metadata);
	}

	res = sqlConn->readDoubleColumn(YAW_COLIDX,yaw);
	res = res && sqlConn->readDoubleColumn(PITCH_COLIDX,pitch);
	res = res && sqlConn->readDoubleColumn(ROLL_COLIDX,roll);
	res = res && sqlConn->readIntColumn(POSE_ANNOTTYPE_COLIDX,poseAnnotType);
	if (res)
	{
		FacePose *pose = new FacePose();
		pose->yaw = yaw;
		pose->pitch = pitch;
		pose->roll = roll;
		pose->annotTypeID = poseAnnotType;
		fd->setPose(pose);
	}

	res = sqlConn->readDoubleColumn(CAMYAW_COLIDX,camYaw);
	res = res && sqlConn->readDoubleColumn(CAMPITCH_COLIDX,camPitch);
	res = res && sqlConn->readDoubleColumn(CAMROLL_COLIDX,camRoll);
	if (res)
	{
		Pose *camPose = new Pose();
		camPose->yaw = camYaw;
		camPose->pitch = camPitch;
		camPose->roll = camRoll;
		fd->setCamPose(camPose);
	}

	res = sqlConn->readIntColumn(IMAGEID_COLIDX,imageId);
	if (res)
	{
		FaceDbImage *img = new FaceDbImage();
		img->image_id = imageId;
		img->file_id = fileId;
		img->db_id = dbId;
		if (sqlConn->readStringColumn(FILEPATH_COLIDX,filepath))
			img->filepath = filepath;
		if (sqlConn->readIntColumn(BW_COLIDX,bw))
			img->bw = bw;
		if (sqlConn->readIntColumn(WIDTH_COLIDX,width))
			img->width = width;
		if (sqlConn->readIntColumn(HEIGHT_COLIDX,height))
			img->height = height;
		if (sqlConn->readStringColumn(DBPATH_COLIDX,dbpath))
			img->dbpath = dbpath;

		fd->setDbImg(img);
	}

	//cout << "loading ellipse data" << endl;
	FaceEllipse ellipse;
	res = sqlConn->readFloatColumn(ELLIPSE_X_COLIDX,ellipse.x);
	res = res && sqlConn->readFloatColumn(ELLIPSE_Y_COLIDX,ellipse.y);
	res = res && sqlConn->readFloatColumn(ELLIPSE_RA_COLIDX,ellipse.ra);
	res = res && sqlConn->readFloatColumn(ELLIPSE_RB_COLIDX,ellipse.rb);
	res = res && sqlConn->readFloatColumn(ELLIPSE_THETA_COLIDX,ellipse.theta);
	res = res && sqlConn->readIntColumn(ELLIPSE_ANNOTTYPE_COLIDX,ellipse.annot_type_id);
	res = res && sqlConn->readBoolColumn(ELLIPSE_UPSIDEDOWN_COLIDX,ellipse.upsideDown);
	if (res)
		fd->setEllipse(ellipse);

	fd->loadRects(sqlConn);

	data[faceId] = fd;

	//cout << fd.ID << ", " << fd.dbID << ", " << fd.fileID << endl;
	return 0;
}


