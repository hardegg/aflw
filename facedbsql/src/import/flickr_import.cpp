/*
 * flickr_import.cpp
 *
 *  Created on: 28.07.2010
 *      Author: pwohlhart
 */

#define BOOST_FILESYSTEM_VERSION 3


#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"

#include <map>
#include <algorithm>

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include "cv.h"
#include "highgui.h"

#include "FeaturesFileReader.h"
#include "PoseFileReader.h"
#include "MetaDataFileReader.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main( int argc, char* argv[] )
{
	cout << "Importing Data from Annotation Tool into SQLite3 DB " << endl;

	//----------------- Program Options

	po::positional_options_description pod;
	pod.add("sqldb-file",1);
	pod.add("imgdbname",1);
	pod.add("feat-file",1);
	pod.add("pose-file",1);
	pod.add("meta-file",1);

	po::options_description desc("Options");
	desc.add_options()
		("help","produce help message")
		("sqldb-file", po::value< string >(), "sql database file")
		("imgdbname", po::value< string >(), "image database name")
		("feat-file", po::value< string >(), "features file (features.txt)")
		("pose-file", po::value< string >(), "pose file (angles.txt)")
		("meta-file", po::value< string >(), "meta data file (metaData.txt)")
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

	string featFile;
	if (vm.count("feat-file"))
		featFile = vm["feat-file"].as<string>();
	else
		showHelp = true;

	string poseFile;
	if (vm.count("pose-file"))
		poseFile = vm["pose-file"].as<string>();
	else
		showHelp = true;

	string metaFile;
	if (vm.count("meta-file"))
		metaFile = vm["meta-file"].as<string>();
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
	cout << "    Features File: " << featFile << endl;
	cout << "    Pose File:     " << poseFile << endl;
	cout << "    MetaData File: " << metaFile << endl;
	cout << "    Base Path:     " << basePath << endl;
	cout << "-------------------------------------------------------" << endl;

	FeaturesFileReader featuresFileReader;
	featuresFileReader.load(featFile);

	PoseFileReader poseFileReader;
	poseFileReader.load(poseFile);

	MetaDataFileReader metaDataFileReader;
	metaDataFileReader.load(metaFile);

	const vector<FeaturesFileLineData> &fd = featuresFileReader.data;
	vector<int> numfaces(fd.size(),0);

	for(int counter = 0; counter < fd.size(); ++counter)
	{
		numfaces.at(counter) = std::count( fd.begin(), fd.end(), fd.at(counter).imageFilename );
	}
	
	if (featuresFileReader.data.size() != poseFileReader.data.size())
	{
		cout << "WARNING: '" << featFile << "' and '" << poseFile << "' do not have the same number of data records" << endl;
		cout << "          " << featuresFileReader.data.size() << "' != '" << poseFileReader.data.size() << endl;
		//return 1;
	}

	//------------------ INSERT INTO sql db

	SQLiteDBConnection sqlConn;

	if (sqlConn.open(sqlDBFile))
	{
		// get the feature IDs from the database - matching the feature codes from the input file
		FeatureCoordTypes fct;
		fct.load(&sqlConn);
		//fct.debugPrint();

		vector<int> featIDs;
		for (unsigned int i = 0; i < FeaturesFileLineData::numFeaturesFileFeatureCodes; ++i)
		{
			string featCode = FeaturesFileLineData::featuresFileFeatureCodes[i];
			int featID = fct.getIDByCode(featCode);
			if (featID < 0)
				cout << "Error: No ID for feature '" << featCode << "'" << endl;
			else
				featIDs.push_back(featID);
		}

		string insertFaceSqlStmtStr = "INSERT INTO Faces(file_id,db_id) VALUES (?1,?2)";
		SQLiteStmt *insertFaceSqlStmt = sqlConn.prepare(insertFaceSqlStmtStr);

		string insertFeatureCoordsSqlStmtStr = "INSERT INTO FeatureCoords(face_id,feature_id,x,y) VALUES (?1, ?2, ?3, ?4)";
		SQLiteStmt *insertFeatureCoordsSqlStmt = sqlConn.prepare(insertFeatureCoordsSqlStmtStr);

		string insertPoseSqlStmtStr = "INSERT INTO FacePose(face_id,roll,pitch,yaw) VALUES (?1, ?2, ?3, ?4)";
		SQLiteStmt *insertPoseSqlStmt = sqlConn.prepare(insertPoseSqlStmtStr);

		string insertImageSqlStmtStr = "INSERT INTO FaceImages(db_id,file_id,filepath,bw,width,height) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		SQLiteStmt *insertImageSqlStmt = sqlConn.prepare(insertImageSqlStmtStr);

		std::string insertMetaDataSqlStmtStr = "INSERT INTO FaceMetadata(face_id,sex,occluded,glasses,bw,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		SQLiteStmt *insertMetaDataSqlStmt = sqlConn.prepare(insertMetaDataSqlStmtStr);

		bool allOK = true;

		if (insertFaceSqlStmt && insertFeatureCoordsSqlStmt)
		{
			sqlConn.exec("PRAGMA synchronous = OFF;");
			sqlConn.exec("BEGIN TRANSACTION;");

			vector<FeaturesFileLineData>::iterator featIt = featuresFileReader.data.begin();
			vector<int>::iterator numFacesIt = numfaces.begin();
			//vector<PoseFileLineData>::iterator poseIt = poseFileReader.data.begin();
			//for ( ; (featIt != featuresFileReader.data.end()) && (poseIt != poseFileReader.data.end()); ++featIt, ++poseIt, ++numFacesIt)
			for ( ; featIt != featuresFileReader.data.end(); ++featIt, ++numFacesIt)
			{
				//cout << it->imageFilename << ", " << it->coords.size() << endl;

				//cout << sqlStmtSStr.str() << endl;
				cout << "Inserting " << featIt->imageFilename << " ... ";

				//if (featIt->imageFilename != poseIt->imageFilename)
				//{
				//	cout << "ERROR: " << "img filenames '" << featIt->imageFilename << "' and '" << poseIt->imageFilename << "' do not match" << endl;
				//	continue;
				//}

				insertFaceSqlStmt->reset();
				insertFaceSqlStmt->bind(1,featIt->imageFilename);
				insertFaceSqlStmt->bind(2,imgDBName);
				allOK = allOK && (sqlConn.step(insertFaceSqlStmt) == SQLITE_DONE);
				if (!allOK)
					break;

				// get database id of inserted face
				int face_id = sqlConn.getLastInsertRowid();

				// insert pose
				//insertPoseSqlStmt->reset();
				//insertPoseSqlStmt->bind(1,face_id);
				//insertPoseSqlStmt->bind(2,poseIt->roll);
				//insertPoseSqlStmt->bind(3,poseIt->pitch);
				//insertPoseSqlStmt->bind(4,poseIt->yaw);
				//allOK = allOK && (sqlConn.step(insertPoseSqlStmt) == SQLITE_DONE);
				//if (!allOK)
				//	break;

				// insert meta data			
				std::map<std::string, MetaDataFileLineData>::const_iterator mdIT = metaDataFileReader.data.find(featIt->imageFilename);
					
				insertMetaDataSqlStmt->reset();
				insertMetaDataSqlStmt->bind(1,face_id);

				if(mdIT!=metaDataFileReader.data.end() && *numFacesIt == 1)
				{
					insertMetaDataSqlStmt->bind(2,mdIT->second.sex);
					insertMetaDataSqlStmt->bind(3,mdIT->second.occluded);
					insertMetaDataSqlStmt->bind(4,mdIT->second.glasses);
					insertMetaDataSqlStmt->bind(5,mdIT->second.bw);
					insertMetaDataSqlStmt->bind(6,0);
				}
				else
				{
					insertMetaDataSqlStmt->bind(2,FaceMetadata::UNDEFINED);
					insertMetaDataSqlStmt->bind(3,FaceMetadata::UNCHECKED);
					insertMetaDataSqlStmt->bind(4,FaceMetadata::UNCHECKED);
					insertMetaDataSqlStmt->bind(5,FaceMetadata::UNCHECKED);
					insertMetaDataSqlStmt->bind(6,0);
				}

				allOK = allOK && (sqlConn.step(insertMetaDataSqlStmt) == SQLITE_DONE);
				if (!allOK)
					break;

				// insert features
				insertFeatureCoordsSqlStmt->bind(1,face_id);
				for (unsigned int i = 0; i < featIt->coords.size(); ++i)
				{
					if ((featIt->coords[i].first > 0) && (featIt->coords[i].second > 0))
					{
						insertFeatureCoordsSqlStmt->bind(2,featIDs[i]);
						insertFeatureCoordsSqlStmt->bind(3,featIt->coords[i].first);
						insertFeatureCoordsSqlStmt->bind(4,featIt->coords[i].second);
						allOK = allOK && (sqlConn.step(insertFeatureCoordsSqlStmt) == SQLITE_DONE);
						if (!allOK)
							break;
						insertFeatureCoordsSqlStmt->reset();
					}
				}

				cout << "done" << endl;
			}

			// insert images
			std::vector<std::string> faceImages;
			featIt = featuresFileReader.data.begin();
			for ( ; (featIt != featuresFileReader.data.end()); ++featIt)
			{
				std::string dbPath = "";
				std::string filePath = featIt->imageFilePath;
				std::string::size_type pos = filePath.find(imgDBName);
				if (pos != std::string::npos)
				{
					dbPath = basePath + imgDBName + "/";
					filePath = filePath.substr(pos+imgDBName.length()+1);

					pos = filePath.find("\\"); // find first space
					while ( pos != string::npos )
					{
						filePath.replace( pos, 1, "/" );
						pos = filePath.find( "\\", pos + 1 );
					}
				}

				if (find(faceImages.begin(),faceImages.end(),filePath) == faceImages.end())
				{
					faceImages.push_back(filePath);

					std::string totalFileName = dbPath + filePath;
					fs::path tmppath(totalFileName);
					totalFileName = tmppath.parent_path().string() + "/" + featIt->imageFilename;
					//totalFileName = tmppath.native_directory_string();

					cout << "Reading: '" << totalFileName << "'" << endl;
					cv::Mat img = cv::imread(totalFileName,-1);

					//db_id,file_id,filepath,bw,width,height
					insertImageSqlStmt->bind(1,imgDBName);
					insertImageSqlStmt->bind(2,featIt->imageFilename);
					insertImageSqlStmt->bind(3,filePath);
					insertImageSqlStmt->bind(4,img.channels() == 1);
					insertImageSqlStmt->bind(5,img.cols);
					insertImageSqlStmt->bind(6,img.rows);
					allOK = allOK && (sqlConn.step(insertImageSqlStmt) == SQLITE_DONE);
					if (!allOK)
						break;
					insertImageSqlStmt->reset();

                     std::cout << " imgDBName: " << imgDBName << "\n filename: " << 
                        featIt->imageFilename << "\n filePath: " << filePath << 
                        "\n channels: " << img.channels() << " cols: " << img.cols << 
                        " rows: " << img.rows << std::endl;
				}
			}


			if (allOK)
				sqlConn.exec("COMMIT;");
			else
				sqlConn.exec("ROLLBACK TRANSACTION;");

			sqlConn.exec("PRAGMA synchronous = NORMAL;");

			sqlConn.finalize(insertFaceSqlStmt);
			delete insertFaceSqlStmt;
			sqlConn.finalize(insertFeatureCoordsSqlStmt);
			delete insertFeatureCoordsSqlStmt;
			sqlConn.finalize(insertPoseSqlStmt);
			delete insertPoseSqlStmt;
		}

		sqlConn.close();
	}
	else
		cout << "Failed to open sql db file '" << sqlDBFile << "'" << endl;

	cout << "done." << endl;
}
