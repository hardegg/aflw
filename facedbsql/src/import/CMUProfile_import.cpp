/*
 * flickr_import.cpp
 *
 *  Created on: 28.07.2010
 *      Author: pwohlhart
 */


#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"

#include <map>
#include <algorithm>

#include "boost/program_options.hpp"

//#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "boost/filesystem.hpp"

#include "cv.h"
#include "highgui.h"

#include "CMUGTFilesReader.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;



int getAnnotTypeID(SQLiteDBConnection* sqlConn)
{
	int annotTypeID_CMU = 0;
	SQLiteStmt *findStmt = sqlConn->prepare("SELECT annot_type_id FROM AnnotationType WHERE CODE=CMU");
	int res = sqlConn->step(findStmt);
	if (res == SQLITE_ROW)
	{
		if (!findStmt->readIntColumn(0,annotTypeID_CMU))
			throw runtime_error("error: couldn't read annotation type id for CMU from database");
	}
	else
	{
		// no entry for CMU in db -> create it
		SQLiteStmt *findMaxStmt = sqlConn->prepare("SELECT max(annot_type_id) FROM AnnotationType");
		res = sqlConn->step(findMaxStmt);
		if (res == SQLITE_ROW)
		{
			int maxID = 0;
			findMaxStmt->readIntColumn(0,maxID);
			annotTypeID_CMU = maxID+1;
			SQLiteStmt *insertStmt = sqlConn->prepare("INSERT INTO AnnotationType(annot_type_id,description,CODE) VALUES (?1,?2,?3)");
			insertStmt->bind(1,annotTypeID_CMU);
			insertStmt->bind(2,"Imported from CMU Profile Database");
			insertStmt->bind(3,"CMU");
			if (!sqlConn->step(insertStmt))
				throw runtime_error("error: couldn't insert annotation type id for CMU into database");
			delete insertStmt;
		}
		else
			throw runtime_error("error: couldn't read max annotation type id from database");
		//sqlConn->finalize(findMaxStmt);
		delete findMaxStmt;
	}
	//sqlConn->finalize(findStmt);
	delete findStmt;

	return annotTypeID_CMU;
}



int main( int argc, char* argv[] )
{
	cout << "Importing Data from Annotation Tool into SQLite3 DB " << endl;

	//----------------- Program Options

	po::positional_options_description pod;
	pod.add("sqldb-file",1);
	pod.add("imgdbname",1);
	pod.add("feat-file",1);

	po::options_description desc("Options");
	desc.add_options()
		("help","produce help message")
		("sqldb-file", po::value< string >(), "sql database file")
		("imgdbname", po::value< string >(), "image database name")
		("gtfile-dir", po::value< string >(), "directory containing groundtruth files")
	;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
	po::notify(vm);


	bool showHelp = false;

	string sqlDBFile;
	if (vm.count("sqldb-file"))
		sqlDBFile = vm["sqldb-file"].as<string>();
	else
		showHelp = true;

	string imgDBName;
	if (vm.count("imgdbname"))
		imgDBName = vm["imgdbname"].as<string>();
	else
		showHelp = true;

	string gtFilesDir;
	if (vm.count("gtfile-dir"))
		gtFilesDir = vm["gtfile-dir"].as<string>();
	else
		showHelp = true;

	if (vm.count("help") || showHelp)
	{
	    cout << desc << "\n";
	    return 1;
	}

	//----------------- Gemma, geht scho

	std::string basePath = "";
	std::string::size_type pos = sqlDBFile.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		basePath = sqlDBFile.substr(0,pos+1);
	}


	cout << "-------------------------------------------------------" << endl;
	cout << "  Config:" << endl;
	cout << "    SQL DB Name:   " << sqlDBFile << endl;
	cout << "    Image DB Name: " << imgDBName << endl;
	cout << "    Groundtruth Files Dir: " << gtFilesDir << endl;
	cout << "    Base Path:     " << basePath << endl;
	cout << "-------------------------------------------------------" << endl;


	//------------------ INSERT INTO sql db

	SQLiteDBConnection sqlConn;

	if (sqlConn.open(sqlDBFile)) {

		int annotTypeID_CMU = getAnnotTypeID(&sqlConn);

		CMUGTFilesReader cmuGTFilesReader;
		cmuGTFilesReader.load(gtFilesDir,&sqlConn,annotTypeID_CMU);

		string insertFaceSqlStmtStr = "INSERT INTO Faces(file_id,db_id) VALUES (?1,?2)";
		SQLiteStmt *insertFaceSqlStmt = sqlConn.prepare(insertFaceSqlStmtStr);

		string insertFeatureCoordsSqlStmtStr = "INSERT INTO FeatureCoords(face_id,feature_id,x,y) VALUES (?1, ?2, ?3, ?4)";
		SQLiteStmt *insertFeatureCoordsSqlStmt = sqlConn.prepare(insertFeatureCoordsSqlStmtStr);

		string insertPoseSqlStmtStr = "INSERT INTO FacePose(face_id,roll,pitch,yaw) VALUES (?1, ?2, ?3, ?4)";
		SQLiteStmt *insertPoseSqlStmt = sqlConn.prepare(insertPoseSqlStmtStr);

		string insertImageSqlStmtStr = "INSERT INTO FaceImages(db_id,file_id,filepath,bw,width,height) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		SQLiteStmt *insertImageSqlStmt = sqlConn.prepare(insertImageSqlStmtStr);

		std::string insertMetaDataSqlStmtStr = "INSERT INTO FaceMetadata(face_id,sex,occluded,glasses,bw,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5,?6)";
		SQLiteStmt *insertMetaDataSqlStmt = sqlConn.prepare(insertMetaDataSqlStmtStr);

		bool allOK = true;

		if (insertFaceSqlStmt && insertFeatureCoordsSqlStmt)
		{
			sqlConn.exec("PRAGMA synchronous = OFF;");
			sqlConn.exec("BEGIN TRANSACTION;");

			int numFaces = cmuGTFilesReader.numFaces();
			// insert images
			std::vector<std::string> listedImages;
			std::vector<std::string> validImages;
			for (int i = 0; i < numFaces; ++i)
			{
				std::string imageFilename = cmuGTFilesReader.getFileName(i);
				std::string imageFilepath = cmuGTFilesReader.getFilePath(i);

				std::string dbPath = basePath + imgDBName + "/";

				if (find(listedImages.begin(),listedImages.end(),imageFilepath) == listedImages.end())
				{
					listedImages.push_back(imageFilepath);

					std::string totalFileName = dbPath + imageFilepath;
					fs::path tmppath(totalFileName);
					
					totalFileName = tmppath.parent_path().string();

					cout << "Reading: '" << totalFileName << "'" << endl;
					cv::Mat img = cv::imread(totalFileName,-1);

					if ((img.cols > 0) && (img.rows > 0))
					{
						validImages.push_back(imageFilepath);

						cout << "   inserting " << imageFilename << std::endl;
						//db_id,file_id,filepath,bw,width,height
						insertImageSqlStmt->bind(1,imgDBName);
						insertImageSqlStmt->bind(2,imageFilename);
						insertImageSqlStmt->bind(3,imageFilepath);
						insertImageSqlStmt->bind(4,img.channels() == 1);
						insertImageSqlStmt->bind(5,img.cols);
						insertImageSqlStmt->bind(6,img.rows);
						allOK = allOK && (sqlConn.step(insertImageSqlStmt) == SQLITE_DONE);
						if (!allOK)
							break;
						insertImageSqlStmt->reset();
					}
					else
						std::cout << " ! Invalid image: " << totalFileName << std::endl;
				}
			}

			for (int i = 0; i < numFaces; ++i)
			{
				std::string imageFilepath = cmuGTFilesReader.getFilePath(i);
				if (find(validImages.begin(),validImages.end(),imageFilepath) == validImages.end())
					continue; // not found in valid images

				std::string imageFilename = cmuGTFilesReader.getFileName(i);

				cout << "Inserting Face " << i << " on " << imageFilename << " ... ";

				FeaturesCoords fc = cmuGTFilesReader.getFeatureCoords(i);
				insertFaceSqlStmt->reset();
				insertFaceSqlStmt->bind(1,imageFilename);
				insertFaceSqlStmt->bind(2,imgDBName);
				allOK = allOK && (sqlConn.step(insertFaceSqlStmt) == SQLITE_DONE);
				if (!allOK)
					break;

				// get database id of inserted face
				int face_id = sqlConn.getLastInsertRowid();

				// TODO: calculate pose from features
				// insert pose
				/*
				insertPoseSqlStmt->reset();
				insertPoseSqlStmt->bind(1,face_id);
				insertPoseSqlStmt->bind(2, roll);
				insertPoseSqlStmt->bind(3, pitch);
				insertPoseSqlStmt->bind(4, yaw);
				allOK = allOK && (sqlConn.step(insertPoseSqlStmt) == SQLITE_DONE);
				if (!allOK)
					break;
				*/

				// insert meta data
				insertMetaDataSqlStmt->reset();
				insertMetaDataSqlStmt->bind(1,face_id);
				insertMetaDataSqlStmt->bind(2,FaceMetadata::UNDEFINED); // sex
				insertMetaDataSqlStmt->bind(3,FaceMetadata::UNCHECKED); // occluded
				insertMetaDataSqlStmt->bind(4,FaceMetadata::UNCHECKED); // glasses
				insertMetaDataSqlStmt->bind(5,FaceMetadata::UNCHECKED); // black/white
				insertMetaDataSqlStmt->bind(6,annotTypeID_CMU); // black/white

				allOK = allOK && (sqlConn.step(insertMetaDataSqlStmt) == SQLITE_DONE);
				if (!allOK)
					break;

				// insert features
				insertFeatureCoordsSqlStmt->bind(1,face_id);
				std::vector<int> featIDs = fc.getFeatureIds();
				for (unsigned int i = 0; i < featIDs.size(); ++i)
				{
					cv::Point2f pt = fc.getCoords(featIDs[i]);
					if ((pt.x > 0) && (pt.y > 0))
					{
						insertFeatureCoordsSqlStmt->bind(2,featIDs[i]);
						insertFeatureCoordsSqlStmt->bind(3,pt.x);
						insertFeatureCoordsSqlStmt->bind(4,pt.y);
						allOK = allOK && (sqlConn.step(insertFeatureCoordsSqlStmt) == SQLITE_DONE);
						if (!allOK)
							break;
						insertFeatureCoordsSqlStmt->reset();
					}
				}

				cout << "done" << endl;
			}

			if (allOK)
				sqlConn.exec("COMMIT;");
			else
				sqlConn.exec("ROLLBACK TRANSACTION;");

			sqlConn.exec("PRAGMA synchronous = NORMAL;");

			//sqlConn.finalize(insertFaceSqlStmt);
			delete insertFaceSqlStmt;
			//sqlConn.finalize(insertFeatureCoordsSqlStmt);
			delete insertFeatureCoordsSqlStmt;
			//sqlConn.finalize(insertPoseSqlStmt);
			delete insertPoseSqlStmt;
		}
	}

	cout << "done." << endl;
}

