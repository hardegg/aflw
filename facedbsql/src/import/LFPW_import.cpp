/*
 * LFPW_import.cpp
 *
 *  Created on: 08.10.2012 
 *      Author: Elena Wirtl
 */


#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"
#include "../facedbtool/FacePoseCalculator.cpp"
#include "../facedbtool/FaceStuffCalculator.cpp"
#include "../facedbtool/FaceRectCalculator.cpp"
#include "../facedbtool/FaceEllipsesCalculator.cpp"

#include <algorithm>

#include "boost/program_options.hpp"

#include "boost/filesystem.hpp"

#include "cv.h"
#include "highgui.h"

#include "LFPWFilesReader.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;


int getAnnotTypeID(SQLiteDBConnection* sqlConn)
{   
	int annotTypeID_LFPW = 0;
	SQLiteStmt *findStmt = sqlConn->prepare("SELECT annot_type_id FROM annotationtype WHERE CODE like 'lfpw' ");
	int res = sqlConn->step(findStmt);

	
	if (res == SQLITE_ROW)
	{
		if (!findStmt->readIntColumn(0,annotTypeID_LFPW))
			throw runtime_error("error: couldn't read annotation type id for lfpw from database");
	}
	else
	{
		// no entry for LFPW in db -> create it
		SQLiteStmt *findMaxStmt = sqlConn->prepare("SELECT max(annot_type_id) FROM AnnotationType");
		res = sqlConn->step(findMaxStmt);
		if (res == SQLITE_ROW)
		{
			int maxID = 0;
			findMaxStmt->readIntColumn(0,maxID);
			annotTypeID_LFPW = maxID+1;
			SQLiteStmt *insertStmt = sqlConn->prepare("INSERT INTO AnnotationType(annot_type_id,description,CODE) VALUES (?1,?2,?3)");
			insertStmt->bind(1,annotTypeID_LFPW);
			insertStmt->bind(2,"Imported from LFPW Profile Database");
			insertStmt->bind(3,"lfpw");
			if (!sqlConn->step(insertStmt))
				throw runtime_error("error: couldn't insert annotation type id for lfpw into database");
			delete insertStmt;
		}
		else
			throw runtime_error("error: couldn't read max annotation type id from database");
		//sqlConn->finalize(findMaxStmt);
		delete findMaxStmt;
	}
	//sqlConn->finalize(findStmt);
	delete findStmt;

	return annotTypeID_LFPW;
}



int main( int argc, char* argv[] )
{
	cout << "Importing Data from Labeled Face Parts in the Wild (LFPW) Dataset into SQLite3 DB " << endl;

	//----------------- Program Options

	po::positional_options_description pod;
	pod.add("sqldb-file",1);
	pod.add("lfpwfile-dir",1);

	po::options_description desc("Options");
	desc.add_options()
		("help","produce help message")
		("sqldb-file", po::value< string >(), "sql database file")
		("lfpwfile-dir", po::value< string >(), "directory containing files with the feature coordinates");

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
	po::notify(vm);


	bool showHelp = false;

	string sqlDBFile;
	if (vm.count("sqldb-file"))
		sqlDBFile = vm["sqldb-file"].as<string>();
	else
		showHelp = true;

	string imgDBName = "lfpw";

	string lfpwFilesDir;
	if (vm.count("lfpwfile-dir"))
		lfpwFilesDir = vm["lfpwfile-dir"].as<string>();
	else
		showHelp = true;
		
  

	if (vm.count("help") || showHelp)
	{
	    cout << desc << "\n";
	    return 1;
	}

	//std::string basePath = "";
	//std::string::size_type pos = sqlDBFile.find_last_of("\\/");
	//if (pos != std::string::npos)
	//{
	//	basePath = sqlDBFile.substr(0,pos+1);
	//}


	cout << "-------------------------------------------------------" << endl;
	cout << "  Config:" << endl;
	cout << "    SQL DB Name:   " << sqlDBFile << endl;
	cout << "    Features File Dir: " << lfpwFilesDir << endl;
	cout << "-------------------------------------------------------" << endl;


	//------------------ INSERT INTO sql db

	SQLiteDBConnection sqlConn;

	if (sqlConn.open(sqlDBFile)) {

		int annotTypeID_LFPW = getAnnotTypeID(&sqlConn);

		LFPWFilesReader LFPWFilesReader;
		LFPWFilesReader.load(lfpwFilesDir,annotTypeID_LFPW);

		string insertFaceSqlStmtStr = "INSERT INTO Faces(file_id,db_id) VALUES (?1,?2)";
		SQLiteStmt *insertFaceSqlStmt = sqlConn.prepare(insertFaceSqlStmtStr);

        string insertDBSqlStmtStr = "INSERT INTO databases(db_id,path,description) VALUES (?1, ?2, ' Labeled Face Parts in the Wild (LFPW) Dataset ')";
		SQLiteStmt *insertDBSqlStmt = sqlConn.prepare(insertDBSqlStmtStr);

		string insertFeatureCoordsSqlStmtStr = "INSERT INTO FeatureCoords(face_id,feature_id,x,y) VALUES (?1, ?2, ?3, ?4)";
		SQLiteStmt *insertFeatureCoordsSqlStmt = sqlConn.prepare(insertFeatureCoordsSqlStmtStr);

		string insertPoseSqlStmtStr = "INSERT INTO FacePose(face_id,roll,pitch,yaw) VALUES (?1, ?2, ?3, ?4)";
		SQLiteStmt *insertPoseSqlStmt = sqlConn.prepare(insertPoseSqlStmtStr);

		string insertImageSqlStmtStr = "INSERT INTO FaceImages(db_id,file_id,filepath,bw,width,height) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		SQLiteStmt *insertImageSqlStmt = sqlConn.prepare(insertImageSqlStmtStr);

		std::string insertMetaDataSqlStmtStr = "INSERT INTO FaceMetadata(face_id,sex,occluded,glasses,bw,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5,?6)";
		SQLiteStmt *insertMetaDataSqlStmt = sqlConn.prepare(insertMetaDataSqlStmtStr);
		

		bool allOK = true;
		
		//If db entry for lfpw does not exists then create it
	    SQLiteStmt *findStmt = sqlConn.prepare("SELECT db_id FROM databases WHERE db_id like 'lfpw' ");
	    
	    int res = sqlConn.step(findStmt);

	   
	   if (res != SQLITE_ROW)
	   {
	       insertDBSqlStmt->bind(1,imgDBName); //name of the database
	       insertDBSqlStmt->bind(2,imgDBName+"/"); //directory for the pictures of the db
		   allOK = allOK && (sqlConn.step(insertDBSqlStmt) == SQLITE_DONE);
	    }   
	       
	       //The highest needed featureCoord number is 41
	       int maxID = 0;
	       SQLiteStmt *findMaxStmt = sqlConn.prepare("SELECT max(feature_id) FROM FeatureCoordTypes");
	       sqlConn.step(findMaxStmt);
	       findMaxStmt->readIntColumn(0,maxID);

		if ((maxID >= 41) && insertFeatureCoordsSqlStmt && allOK)
		{
			sqlConn.exec("PRAGMA synchronous = OFF;");
			sqlConn.exec("BEGIN TRANSACTION;");

			int numFaces = LFPWFilesReader.numFaces();
			
			// insert images
			std::vector<std::string> listedImages;
			std::vector<std::string> validImages;
			for (int i = 0; i < numFaces; ++i)
			{
			    //get the filename and filepath
				std::string imageFilename = LFPWFilesReader.getFileName(i);
				std::string imageFilepath = LFPWFilesReader.getFilePath(i);
				std::string totalFileName = LFPWFilesReader.getTotalFileName(i);

				//std::string dbPath = basePath + imgDBName + "/";
				
				//TODO what is that find thingy? do I need it?
				// all comments which aren't really comments delete!
               	//if (find(listedImages.begin(),listedImages.end(),imageFilepath) == listedImages.end())
				
					listedImages.push_back(imageFilepath);
					
                    imageFilepath = imageFilepath + "/" + imageFilename;
					
				
					cout << "Reading: '" << totalFileName << "'" << endl;
					cv::Mat img = cv::imread(totalFileName,-1);

					if ((img.cols > 0) && (img.rows > 0))
					{
						validImages.push_back(totalFileName);

						cout << "   inserting " << imageFilename << std::endl;
						//db_id,file_id,filepath,bw,width,height
						insertImageSqlStmt->bind(1,imgDBName);
						insertImageSqlStmt->bind(2,imageFilename);
						insertImageSqlStmt->bind(3,imageFilepath);
						insertImageSqlStmt->bind(4,img.channels() == 3);
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

			for (int i = 0; i < numFaces; ++i)
			{
				std::string imageFilepath = LFPWFilesReader.getTotalFileName(i);
				if (find(validImages.begin(),validImages.end(),imageFilepath) == validImages.end())
					continue; // not found in valid images

				std::string imageFilename = LFPWFilesReader.getFileName(i);

				cout << "Inserting Face " << i << " on " << imageFilename << " ... ";

				FeaturesCoords fc = LFPWFilesReader.getFeatureCoords(i);
				insertFaceSqlStmt->reset();
				insertFaceSqlStmt->bind(1,imageFilename);
				insertFaceSqlStmt->bind(2,imgDBName);
				allOK = allOK && (sqlConn.step(insertFaceSqlStmt) == SQLITE_DONE);
				if (!allOK)
					break;

				// get database id of inserted face
				int face_id = sqlConn.getLastInsertRowid();

				// insert meta data
				insertMetaDataSqlStmt->reset();
				insertMetaDataSqlStmt->bind(1,face_id);
				insertMetaDataSqlStmt->bind(2,FaceMetadata::UNDEFINED); // sex
				
				if(LFPWFilesReader.getFileVisibility(i))
				  insertMetaDataSqlStmt->bind(3,FaceMetadata::UNCHECKED); // occluded
				else
				  insertMetaDataSqlStmt->bind(3,FaceMetadata::CHECKED); // occluded
				  
				insertMetaDataSqlStmt->bind(4,FaceMetadata::UNCHECKED); // glasses
				insertMetaDataSqlStmt->bind(5,FaceMetadata::UNCHECKED); // black/white
				insertMetaDataSqlStmt->bind(6,annotTypeID_LFPW); 

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
            
           
			delete insertFeatureCoordsSqlStmt;
			
		}
		
		//}
		//else
		//{
		//   cout << "Database " << imgDBName << " excists already. " << endl;
		//}
	}

	cout << "done." << endl;
}

