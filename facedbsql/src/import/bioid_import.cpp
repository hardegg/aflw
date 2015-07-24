/*
 * bioid_import.cpp
 *
 *  Created on: 07.04.2012
 *      Author: szweimueller
 */

#define BOOST_FILESYSTEM_VERSION 3


#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"

#include <algorithm>
#include <map>
#include <vector>
#include <set>

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/format.hpp"

#include "cv.h"
#include "highgui.h"

#include "BioIdFileReader.h"

#include "DeleteSqlEntries.h"
#include "ImagesFromFolder.h"
#include "MetaDataFileReader.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


int main( int argc, char* argv[] )
{
	cout << "\nImporting BioId Data from Annotation Tool into SQLite3 DB " << endl;

	//----------------- program options

	po::positional_options_description pod;
	pod.add("sqldb-file",1);
	pod.add("imgdbname",1);
    pod.add("image-dir",1);
	pod.add("eyes-dir",1);
	pod.add("points-dir",1);
	pod.add("clear-sqldb",1);

	po::options_description description("Options");
	description.add_options()
		("help", "produce help message")
		("sqldb-file", po::value< string >(), "sql database file")
		("imgdbname", po::value< string >(), "image database name")
		("image-dir", po::value< string >(), "image file path (BioId_XXXX.pgm)")
        ("eyes-dir", po::value< string >(), "eyes file path (BioId_XXXX.eye)")
		("points-dir", po::value< string >(), "points file path (bioid_XXXX.pts)")
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

	string eyeDir;
	if (variablesMap.count("eyes-dir"))
		eyeDir = variablesMap["eyes-dir"].as<string>();
	else
		showHelp = true;

	string pointsDir;
	if (variablesMap.count("points-dir"))
		pointsDir = variablesMap["points-dir"].as<string>();
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
	std::cout << "    SQL DB Name:   " << sqlDBFile << std::endl;
	std::cout << "    Image DB File: " << imgDBName << std::endl;
    std::cout << "    Image Folder:  " << imgDir << std::endl;
	std::cout << "    Eye Folder:    " << eyeDir << std::endl;
	std::cout << "    Feature Folder:" << pointsDir << std::endl;
	std::cout << "    Optional:      clear SqlDB -> " << clearSqlDB << std::endl;
	std::cout << "    Base Path:     " << basePath << std::endl;
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
		if (pos != std::string::npos)
		{
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

    // get all *pgm files in img file folder
    ImagesFromFolder iff;
    PathVector paths = iff.getImagesFromFolder(imgDir, ".PGM");
    if (paths.empty()) {
            std::cout << " BIOIDIMPORTER -> Warning: Folderpath '" << imgDir << "' not available! " << std::endl;
    } else {
        iff.printFoundFiles(paths);
	
        bool available = true;
        for (unsigned int n = 0; n < paths.size(); ++n) 
        {
            // extract file number    
            std::string filenumber;
            string::size_type p = paths.at(n).string().find_last_of("_");
            if (p != string::npos) {
                filenumber = paths.at(n).string().substr(p+1,4);
            } else {
                break;
            }
            //boost::format fmt("%04d");
            //fmt % n;
    
            BioIdFileReader bioIdFileReader;

            std::string imgFileName = imgDir + "\\" + "BioID" + "_" + filenumber + ".pgm";
            available = bioIdFileReader.loadImage(imgFileName);

            std::string eyeFileName = eyeDir + "\\" + "BioID" + "_" + filenumber + ".eye";
            available = bioIdFileReader.loadEyes(eyeFileName);

            std::string pointsFileName = pointsDir + "\\" + "bioid" + "_" + filenumber + ".pts";
            available = bioIdFileReader.loadPoints(pointsFileName);

            // bioIdFileReader.printBioIdData();

            // bioIdFileReader.drawLandmarks();

            if (available == false) {
                std::cout << " data for '" << imgFileName << " missing." << std::endl;
                break;
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
		        for (int i = 0; i < BioIdFileData::numFeaturesFileFeatureCodes; ++i)
		        {
			        string featCode = BioIdFileData::featuresFileFeatureCodes[i];
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

		        string insertImageSqlStmtStr = "INSERT INTO FaceImages(db_id,file_id,filepath,bw,width,height) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
		        SQLiteStmt *insertImageSqlStmt = sqlConn.prepare(insertImageSqlStmtStr);

                bool allOK = true;

		        if (insertFaceSqlStmt && insertFeatureCoordsSqlStmt)
		        {
			        sqlConn.exec("PRAGMA synchronous = OFF;");
			        sqlConn.exec("BEGIN TRANSACTION;");

                    std::map<int, BioIdFileData>::iterator bioIdIt = bioIdFileReader.m_bioIdDataMap.begin();
                    vector<std::pair<double, double> >::iterator pointsIt = bioIdIt->second.points.begin();

                    // extract fileName from filePath
                    std::string fileName = "";
                    std::string filePath = bioIdIt->second.imgFilename;
			        std::string::size_type pos = filePath.find(imgDBName);
			        if (pos != std::string::npos)
			        {
				        fileName = filePath.substr(pos+imgDBName.length()+1);    
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
                    if (!allOK) {
				        break;
                    }

			        // get database id of inserted face	
                    int face_id = sqlConn.getLastInsertRowid();

                    // insert features
			        insertFeatureCoordsSqlStmt->bind(1,face_id);
                    for (int i = 0 ; pointsIt != bioIdIt->second.points.end(); ++pointsIt, ++i)
			        {
                        if ( (pointsIt->first > 0) && (pointsIt->second > 0) ) {
                            insertFeatureCoordsSqlStmt->bind(2,featIDs[i]);
                            insertFeatureCoordsSqlStmt->bind(3,static_cast<float>(pointsIt->first));
					        insertFeatureCoordsSqlStmt->bind(4,static_cast<float>(pointsIt->second));
					        allOK = allOK && (sqlConn.step(insertFeatureCoordsSqlStmt) == SQLITE_DONE);
					        if (!allOK) {
						        break;
                            }
					        insertFeatureCoordsSqlStmt->reset();
				        }
			        }
		
		            // insert image                
			        std::string totalFileName = filePath + "/" + fileName;
			        fs::path tmppath(totalFileName);
			        totalFileName = tmppath.parent_path().string();

			        cv::Mat img = cv::imread(totalFileName,-1);
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
						if (!allOK) {
							break;
						}
						insertImageSqlStmt->reset();
					}

			        if (allOK) {
				        sqlConn.exec("COMMIT;");
                        std::cout << "Reading ...   Inserting...    File: '" << paths.at(n).string() <<
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
		        std::cout << "Failed to open sql db file '" << sqlDBFile << "'" << std::endl;
            }
        }
     }
	 std::cout << "done." << std::endl;
}
