/*
 * put_import.cpp
 *
 *  Created on: 28.04.2012
 *      Author: szweimueller
 */

#define BOOST_FILESYSTEM_VERSION 3


#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"

#include "../querys/DeleteFacesByIdsQuery.h"

#include <algorithm>
#include <map>
#include <vector>
#include <set>

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/format.hpp"

#include "cv.h"
#include "highgui.h"

#include "PutFileReader.h"

#include "DeleteSqlEntries.h"
#include "ImagesFromFolder.h"
#include "MetaDataFileReader.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;



int getAnnotTypeID(SQLiteDBConnection* sqlConn)
{
	int annotTypeID_PUT = 0;
	SQLiteStmt *findStmt = sqlConn->prepare("SELECT annot_type_id FROM AnnotationType WHERE CODE='PUT'");
	int res = sqlConn->step(findStmt);
	if (res == SQLITE_ROW)
	{
		if (!findStmt->readIntColumn(0,annotTypeID_PUT))
			throw runtime_error("error: couldn't read annotation type id for PUT from database");
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
			annotTypeID_PUT = maxID+1;
			SQLiteStmt *insertStmt = sqlConn->prepare("INSERT INTO AnnotationType(annot_type_id,description,CODE) VALUES (?1,?2,?3)");
			insertStmt->bind(1,annotTypeID_PUT);
			insertStmt->bind(2,"Imported from PUT Database");
			insertStmt->bind(3,"PUT");
			if (!sqlConn->step(insertStmt))
				throw runtime_error("error: couldn't insert annotation type id for PUT into database");
			delete insertStmt;
		}
		else
			throw runtime_error("error: couldn't read max annotation type id from database");
		//sqlConn->finalize(findMaxStmt);
		delete findMaxStmt;
	}
	//sqlConn->finalize(findStmt);
	delete findStmt;

	return annotTypeID_PUT;
}

int main( int argc, char* argv[] )
{
	std::cout << "\nImporting PUT Data from Annotation Tool into SQLite3 DB " << std::endl;

	//----------------- program options

	po::positional_options_description pod;
	pod.add("sqldb-file",1);
	pod.add("imgdbname",1);
    pod.add("image-dir",1);
	pod.add("landmarks-dir",1);
	pod.add("regions-dir",1);
	pod.add("clear-sqldb",1);

	po::options_description description("Options");
	description.add_options()
		("help", "produce help message")
		("sqldb-file", po::value< string >(), "sql database file")
		("imgdbname", po::value< string >(), "image database name")
		("image-dir", po::value< string >(), "image file path (../Images/XXXX/xxxxxxxx.jpg)")
        ("landmarks-dir", po::value< string >(), "landmarks file path (../LXXX/xxxxxxxx.yml)")
		("regions-dir", po::value< string >(), "regions file path (../RXXX/xxxxxxxx.yml)")
		("clear-sqldb", po::value< string >(), "optinal: clear sql database (default: false)")
	;

	po::variables_map variablesMap;

	try 
	{
		po::store(po::command_line_parser(argc, argv).options(description).positional(pod).run(), variablesMap);
		po::notify(variablesMap);
    } catch ( const boost::program_options::error& e ) 
	{
        std::cerr << e.what() << std::endl;
    }

	bool showHelp = false;

	string sqlDBFile;
	if (variablesMap.count("sqldb-file"))
		sqlDBFile = variablesMap["sqldb-file"].as<string>();
	else
		showHelp = true;

	string imgDBName;
	if (variablesMap.count("imgdbname"))
		imgDBName = variablesMap["imgdbname"].as<string>();
	else
		showHelp = true;

    string imgDir;
	if (variablesMap.count("image-dir"))
		imgDir = variablesMap["image-dir"].as<string>();
	else
		showHelp = true;


	string landmarksDir;
	if (variablesMap.count("landmarks-dir"))
		landmarksDir = variablesMap["landmarks-dir"].as<string>();
	else
		showHelp = true;

	string regionsDir;
	if (variablesMap.count("regions-dir"))
		regionsDir = variablesMap["regions-dir"].as<string>();
	else
		showHelp = true;

	string clearSqlDB;
	if (variablesMap.count("clear-sqldb"))
		clearSqlDB = variablesMap["clear-sqldb"].as<string>();
	else
		clearSqlDB = "false";

	if (variablesMap.count("help") || showHelp)
	{
	    cout << description << "\n";
	    return 1;
	}
    
    std::string basePath = "";

	std::cout << "-------------------------------------------------------" << std::endl;
	std::cout << "  Config:" << std::endl;
	std::cout << "    SQL DB Name:     " << sqlDBFile << std::endl;
	std::cout << "    Image DB Name:   " << imgDBName << std::endl;
    std::cout << "    Image Folder:    " << imgDir << std::endl;
	std::cout << "    Landmarks Folder:" << landmarksDir << std::endl;
	std::cout << "    Regions Folder:  " << regionsDir << std::endl;
	std::cout << "    Optional:        clear SqlDB -> " << clearSqlDB << std::endl;
	std::cout << "    Base Path:       " << basePath << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;


    //------------------ DELETE db entries from sql database
    if (clearSqlDB.compare("true") == 0) {
		cout << " \nCLEAR Sql Database " << endl;
        DeleteSqlEntries dsde;
        dsde.deleteAllSqlEntries(sqlDBFile);  
        return 0;
	} else {
		std::cout << " \nADD data to Sql Database" << std::endl;
		std::string::size_type pos = sqlDBFile.find_last_of("\\/");
		if (pos != std::string::npos) {
			basePath = sqlDBFile.substr(0,pos+1);
		}
	}


    //------------------ INSERT dbname INTO sql database
    DeleteSqlEntries dnsf;

    bool deleted = false;
    deleted = dnsf.deleteDBName(sqlDBFile, imgDBName);
    if (deleted == true) {
        dnsf.insertDBName(sqlDBFile, imgDBName);
    }

    // get all *jpg files from img file folder
    ImagesFromFolder iff;

    // PathVector paths = iff.getImagesFromFolder(imgDir + "\\0" + folderName, ".JPG");
	// paths.clear();

	PathVector pathImages = iff.getFilesFromFolderStruct(imgDir, ".JPG");
	iff.clearPathVector();
	PathVector pathLandmarks = iff.getFilesFromFolderStruct(landmarksDir, ".YML");
	iff.clearPathVector();

    if (pathImages.empty()) {
        std::cout << " PUTIMPORTER -> Warning: NO Files in Folderpath '" << imgDir << "' found! " << std::endl;
	} else if (pathLandmarks.empty()) {
		std::cout << " PUTIMPORTER -> Warning: NO Files in Folderpath '" << landmarksDir << "' found! " << std::endl;
    } else {
        // iff.printFoundFiles(paths);
		// iff.printFoundFiles(pathLandmarks);

		PutFileReader putFileReader;

		// first read all images 
        for (unsigned int n = 0; n < pathImages.size(); ++n)
        {    
			std::string imgFileName = pathImages.at(n).string();
			putFileReader.loadImage(imgFileName);    
            std::cout << " Insert '" << imgFileName << "' into putFileReader structure. " << std::endl;
		}

		// second add landmarks to images (if no images available ignore landmark)
		for (unsigned int n = 0; n < pathLandmarks.size(); ++n) 
		{
            std:string landmarksFileName = pathLandmarks.at(n).string();
			putFileReader.loadLandmarks(landmarksFileName);
			std::cout << " Insert '" << landmarksFileName << "' into putFileReader structure. " << std::endl;
		}
              
		// insert all images into sql database
		std::map<int, PutFileData>::iterator putIt = putFileReader.m_putDataMap.begin();  
		for (; putIt != putFileReader.m_putDataMap.end(); ++putIt)
        {  
			//putFileReader.printPutData();
                  
			// Draw landmarks to image.   
			//putFileReader.drawLandmarks();

	        //------------------ INSERT INTO sql db
	        SQLiteDBConnection sqlConn;

	        if (sqlConn.open(sqlDBFile))
	        {
				//int annotTypeID_PUT= getAnnotTypeID(&sqlConn);

		        // get the feature IDs from the database - matching the feature codes from the input file
		        FeatureCoordTypes fct;
		        fct.load(&sqlConn);
		        //fct.debugPrint();

		        vector<int> featIDs;
		        for (int i = 0; i < PutFileData::numFeaturesFileFeatureCodes; ++i)
		        {
			        string featCode = PutFileData::featuresFileFeatureCodes[i];
			        int featID = fct.getIDByCode(featCode);
			        if (featID < 0)
				        std::cout << "Error: No ID for feature '" << featCode << "'" << std::endl;
			        else {
				        featIDs.push_back(featID);
                    }
		        }

                string insertFaceSqlStmtStr = "INSERT INTO Faces(file_id,db_id) VALUES (?1,?2)";
		        SQLiteStmt *insertFaceSqlStmt = sqlConn.prepare(insertFaceSqlStmtStr);

		        string insertFeatureCoordsSqlStmtStr = "INSERT INTO FeatureCoords(face_id,feature_id,x,y) VALUES (?1, ?2, ?3, ?4)";
		        SQLiteStmt *insertFeatureCoordsSqlStmt = sqlConn.prepare(insertFeatureCoordsSqlStmtStr);

		        string insertImageSqlStmtStr = "INSERT INTO FaceImages(db_id,file_id,filepath,bw,width,height) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		        SQLiteStmt *insertImageSqlStmt = sqlConn.prepare(insertImageSqlStmtStr);

                bool allOK = true;

		        if (insertFaceSqlStmt && insertFeatureCoordsSqlStmt)
                {   
			        sqlConn.exec("PRAGMA synchronous = OFF;");
			        sqlConn.exec("BEGIN TRANSACTION;");

                        
                    vector<std::pair<double, double> >::iterator landmarksIt = putIt->second.landmarks.begin();
					if (landmarksIt == putIt->second.landmarks.end()) {
						std::cout << " NO Landmarks for Image:  " << putIt->second.imgFilename << " found." << std::endl;
						allOK = false;
					} 

                    // extract fileName and filePath
                    std::string fileName = "";
                    std::string filePath = putIt->second.imgFilename;
                    std::string::size_type pos = filePath.find_last_of("\\");
			        if (pos != std::string::npos)
			        {
				        fileName = filePath.substr(pos+1);  
				        pos = filePath.find("\\"); // find first space
				        while ( pos != string::npos )
				        {
					        filePath.replace( pos, 1, "/" );
					        pos = filePath.find( "\\", pos + 1 );
				        }
			        }
              
			        insertFaceSqlStmt->reset();
                    insertFaceSqlStmt->bind(1,fileName);
			        insertFaceSqlStmt->bind(2,imgDBName);
			        allOK = allOK && (sqlConn.step(insertFaceSqlStmt) == SQLITE_DONE);

			        // get database id of inserted face			
			        int face_id = sqlConn.getLastInsertRowid();

                    // insert features
			        insertFeatureCoordsSqlStmt->bind(1,face_id);
                    for (unsigned int i = 0 ; landmarksIt != putIt->second.landmarks.end(); ++landmarksIt, ++i)
			        {
                        if ( (landmarksIt->first > 0) && (landmarksIt->second > 0) ) {
                            if (i >= featIDs.size()) {
                                break;
                            }
                            insertFeatureCoordsSqlStmt->bind(2,featIDs[i]);
                            insertFeatureCoordsSqlStmt->bind(3,static_cast<float>(landmarksIt->first));
					        insertFeatureCoordsSqlStmt->bind(4,static_cast<float>(landmarksIt->second));
					        allOK = allOK && (sqlConn.step(insertFeatureCoordsSqlStmt) == SQLITE_DONE);
					        if (!allOK) {
						        break;
                            }
					        insertFeatureCoordsSqlStmt->reset();
				        }
			        }

		            // insert image                
                    std::string totalFileName = filePath;
                        
			        fs::path tmppath(totalFileName);
                    totalFileName = tmppath.relative_path().string();
                        
			        cv::Mat img = cv::imread(tmppath.string(),-1);
					if (img.empty()) {
						std::cout << "Error: Could not load Image: '" << totalFileName << "'" << std::endl;
						allOK = false;
					} else {

						std::string relativeFilePath = "";
						if (filePath.size() > imgDir.size()) {
							relativeFilePath = filePath.substr(imgDir.size()+1);
						}

						//db_id,file_id,filepath,bw,width,height
						insertImageSqlStmt->bind(1,imgDBName);
						insertImageSqlStmt->bind(2,fileName);
						insertImageSqlStmt->bind(3,relativeFilePath);
						insertImageSqlStmt->bind(4,img.channels() == 1);
						insertImageSqlStmt->bind(5,img.cols);
						insertImageSqlStmt->bind(6,img.rows);

						allOK = allOK && (sqlConn.step(insertImageSqlStmt) == SQLITE_DONE);
						insertImageSqlStmt->reset();
					}

			        if (allOK) {
				        sqlConn.exec("COMMIT;");
                        std::cout << "Reading ...   Inserting...    File: '" << putIt->second.filenumber <<
                            "' into Database" << std::endl;  
                    }
			        else {
				        sqlConn.exec("ROLLBACK TRANSACTION;");
                    }

			        sqlConn.exec("PRAGMA synchronous = NORMAL;");

			        sqlConn.finalize(insertFaceSqlStmt);
			        delete insertFaceSqlStmt;
			        sqlConn.finalize(insertFeatureCoordsSqlStmt);
			        delete insertFeatureCoordsSqlStmt;		
                }
	            sqlConn.close();

	        } else {
		        std::cout << "failed to open sql db file '" << sqlDBFile << "'" << std::endl;
            }
        }
    }
    std::cout << "done." << std::endl;
}
