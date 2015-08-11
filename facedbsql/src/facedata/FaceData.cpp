/*
 * FaceData.cpp
 *
 *  Created on: 26.07.2010
 *      Author: pwohlhart
 */

#include "FaceData.h"
#include "../querys/ImageByFileIDQuery.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <fstream>

static string BaseName(const string& path)
{
#ifdef __linux__
	char *fname;
	fname = basename((char *)path.c_str());
#else
	char drive[4096], dir[4096], fname[4096], ext[4096];
	_splitpath(path.c_str(), drive, dir, fname, ext);
#endif
	return string(fname);
}

FaceData::FaceData() : 	ID(-1), dbID(""), fileID(""), _img(0), _meta(0), _pose(0), _camPose(0), _fcoords(0)
{
}

FaceData::FaceData(const FaceData &fd) : _img(0), _meta(0), _pose(0), _camPose(0), _fcoords(0)
{
	ID = fd.ID;
	dbID = fd.dbID;
	fileID = fd.fileID;
	if (fd.getDbImg())
		_img = new FaceDbImage(*fd.getDbImg());
	if (fd.getMetadata())
		_meta = new FaceMetadata(*fd.getMetadata());
	if (fd.getPose())
		_pose = new FacePose(*fd.getPose());
	if (fd.getCamPose())
		_camPose = new Pose(*fd.getCamPose());
	if (fd.getFeaturesCoords())
		//_fcoords = new FeaturesCoords(this,*fd.getFeaturesCoords());
		_fcoords = new FeaturesCoords(*fd.getFeaturesCoords());
	if (fd.hasEllipse())
		setEllipses(fd.getEllipses());
}

FaceData::~FaceData()
{
	if (_img)
		delete _img;
	if (_meta)
		delete _meta;
	if (_pose)
		delete _pose;
	if (_camPose)
		delete _camPose;
	if (_fcoords)
		delete _fcoords;
}


void FaceData::setDbImg(FaceDbImage *img)
{
	if (_img)
		delete _img;
	_img = img;
}

void FaceData::setMetadata(FaceMetadata *metadata)
{
	if (_meta)
		delete _meta;
	_meta = metadata;
}

void FaceData::setPose(FacePose *pose)
{
	if (_pose)
		delete _pose;
	_pose = pose;
}

void FaceData::setCamPose(Pose *camPose)
{
	if (_camPose)
		delete _camPose;
	_camPose = camPose;
}

void FaceData::setFeatureCoords(FeaturesCoords *fcoords)
{
	if (_fcoords)
		delete _fcoords;
	_fcoords = fcoords;
}

void FaceData::SaveFaceRegion_rect(const string& rootFolder, const string& dstFolderPath, vector<ofstream*>& ofDetects, vector<ofstream*>& ofCalibs)
{
    static int fileNo = 0;
    stringstream sss;
    // Because the file name might duplicate
    sss << BaseName(_img->filepath) << "_" << fileNo;   
    string baseName = sss.str();
    ++fileNo;
    
    vector<FaceRect> rects = this->getRects();
    string imgPath = rootFolder + "/" + _img->filepath;
    cv::Mat img = cv::imread(imgPath);
    if (img.empty())
        return;

    double s[5] = {0.83, 0.91, 1.0, 1.10, 1.21};
    double x[3] = {-0.17, 0, 0.17};
    double y[3] = {-0.17, 0, 0.17};

    string calibFolder = dstFolderPath + "/" + "calibration";
    string detecFolder = dstFolderPath + "/" + "detection";
    
    /*
    std::ofstream ofDetect;
    ofDetect.open(string(detecFolder + "/filelist.txt").c_str(), std::ofstream::out);
    std::ofstream ofCalib;
    ofCalib.open(string(calibFolder + "/filelist.txt").c_str(), std::ofstream::out);
     */

    static int nInvalids = 0;   
    for (int i = 0; i < rects.size(); i++) {
        cv::Rect rect = cv::Rect(rects[i].x, rects[i].y, rects[i].w, rects[i].h);
        cv::Rect imgRect = cv::Rect(0, 0, img.cols, img.rows);                 
                
        for (int is = 0; is < 5; is++) {
            for (int ix = 0; ix < 3; ix++) {
                for (int iy = 0; iy < 3; iy++) {
                    int rx = cvRound(rect.x - x[ix]*rect.width/s[is]);
                    int ry = cvRound(rect.y - y[iy]*rect.height/s[is]);
                    int rw = cvRound(rect.width/s[is]);
                    int rh = cvRound(rect.height/s[is]);
                    cv::Rect ptbRect = cv::Rect(rx, ry, rw, rh);
                    
                    cv::Rect newrect = ptbRect & imgRect;
                    //Skip if the rect is out of image range 
                    if (newrect.area() < ptbRect.area())
                        continue;
                    cv::Mat patch = img(ptbRect);
                    cv::Mat patch12, patch24, patch48;
                    cv::resize(patch, patch12, cv::Size(12, 12));
                    cv::resize(patch, patch24, cv::Size(24, 24));
                    cv::resize(patch, patch48, cv::Size(48, 48));  

                    int calibLabel = is*3*3 + ix*3 + iy;
                    
                    stringstream ss;
                    
                    ss.str(std::string());
                    ss << calibFolder <<"/12x12"<< "/" << baseName << "_" << i << "_" << calibLabel << ".bmp";
                    cv::imwrite(ss.str(), patch12);
                    *ofCalibs[0] << ss.str() << " " << calibLabel << endl;

                    ss.str(std::string());
                    ss << calibFolder <<"/24x24"<< "/" << baseName << "_" << i << "_" << calibLabel << ".bmp";
                    cv::imwrite(ss.str(), patch24);
                    *ofCalibs[1] << ss.str() << " " << calibLabel << endl;

                    ss.str(std::string());
                    ss << calibFolder <<"/48x48"<< "/" << baseName << "_" << i << "_" << calibLabel << ".bmp";                    
                    cv::imwrite(ss.str(), patch48);
                    *ofCalibs[2] << ss.str() << " " << calibLabel << endl;

                    if (is == 2 && ix == 1 && iy == 1) {
                        ss.str(std::string());
                        ss << detecFolder <<"/12x12"<< "/" << baseName << "_" << i << ".bmp";                        
                        cv::imwrite(ss.str(), patch12);
                        *ofDetects[0] << ss.str() << " " << 1 << endl;
                        ss.str(std::string());
                        ss << detecFolder <<"/24x24"<< "/" << baseName << "_" << i << ".bmp";
                        *ofDetects[1] << ss.str() << " " << 1 << endl;
                        cv::imwrite(ss.str(), patch24);
                        ss.str(std::string());
                        ss << detecFolder <<"/48x48"<< "/" << baseName << "_" << i << ".bmp";                        
                        *ofDetects[2] << ss.str() << " " << 1 << endl;
                        cv::imwrite(ss.str(), patch48);
                    }                 
                    
                }
            }
        }
    }
    
}

void FaceData::debugPrint()
{
	cout << "Face (id = " << ID << ")" << endl;
	cout << "  db:   " << dbID << endl;
	cout << "  file: " << fileID << endl;
	if (_meta)
	{
		cout << "  meta: " << endl;
		cout << "    occl: " << _meta->occluded << endl;
		cout << "    sex: " << _meta->sex << endl;
	}
	if (_img)
	{
		cout << "  img: " << endl;
		cout << "    id: " << _img->image_id << endl;
		cout << "    filepath: " << _img->filepath << endl;
		cout << "    bw: " << _img->bw << endl;
		cout << "    width: " << _img->width << endl;
		cout << "    height: " << _img->height << endl;
	}
	if (_pose)
		cout << "  pose: " << _pose->yaw << ", " << _pose->pitch << ", " << _pose->roll << endl;
	if (_camPose)
		cout << "  cam pose: " << _camPose->yaw << ", " << _camPose->pitch << ", " << _camPose->roll << endl;
	if (_fcoords)
		_fcoords->debugPrint();
}

bool FaceData::save(SQLiteDBConnection *sqlConn)
{
	//*-- check if image is registered in db *--/
	ImageByFileIDQuery imageQuery;
	imageQuery.image_id = fileID;
	imageQuery.db_id = dbID;
	imageQuery.exec(sqlConn);

	const FaceDbImage &resultImage = imageQuery.resultImage;

	bool allOK = true;

	sqlConn->exec("PRAGMA synchronous = OFF;");
	sqlConn->exec("BEGIN TRANSACTION;");

	//
	// IMAGE
	//

	if(resultImage.image_id == -1) //IF IMAGE IS UNKNOWN WE HAVE TO REGISTER IT
	{
		//*-- REGISTER IMAGE IN DB --*//
		std::string insertImageSqlStmtStr = "INSERT INTO FaceImages(db_id,file_id,filepath,bw,width,height) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		SQLiteStmt *insertImageSqlStmt = sqlConn->prepare(insertImageSqlStmtStr);

		if (!insertImageSqlStmt)
		{
			//ERROR HANDLING
		}
		
		//db_id,file_id,filepath,bw,width,height
		insertImageSqlStmt->reset();
		insertImageSqlStmt->bind(1,dbID);
		insertImageSqlStmt->bind(2,fileID);
		insertImageSqlStmt->bind(3,getDbImg()->filepath);
		insertImageSqlStmt->bind(4,getDbImg()->bw);
		insertImageSqlStmt->bind(5,getDbImg()->width);
		insertImageSqlStmt->bind(6,getDbImg()->height);

		allOK = allOK && (sqlConn->step(insertImageSqlStmt) == SQLITE_DONE);

		if(allOK)
		{
			int image_id = sqlConn->getLastInsertRowid();
			getDbImg()->image_id = image_id;
		}

		//insertImageSqlStmt->setFinalized();
		sqlConn->finalize(insertImageSqlStmt);
		delete insertImageSqlStmt;
		insertImageSqlStmt = 0;
	}

	//
	// FACE
	//

	if(this->ID == -1) //NO ID --> REGISTER
	{
		//*-- INSERT FACE, GET ID FOR IT --*//
		std::string insertFaceSqlStmtStr = "INSERT INTO Faces(file_id,db_id) VALUES (?1,?2)";
		SQLiteStmt *insertFaceSqlStmt = sqlConn->prepare(insertFaceSqlStmtStr);

		if (!insertFaceSqlStmt)
		{
			//ERROR HANDLING
		}

		insertFaceSqlStmt->reset();
		insertFaceSqlStmt->bind(1,fileID);
		insertFaceSqlStmt->bind(2,dbID);

		allOK = allOK && (sqlConn->step(insertFaceSqlStmt) == SQLITE_DONE);

		int face_id = sqlConn->getLastInsertRowid();
		ID = face_id;

		insertFaceSqlStmt->setFinalized();
		delete insertFaceSqlStmt;
		insertFaceSqlStmt = 0;
	}

	//
	// POSE INSERT OR UPDATE
	//
	
	if(getPose())
	{
		std::string insertUpdatePoseSqlStmtStr;
		SQLiteStmt *insertUpdatePoseSqlStmt;

		std::string insertPoseSqlStmtStr = "INSERT OR REPLACE INTO FacePose(face_id,roll,pitch,yaw,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5)";
		insertUpdatePoseSqlStmt = sqlConn->prepare(insertPoseSqlStmtStr);

		insertUpdatePoseSqlStmt->reset();
		insertUpdatePoseSqlStmt->bind(1,ID);
		insertUpdatePoseSqlStmt->bind(2,static_cast<float>(getPose()->roll));
		insertUpdatePoseSqlStmt->bind(3,static_cast<float>(getPose()->pitch));
		insertUpdatePoseSqlStmt->bind(4,static_cast<float>(getPose()->yaw));
		insertUpdatePoseSqlStmt->bind(5,static_cast<float>(getPose()->annotTypeID));

		allOK = allOK && (sqlConn->step(insertUpdatePoseSqlStmt) == SQLITE_DONE);

		insertUpdatePoseSqlStmt->setFinalized();
		delete insertUpdatePoseSqlStmt;
		insertUpdatePoseSqlStmt = 0;
	}

	//
	// META DATA
	//

	if(getMetadata())
	{
		std::string insertMetaDataSqlStmtStr = "INSERT OR REPLACE INTO FaceMetadata(face_id,sex,occluded,glasses,bw,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		SQLiteStmt *insertMetaDataSqlStmt = sqlConn->prepare(insertMetaDataSqlStmtStr);

		if (!insertMetaDataSqlStmt)
		{
			//ERROR HANDLING
		}

		insertMetaDataSqlStmt->reset();
		insertMetaDataSqlStmt->bind(1,ID);
		insertMetaDataSqlStmt->bind(2,getMetadata()->sex);
		insertMetaDataSqlStmt->bind(3,getMetadata()->occluded);
		insertMetaDataSqlStmt->bind(4,getMetadata()->glasses);
		insertMetaDataSqlStmt->bind(5,getMetadata()->bw);
		insertMetaDataSqlStmt->bind(6,getMetadata()->annot_type_id);

		allOK = allOK && (sqlConn->step(insertMetaDataSqlStmt) == SQLITE_DONE);

		//insertMetaDataSqlStmt->setFinalized();
		delete insertMetaDataSqlStmt;
		insertMetaDataSqlStmt = 0;
	}

	//
	// FEATURE COORDS INSERT OR UPDATE
	//

	if(getFeaturesCoords())
	{
		//*-- INSERT OR UPDATE FEATURE COORDS --*//
		std::string insertFeatureCoordsSqlStmtStr = "INSERT OR REPLACE INTO FeatureCoords(face_id,feature_id,x,y,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5)";
		SQLiteStmt *insertFeatureCoordsSqlStmt = sqlConn->prepare(insertFeatureCoordsSqlStmtStr);

		//*-- DELETE FEATURE COORDS TO AVOID (-1,-1) entries in DB --*//
		std::string deleteFeatureCoordsSqlStmtStr = "DELETE FROM FeatureCoords WHERE face_id = ?1 and feature_id = ?2";
		SQLiteStmt *deleteFeatureCoordsSqlStmt = sqlConn->prepare(deleteFeatureCoordsSqlStmtStr);
		
		if (!insertFeatureCoordsSqlStmt || !deleteFeatureCoordsSqlStmt)
		{
			//ERROR HANDLING
		}

		FeaturesCoords *fc = getFeaturesCoords();
		std::vector<int> fIds = fc->getFeatureIds();

		for(int counter = 0; counter < fIds.size(); ++counter)
		{
			int fId = fIds.at(counter);

			const cv::Point2f &tmpPoint = fc->getCoords(fId);
			if ((tmpPoint.x > 0) && (tmpPoint.y > 0))
			{
				const int annotType = fc->getAnnotType(fId);
				insertFeatureCoordsSqlStmt->reset();
				insertFeatureCoordsSqlStmt->bind(1,ID);
				insertFeatureCoordsSqlStmt->bind(2,fId);
				insertFeatureCoordsSqlStmt->bind(3,tmpPoint.x);
				insertFeatureCoordsSqlStmt->bind(4,tmpPoint.y);
				insertFeatureCoordsSqlStmt->bind(5,annotType);

				allOK = allOK && (sqlConn->step(insertFeatureCoordsSqlStmt) == SQLITE_DONE);
			}
			else if ((tmpPoint.x == -1) && (tmpPoint.y == -1)) //point deleted / undefined
			{
				deleteFeatureCoordsSqlStmt->reset();
				deleteFeatureCoordsSqlStmt->bind(1,ID);
				deleteFeatureCoordsSqlStmt->bind(2,fId);

				allOK = allOK && (sqlConn->step(deleteFeatureCoordsSqlStmt) == SQLITE_DONE);
			}
		}

		insertFeatureCoordsSqlStmt->setFinalized();
		delete insertFeatureCoordsSqlStmt;
		insertFeatureCoordsSqlStmt = 0;

		deleteFeatureCoordsSqlStmt->setFinalized();
		delete deleteFeatureCoordsSqlStmt;
		deleteFeatureCoordsSqlStmt = 0;
	}
	

	if (allOK)
		sqlConn->exec("COMMIT;");
	else
		sqlConn->exec("ROLLBACK TRANSACTION;");

	sqlConn->exec("PRAGMA synchronous = NORMAL;");

	return allOK;
}

void FaceData::loadFeatureCoords(SQLiteDBConnection *sqlConn)
{
	_fcoords = new FeaturesCoords(ID);
	_fcoords->load(sqlConn);
}

void FaceData::loadRects(SQLiteDBConnection *sqlConn)
{
	_rects.clear();

	stringstream sqlStmtStrm;
	sqlStmtStrm << "SELECT x,y,w,h,annot_type_id FROM FaceRect WHERE face_id = " << ID;

	SQLiteStmt *rectsQueryStmt = sqlConn->prepare(sqlStmtStrm.str());

	int res = 0;
	do {
		res = sqlConn->step(rectsQueryStmt);
		bool allOK = true;
		if (res == SQLITE_ROW)
		{
			FaceRect fr;
			allOK = allOK && rectsQueryStmt->readIntColumn(0,fr.x);
			allOK = allOK && rectsQueryStmt->readIntColumn(1,fr.y);
			allOK = allOK && rectsQueryStmt->readIntColumn(2,fr.w);
			allOK = allOK && rectsQueryStmt->readIntColumn(3,fr.h);
			allOK = allOK && rectsQueryStmt->readIntColumn(4,fr.annotType);
			if (allOK)
				_rects.push_back(fr);
			else
				break;
		}
	} while (res == SQLITE_ROW);

	delete rectsQueryStmt;
}

void FaceData::loadEllipses(SQLiteDBConnection *sqlConn)
{
	_ellipses.clear();

	stringstream sqlStmtStrm;
	sqlStmtStrm << "SELECT x,y,ra,rb,theta,annot_type_id,upsidedown FROM FaceEllipse WHERE face_id = " << ID;

	SQLiteStmt *rectsQueryStmt = sqlConn->prepare(sqlStmtStrm.str());

	int res = 0;
	do {
		res = sqlConn->step(rectsQueryStmt);
		bool allOK = true;
		if (res == SQLITE_ROW)
		{
			FaceEllipse fe;
			allOK = allOK && rectsQueryStmt->readFloatColumn(0,fe.x);
			allOK = allOK && rectsQueryStmt->readFloatColumn(1,fe.y);
			allOK = allOK && rectsQueryStmt->readFloatColumn(2,fe.ra);
			allOK = allOK && rectsQueryStmt->readFloatColumn(3,fe.rb);
			allOK = allOK && rectsQueryStmt->readFloatColumn(4,fe.theta);
			allOK = allOK && rectsQueryStmt->readIntColumn(5,fe.annot_type_id);
			allOK = allOK && rectsQueryStmt->readBoolColumn(6,fe.upsideDown);
			if (allOK)
				_ellipses.push_back(fe);
			else
				break;
		}
	} while (res == SQLITE_ROW);

	delete rectsQueryStmt;
}

FaceEllipse FaceData::getEllipse(int annotTypeID) const
{
	for (int i = 0; i < _ellipses.size(); ++i)
	{
		if (_ellipses[i].annot_type_id == annotTypeID)
			return _ellipses[i];
	}

	FaceEllipse fe;
	fe.annot_type_id = -1;
	return fe;
}

void FaceData::setEllipse(FaceEllipse ellipse)
{
	bool found = false;
	for (int i = 0; i < _ellipses.size(); ++i)
	{
		if (_ellipses[i].annot_type_id == ellipse.annot_type_id)
		{
			found = true;
			_ellipses[i] = ellipse;
			break;
		}
	}
	if (!found)
		_ellipses.push_back(ellipse);
	//_hasEllipse = true;
}

void FaceData::setEllipses(vector<FaceEllipse> ellipses)
{
	_ellipses = ellipses;
}

//**************************************************************************
//    FaceEllipse
//**************************************************************************

FaceEllipse::FaceEllipse() : x(-1),y(-1),ra(0),rb(0),theta(0),annot_type_id(-1),upsideDown(false)
{

}

bool FaceEllipse::equals(const FaceEllipse &other)
{
	bool same = (x == other.x) &&
			(y == other.y) &&
			(ra == other.ra) &&
			(rb == other.rb) &&
			(theta == other.theta) &&
			(annot_type_id == other.annot_type_id) &&
			(upsideDown == other.upsideDown);
	return same;
}


