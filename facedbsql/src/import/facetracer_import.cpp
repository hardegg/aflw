/*
 * facetrace_import.cpp
 *
 *  Created on: 01.07.2012
 *      Author: szweimueller
 */

#define BOOST_FILESYSTEM_VERSION 3


#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"

#include "../querys/DeleteFacesByIdsQuery.h"

#include "../util/utils.h"

#include <algorithm>
#include <map>
#include <vector>
#include <set>

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/format.hpp"

#include "cv.h"
#include "highgui.h"

#include "FaceTracerFileReader.h"

#include "DeleteSqlEntries.h"
#include "ImagesFromFolder.h"
#include "MetaDataFileReader.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


int getAnnotTypeID(SQLiteDBConnection* sqlConn)
{
	int annotTypeID_FACETRACER = 0;
	SQLiteStmt *findStmt = sqlConn->prepare("SELECT annot_type_id FROM AnnotationType WHERE CODE='FACETRACER'");
	int res = sqlConn->step(findStmt);
	if (res == SQLITE_ROW)
	{
		if (!findStmt->readIntColumn(0,annotTypeID_FACETRACER))
			throw runtime_error("error: couldn't read annotation type id for FaceTracer from database");
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
			annotTypeID_FACETRACER = maxID+1;
			SQLiteStmt *insertStmt = sqlConn->prepare("INSERT INTO AnnotationType(annot_type_id,description,CODE) VALUES (?1,?2,?3)");
			insertStmt->bind(1,annotTypeID_FACETRACER);
			insertStmt->bind(2,"Imported from FaceTracer Database");
			insertStmt->bind(3,"FACETRACER");
			if (!sqlConn->step(insertStmt))
				throw runtime_error("error: couldn't insert annotation type id for FaceTracer into database");
			delete insertStmt;
		}
		else
			throw runtime_error("error: couldn't read max annotation type id from database");
		//sqlConn->finalize(findMaxStmt);
		delete findMaxStmt;
	}
	//sqlConn->finalize(findStmt);
	delete findStmt;

	return annotTypeID_FACETRACER;
}

int main( int argc, char* argv[] )
{
	std::cout << "\nImporting FACETRACER Data from Annotation Tool into SQLite3 DB " << std::endl;

	//----------------- program options

	po::positional_options_description pod;
	pod.add("sqldb-file",1);
	pod.add("imgdbname",1);
    pod.add("imgfolder",1);
	pod.add("faceindex-file",1);
    pod.add("facestats-file",1);
	pod.add("facelabel-file",1);
	pod.add("attribute-file",1);
	pod.add("clear-sqldb",1);

	po::options_description description("Options");
	description.add_options()
		("help", "produce help message")
		("sqldb-file", po::value< string >(), "sql database file")
		("imgdbname", po::value< string >(), "image database name")
        ("image-folder", po::value< string >(), "image folder")
		("faceindex-file", po::value< string >(), "faceindex file (../faceindex.txt)")
        ("facestats-file", po::value< string >(), "facestats file (../facestats.txt)")
		("facelabel-file", po::value< string >(), "facelabel file (../facelabel.txt)")
		("attribute-file", po::value< string >(), "attribute file (../attribute.txt)")
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

    string imgFolder;
    if (variablesMap.count("image-folder"))
        imgFolder = variablesMap["image-folder"].as<string>();
    else
        showHelp = true;

    string faceindexFile;
	if (variablesMap.count("faceindex-file"))
		faceindexFile = variablesMap["faceindex-file"].as<string>();
	else
		showHelp = true;


	string facestatsFile;
	if (variablesMap.count("facestats-file"))
		facestatsFile = variablesMap["facestats-file"].as<string>();
	else
		showHelp = true;

	string facelabelFile;
	if (variablesMap.count("facelabel-file"))
		facelabelFile = variablesMap["facelabel-file"].as<string>();
	else
		showHelp = true;
		
	string attributeFile;
	if (variablesMap.count("attribute-file"))
		attributeFile = variablesMap["attribute-file"].as<string>();
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
	std::cout << "    SQL DB Name:    " << sqlDBFile << std::endl;
	std::cout << "    Image DB Name:  " << imgDBName << std::endl;
    std::cout << "    Image Folder:   " << imgFolder << std::endl;
    std::cout << "    FaceIndex File: " << faceindexFile << std::endl;
	std::cout << "    FaceStats File: " << facestatsFile << std::endl;
	std::cout << "    FaceLabel File: " << facelabelFile << std::endl;
	std::cout << "    Attribute File: " << attributeFile << std::endl;
	std::cout << "    Optional:      clear SqlDB -> " << clearSqlDB << std::endl;
	std::cout << "    Base Path:      " << basePath << std::endl;
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

    // get all *jpg files in img file folder
    ImagesFromFolder iff;
    PathVector paths = iff.getImagesFromFolder(imgFolder, ".JPG");
    if (paths.empty()) {
        std::cout << " FACETRACERIMPORTER -> Folderpath '" << imgFolder << "' is empty! " << std::endl;
    } else {
        iff.printFoundFiles(paths);

        bool available = true;
        for (unsigned int n = 0; n < paths.size(); ++n)
        {
            // extract faceId from path
            int faceId = utils::convertToInt(paths.at(n).stem().string());
        
            FaceTracerFileReader faceTracerFileReader;

            std::string faceindexFileName = faceindexFile;
            available = faceTracerFileReader.loadFaceindex(faceId, faceindexFileName, imgFolder);   
        
            std::string facestatsFileName = facestatsFile;
            available = faceTracerFileReader.loadFacestats(faceId, facestatsFileName);

            std::string facelabelFileName = facelabelFile;
            available = faceTracerFileReader.loadFacelabel(faceId, facelabelFileName);

            // faceTracerFileReader.printFaceTracerData();

            // Draw landmarks to image.
            //faceTracerFileReader.drawFacestats(faceId);

            if (available == false) {
                std::cout << "facetrace_import::main() data for faceId('" << faceId << "') missing." << std::endl;
                break;
            }

	        //------------------ INSERT INTO sql db
	        SQLiteDBConnection sqlConn;
	        if (sqlConn.open(sqlDBFile))
	        {
				int annotTypeID_FACETRACER = getAnnotTypeID(&sqlConn);

		        // get the feature IDs from the database - matching the feature codes from the input file
		        FeatureCoordTypes fct;
		        fct.load(&sqlConn);
		        //fct.debugPrint();

		        vector<int> featIDs;
		        for (int i = 0; i < FaceTracerFileData::numFeaturesFileFeatureCodes; ++i)
		        {
			        string featCode = FaceTracerFileData::featuresFileFeatureCodes[i];
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

				string insertPoseSqlStmtStr = "INSERT INTO FacePose(face_id,roll,pitch,yaw,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5)";
				SQLiteStmt *insertPoseSqlStmt = sqlConn.prepare(insertPoseSqlStmtStr);

				string insertMetaDataSqlStmtStr = "INSERT INTO FaceMetadata(face_id,sex,occluded,glasses,bw,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
				SQLiteStmt *insertMetaDataSqlStmt = sqlConn.prepare(insertMetaDataSqlStmtStr);

                bool allOK = true;

		        if (insertFaceSqlStmt && insertFeatureCoordsSqlStmt)
                {   
			        sqlConn.exec("PRAGMA synchronous = OFF;");
			        sqlConn.exec("BEGIN TRANSACTION;");

                    std::map<int, FaceTracerFileData>::iterator faceTracerIt = faceTracerFileReader.m_faceTracerDataMap.begin();  
                    vector<std::pair<double, double> >::iterator facestatsIt = faceTracerIt->second.facestats.begin();

                    std::string fileName = faceTracerIt->second.faceIdFilename;
			        insertFaceSqlStmt->reset();
                    insertFaceSqlStmt->bind(1,fileName);
			        insertFaceSqlStmt->bind(2,imgDBName);
			        allOK = allOK && (sqlConn.step(insertFaceSqlStmt) == SQLITE_DONE);
                    if (!allOK) {
				        break;
                    }

			        // get database id of inserted face			
			        int dbFaceId = sqlConn.getLastInsertRowid();

		            // insert image
					std::string filePath = (boost::filesystem::path(faceTracerIt->second.imageFoldername) / boost::filesystem::path(faceTracerIt->second.faceIdFilename)).string();
                    cv::Mat img = cv::imread(filePath, -1);

                    if (!img.data) {
                        std::cout << "Could not load image: " << filePath.c_str() << std::endl;
                        allOK = false;
                    } else {

						std::string relativeFilePath = "";
						if (filePath.size() > imgFolder.size()) {
							relativeFilePath = filePath.substr(imgFolder.size()+1);
						}

			            //db_id,file_id,filepath,bw,width,height
			            insertImageSqlStmt->bind(1,imgDBName);
                        insertImageSqlStmt->bind(2,fileName);
			            insertImageSqlStmt->bind(3,relativeFilePath);
			            insertImageSqlStmt->bind(4,img.channels() == 1);
			            insertImageSqlStmt->bind(5,img.cols);
			            insertImageSqlStmt->bind(6,img.rows);
                
                        allOK = allOK && (sqlConn.step(insertImageSqlStmt) == SQLITE_DONE);
			            if (!allOK) {
				            break;
                        }
			            insertImageSqlStmt->reset();
                    }

                    // insert features
			        insertFeatureCoordsSqlStmt->bind(1,dbFaceId);
                    for (unsigned int i = 0 ; facestatsIt != faceTracerIt->second.facestats.end(); ++facestatsIt, ++i)
			        {
                        if ( (facestatsIt->first > 0) && (facestatsIt->second > 0) ) {
                            if (i >= featIDs.size()) {
                                allOK = false;
                            }
                            if ((facestatsIt->first >= img.cols) || (facestatsIt->second >= img.rows)) {
                                allOK = false;
                            }
                            insertFeatureCoordsSqlStmt->bind(2,featIDs[i]);
                            insertFeatureCoordsSqlStmt->bind(3,static_cast<float>(facestatsIt->first));
					        insertFeatureCoordsSqlStmt->bind(4,static_cast<float>(facestatsIt->second));
					        allOK = allOK && (sqlConn.step(insertFeatureCoordsSqlStmt) == SQLITE_DONE);
					        if (!allOK) {
						        break;
                            }
					        insertFeatureCoordsSqlStmt->reset();
				        }
			        }
					
					// insert pose
					insertPoseSqlStmt->reset();
					insertPoseSqlStmt->bind(1,dbFaceId);
					insertPoseSqlStmt->bind(2,faceTracerIt->second.roll);
					insertPoseSqlStmt->bind(3,faceTracerIt->second.pitch);
					insertPoseSqlStmt->bind(4,faceTracerIt->second.yaw);
					insertPoseSqlStmt->bind(5,annotTypeID_FACETRACER);
					allOK = allOK && (sqlConn.step(insertPoseSqlStmt) == SQLITE_DONE);
			        if (!allOK) {
				        // break;
                    }
					
					//// insert meta data	
					//insertMetaDataSqlStmt->reset();
					//insertMetaDataSqlStmt->bind(1,dbFaceId);
					//insertMetaDataSqlStmt->bind(2,faceTracerIt->second.sex);
					//insertMetaDataSqlStmt->bind(3,faceTracerIt->second.occluded);
					//insertMetaDataSqlStmt->bind(4,faceTracerIt->second.glasses);
					//insertMetaDataSqlStmt->bind(5,faceTracerIt->second.bw);
					//insertMetaDataSqlStmt->bind(6,annotTypeID_FACETRACER);
					//allOK = allOK && (sqlConn.step(insertMetaDataSqlStmt) == SQLITE_DONE);
					//if (!allOK) {
				    //       // break;
                    //}

			        if (allOK) {
				        sqlConn.exec("COMMIT;");
                        std::cout << "Reading ...   Inserting...    File: '" << faceId <<
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
					sqlConn.finalize(insertPoseSqlStmt);
			        delete insertPoseSqlStmt;	
					sqlConn.finalize(insertMetaDataSqlStmt);
			        delete insertMetaDataSqlStmt;	
                }
	            sqlConn.close();
        
	        } else {
		        std::cout << "failed to open sql db file ('" << sqlDBFile << "')" << std::endl;
            }
        }
    }
    std::cout << "done." << std::endl;
}
