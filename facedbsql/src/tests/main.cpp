/*
 * main.cpp
 *
 *  Created on: 23.07.2010
 *      Author: pwohlhart
 */

#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <vector>

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FaceData.h"
#include "../querys/FaceDBQuery.h"
#include "../querys/FaceIDByPoseQuery.h"
#include "../querys/FaceDataByIDsQuery.h"
#include "../querys/FaceIDByAnnotatedFeatureQuery.h"


using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	cout << "----------------- begin record" << endl;
	for (i = 0; i < argc; i++)
	{
		cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << endl;
	}
	cout << "----------------- end record " << endl;
	return 0;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int
main (int argc, char **argv)
{
	cout << "testing sqlite connection" << endl;
	//string dbFilename = "~/.gvfs/public auf jupiter/projects/mdl/data/facedb-data";
	//string dbFilename = "../db/facedb-data.db";
	string dbFilename = "/media/ssd/data/aflw/data/aflw.sqlite";

	SQLiteDBConnection *sqlConn = new SQLiteDBConnection();

	double degree = M_PI / 180.0;

	double yawRange = 180*degree;
	double pitchRange = 180*degree;
	double rollRange = 180*degree;


    string rootFolder = "/media/ssd/data/aflw/data/flickr";
    string dstFolder = "/media/ssd/data/aflw/data/faces";
    
    string calibFolder = dstFolder + "/" + "calibration";
    string detecFolder = dstFolder + "/" + "detection";

    vector<ofstream*> ofDetects_train(3);
    ofDetects_train[0] = new ofstream(string(dstFolder + "/detection12_train.txt").c_str(), std::ofstream::out);
    ofDetects_train[1] = new ofstream(string(dstFolder + "/detection24_train.txt").c_str(), std::ofstream::out);
    ofDetects_train[2] = new ofstream(string(dstFolder + "/detection24_train.txt").c_str(), std::ofstream::out);
    
    vector<ofstream*> ofCalibs_train(3);
    ofCalibs_train[0] = new ofstream(string(dstFolder + "/calibration12_train.txt").c_str(), std::ofstream::out);
    ofCalibs_train[1] = new ofstream(string(dstFolder + "/calibration24_train.txt").c_str(), std::ofstream::out);
    ofCalibs_train[2] = new ofstream(string(dstFolder + "/calibration48_train.txt").c_str(), std::ofstream::out);
    
    vector<ofstream*> ofDetects_val(3);
    ofDetects_val[0] = new ofstream(string(dstFolder + "/detection12_val.txt").c_str(), std::ofstream::out);
    ofDetects_val[1] = new ofstream(string(dstFolder + "/detection24_val.txt").c_str(), std::ofstream::out);
    ofDetects_val[2] = new ofstream(string(dstFolder + "/detection24_val.txt").c_str(), std::ofstream::out);
    
    vector<ofstream*> ofCalibs_val(3);
    ofCalibs_val[0] = new ofstream(string(dstFolder + "/calibration12_val.txt").c_str(), std::ofstream::out);
    ofCalibs_val[1] = new ofstream(string(dstFolder + "/calibration24_val.txt").c_str(), std::ofstream::out);
    ofCalibs_val[2] = new ofstream(string(dstFolder + "/calibration48_val.txt").c_str(), std::ofstream::out);
    
    int nVals = 800;    // We leave 800 images for validation
        
    if (sqlConn->open(dbFilename))
    {
        FeatureCoordTypes fcTypes;
        fcTypes.load(sqlConn);
        fcTypes.debugPrint();

        cout << "-------------------------------" << endl;

        FaceIDByPoseQuery fidq;
        fidq.setAngles(-yawRange, yawRange, -pitchRange, pitchRange, -rollRange, rollRange);
        fidq.exec(sqlConn);
        fidq.debugPrint();

        cout << "-------------------------------" << endl;

        FaceDataByIDsQuery fdq;
        fdq.queryIds = fidq.face_ids;
        fdq.exec(sqlConn);

        FaceData *fd = fdq.data[fdq.queryIds[0]];

        //FeaturesCoords *fc = new FeaturesCoords(fd);
        //fc->load(sqlConn);
        fd->loadFeatureCoords(sqlConn);
        FeaturesCoords *fc = fd->getFeaturesCoords();
        //fc->debugPrint();

        fd->debugPrint();


        size_t i = 0;
        for (; i< fdq.queryIds.size() - nVals; i++) {
                FaceData *fd = fdq.data[fdq.queryIds[i]];
                fd->SaveFaceRegion_rect(rootFolder, dstFolder, ofDetects_train, ofCalibs_train);
        }
        for (; i< fdq.queryIds.size(); i++) {
                FaceData *fd = fdq.data[fdq.queryIds[i]];
                fd->SaveFaceRegion_rect(rootFolder, dstFolder, ofDetects_val, ofCalibs_val);
        }

        //FeaturesCoords *fc2 = new FeaturesCoords(*fc);
        //fc2->debugPrint();

        cout << "-------------------------------" << endl;
        cout << "Get all faces that have features 1 and 2 annotated: ";

        FaceIDByAnnotatedFeatureQuery fidByFeat;
        fidByFeat.queryFeatureIDs.push_back(1);
        fidByFeat.queryFeatureIDs.push_back(2);
        fidByFeat.exec(sqlConn);
        for (unsigned int i = 0; i < fidByFeat.resultFaceIds.size(); ++i)
                cout << fidByFeat.resultFaceIds[i] << ", ";
        cout << endl;

        cout << "Get all faces that have features 1,2 and 3 annotated: ";
        fidByFeat.queryFeatureIDs.push_back(3);
        fidByFeat.exec(sqlConn);
        for (unsigned int i = 0; i < fidByFeat.resultFaceIds.size(); ++i)
                cout << fidByFeat.resultFaceIds[i] << ", ";
        cout << endl;

        sqlConn->close();
    }

        
	cout << "done" << endl;

    for (int i = 0; i < 3; i++) {
        ofDetects_train[i]->close();
        delete ofDetects_train[i];
        ofDetects_val[i]->close();
        delete ofDetects_val[i];
        ofCalibs_train[i]->close();
        delete ofCalibs_train[i];
        ofCalibs_val[i]->close();
        delete ofCalibs_val[i];

    }
	return 0;
}
