/*
 * AnnotatedFeaturePatches.cpp
 *
 *  Created on: 29.07.2010
 *      Author: pwohlhart
 */

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "../dbconn/SQLiteDBConnection.h"
#include "../querys/FaceIDByAnnotatedFeatureQuery.h"
#include "../querys/FaceDataByIDsQuery.h"
#include "../querys/FaceIDByPoseQuery.h"

#include "cv.h"
#include "highgui.h"

void extractPatches(int featId, std::string featCode, std::map<int,FaceData*> faceDataMap, SQLiteDBConnection *sqlConn, std::string basePath, std::string outputPath)
{
	std::map<int,FaceData*>::iterator it;
	for (it = faceDataMap.begin(); it != faceDataMap.end(); ++it)
	{
		FaceData *faceData = it->second;
		if (!faceData->getFeaturesCoords())
			faceData->loadFeatureCoords(sqlConn);

		cv::Point2f pt = faceData->getFeaturesCoords()->getCoords(featId);

		std::string totalFileName = basePath + faceData->getDbImg()->dbpath + faceData->getDbImg()->filepath;

		cout << faceData->ID << " (" << totalFileName << "): " << pt.x << ", " << pt.y << endl;

		std::stringstream faceIdStr;
		faceIdStr << faceData->ID;
		std::string outFileName = faceData->fileID;
		std::string::size_type pos = outFileName.find_last_of(".");
		if (pos != std::string::npos)
			outFileName = outFileName.substr(0,pos);
		outFileName = outFileName + "_" + faceIdStr.str() + "_" + featCode + ".png";
		std::string totalOutFileName = outputPath + outFileName;

		cout << " -> " << totalOutFileName  << endl;

		cv::Mat img = cv::imread(totalFileName);
		//cv::Mat croppedImg = cv::Mat(img,cv::Range(pt.y-5,pt.y+5),cv::Range(pt.x-5,pt.x+5));
		cv::Mat croppedImg = cv::Mat(img,cv::Rect(pt.x-10,pt.y-10,21,21));

		//std::vector<int> params;
		cv::imwrite(totalOutFileName,croppedImg);
	}
}


int
main (int argc, char **argv)
{
	//string dbFile = "../db/facedb-data.db";
	std::string dbFile = "/home/pwohlhart/work/data/facedb/facedb-data.db";
	std::string basePath = "/home/pwohlhart/work/data/facedb/";
	std::string outputPath = "/home/pwohlhart/work/data/temp/";
	SQLiteDBConnection sqlConn;

	vector<string> featureNames;
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
			featureNames.push_back(argv[i]);
	}
	else
		featureNames.push_back("LeftEyeCenter");

	if (sqlConn.open(dbFile))
	{
		// Load Feature Coordinate Types from DB
		FeatureCoordTypes fcTypes;
		fcTypes.load(&sqlConn);

		// check if the ones we request are in the DB (at all)
		vector<int> featureIDs;
		for (unsigned int i = 0; i < featureNames.size(); ++i)
		{
			int featureID = 0;
			featureID = fcTypes.getIDByDescr(featureNames[i]);
			if (featureID < 0)
			{
				// not in db, maybe it's the code (ie., LEC instead of LeftEyeCenter)?
				featureID = fcTypes.getIDByCode(featureNames[i]);

				if (featureID < 0)
					cout << "Error: Requested feature '" << featureNames[i] << "' not in the database!" << endl;
			}

			if (featureID > 0)
				featureIDs.push_back(featureID);
		}

		// OK, we have the featureIDs now query which faces have it set
		FaceIDByAnnotatedFeatureQuery fidftq;
		fidftq.queryFeatureIDs = featureIDs;
		if (fidftq.exec(&sqlConn))
		{
			cout << "result count: " << fidftq.resultFaceIds.size() << endl;
			//for (unsigned int i = 0; i < fidftq.resultFaceIds.size(); ++i)
			//	cout << fidftq.resultFaceIds[i] << ", ";
			//cout << endl;

			// Get Face Data for returned face_ids
			FaceDataByIDsQuery fdq;
			fdq.queryIds = fidftq.resultFaceIds;
			if (fdq.exec(&sqlConn))
			{
				// report min and max yaw angles for resulting faces
				vector<float> yaw_angles;

				map<int,FaceData*>::iterator it;
				for (it = fdq.data.begin(); it != fdq.data.end(); ++it)
				{
					FacePose *pose = it->second->getPose();
					yaw_angles.push_back(pose->yaw);
				}

				vector<float>::iterator min_it = std::min_element(yaw_angles.begin(),yaw_angles.end());
				vector<float>::iterator max_it = std::max_element(yaw_angles.begin(),yaw_angles.end());
				cout << "yaw angles vary between " << *min_it << " and " << *max_it << endl;

				float max_yaw = *max_it;

				FaceIDByPoseQuery fidpq;
				fidpq.setAngles(max_yaw-0.0001,max_yaw,-10,10,-10,10);
				fidpq.exec(&sqlConn);
				cout << fidpq.face_ids[0] << endl;

				if (featureIDs.size() == 1)
				{
					// exactly one feature was queried -> we can extract patches from the images
					extractPatches(featureIDs[0], fcTypes.getCode(featureIDs[0]),fdq.data, &sqlConn, basePath, outputPath);
				}
			}
			else
				cout << "ERROR: Getting Face Data failed" << endl;
		}
		else
			cout << "ERROR: Query failed" << endl;

		sqlConn.close();

		cout << endl << "done." << endl;
		return 0;
	}
	else
		return 1;
}
