/*
 * FaceRectViolaJonesCalculator.cpp
 *
 *  Created on: 18.08.2011
 *      Author: pwohlhart
 */

#include "FaceRectViolaJonesCalculator.h"

#include <highgui.h>

FaceRectViolaJonesCalculator::FaceRectViolaJonesCalculator()
{
	string faceCascadeFile = "haarcascade_frontalface_alt2.xml";
	//string faceCascadeFile = "haarcascade_frontalface_default.xml";
	_detector.load(faceCascadeFile);
	if (_detector.empty())
		throw runtime_error("couldn't load detector model");

	_faceRectTypeVJ = getAnnotationTypeIDByCode("VIOLAJONES");

	_useTransaction = false;
}

FaceRectViolaJonesCalculator::~FaceRectViolaJonesCalculator() {
}

void FaceRectViolaJonesCalculator::prepareQueryString()
{
	_mcID = _fcTypes.getIDByCode("MC"); // mouth center

	stringstream feature_idsSStr;
	feature_idsSStr << _lelcID << ",";
	feature_idsSStr << _lercID << ",";
	feature_idsSStr << _relcID << ",";
	feature_idsSStr << _rercID << ",";
	feature_idsSStr << _mcID;

	// all face_ids that do not have a type=2 facerect, and some feature-points annotated
	stringstream querySStr;
	querySStr << "SELECT Faces.face_id FROM Faces,FeatureCoords ";
	querySStr << "WHERE faces.face_id = featurecoords.face_id AND ";
	querySStr << "feature_id IN (" << feature_idsSStr.str() << ") ";
	querySStr << "GROUP BY faces.face_id HAVING count(feature_id) = 5 ";
	querySStr << "EXCEPT SELECT FaceRect.face_id FROM FaceRect WHERE FaceRect.annot_type_id = " << _faceRectTypeVJ;



	_faceQueryString = querySStr.str();
}

void FaceRectViolaJonesCalculator::prepareSQLStuff()
{
	string insertRectSqlStmtStr = "INSERT INTO FaceRect(face_id,x,y,w,h,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
	_insertRectSqlStmt = _sqlConn->prepare(insertRectSqlStmtStr);
}

bool FaceRectViolaJonesCalculator::calcStuff(FaceData *faceData)
{
	FaceDbImage *dbimg = faceData->getDbImg();

	std::string fileName = _basePath + dbimg->dbpath + dbimg->filepath;

	cout << "face id: " << faceData->ID << endl;

	vector<cv::Rect> rects = getDetections(fileName);


	// find correct Rect for this face
	cv::Rect corRect;
	if (getCorrectRect(faceData,rects,corRect))
	{
		cout << "inserting " << corRect.x << "," << corRect.y << "," << corRect.width << "," << corRect.height << endl;
		// FaceRectType ... 2 -> Viola Jones Detections
		//                  1 -> Calculated from feature coordinates with center_between_eyes as reference point
		//                 (0 would be manually annotated)
		//int faceRectType = getAnnotationTypeIDByCode("VIOLAJONES");

		// store rect
		_insertRectSqlStmt->bind(1,faceData->ID);
		_insertRectSqlStmt->bind(2,corRect.x);
		_insertRectSqlStmt->bind(3,corRect.y);
		_insertRectSqlStmt->bind(4,corRect.width);
		_insertRectSqlStmt->bind(5,corRect.height);
		_insertRectSqlStmt->bind(6,_faceRectTypeVJ);

		bool allOK = (_sqlConn->step(_insertRectSqlStmt) == SQLITE_DONE);
		if (allOK)
			_insertRectSqlStmt->reset();
		return allOK;
	}
	else
		return true;
}

vector<cv::Rect> FaceRectViolaJonesCalculator::getDetections(string fileName)
{
	vector<cv::Rect> rects;
	map<string,vector<cv::Rect> >::iterator it = _processedFiles.find(fileName);
	if (it != _processedFiles.end())
		rects = it->second;
	else
	{
		std::cout << "working on '" << fileName << "'" << std::endl;

		cv::Mat img = cv::imread(fileName); // forced to 3 channel color image by default

		int flags = CV_HAAR_SCALE_IMAGE; //CV_HAAR_DO_CANNY_PRUNING;
		double scaleFactor = 1.01;
		int minNeighbors = 5;
		_detector.detectMultiScale(img,rects,scaleFactor,minNeighbors,flags,_detector.getOriginalWindowSize());

		cout << "found " << rects.size() << " faces" << endl;
		_processedFiles.insert(make_pair(fileName,rects));

		for (int i = 0; i < rects.size(); ++i)
			cv::rectangle(img,rects[i],cv::Scalar(255,0,0),2);
		cv::imshow("dbg",img);
		cv::waitKey(100);
	}
	return rects;
}

bool FaceRectViolaJonesCalculator::getCorrectRect(FaceData *faceData, vector<cv::Rect> rects, cv::Rect &correctRect)
{
	faceData->loadFeatureCoords(_sqlConn);
	FeaturesCoords *fc = faceData->getFeaturesCoords();

	cv::Point2f lelc = fc->getCoords(_lelcID);
	cv::Point2f lerc = fc->getCoords(_lercID);
	cv::Point2f rerc = fc->getCoords(_rercID);
	cv::Point2f relc = fc->getCoords(_relcID);
	cv::Point2f mc = fc->getCoords(_mcID);

	cv::Point2f lec;
	cv::Point2f rec;
	lec.x = (lelc.x + lerc.x) / 2.0;
	lec.y = (lelc.y + lerc.y) / 2.0;
	rec.x = (relc.x + rerc.x) / 2.0;
	rec.y = (relc.y + rerc.y) / 2.0;

	bool found = false;
	for (int i = 0; i < rects.size(); ++i)
	{
		cv::Rect r = rects[i];
		if (r.contains(lec) && r.contains(rec) && r.contains(mc))
		{
			correctRect = r;
			found = true;
			break; // what if more than one rect contains the three points?
		}
	}

	return found;
}
