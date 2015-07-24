/*
 * FaceRectCalculator.cpp
 *
 *  Created on: 17.11.2010
 *      Author: pwohlhart
 */

#include "FaceRectCalculator.h"

#include "../querys/FaceIDQuery.h"
#include "../querys/FaceDataByIDsQuery.h"

FaceRectCalculator::FaceRectCalculator() : FaceStuffCalculator(), _insertRectSqlStmt(0)
{
}

FaceRectCalculator::~FaceRectCalculator() {
	delete _insertRectSqlStmt;
}

void FaceRectCalculator::prepareQueryString()
{
	_faceQueryString = "SELECT Faces.face_id FROM Faces LEFT OUTER JOIN FaceRect " \
									"ON Faces.face_id = FaceRect.face_id " \
									"WHERE FaceRect.face_id IS NULL";
}

void FaceRectCalculator::prepareSQLStuff()
{
	string insertRectSqlStmtStr = "INSERT INTO FaceRect(face_id,x,y,w,h,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
	_insertRectSqlStmt = _sqlConn->prepare(insertRectSqlStmtStr);
}

bool FaceRectCalculator::calcStuff(FaceData *faceData)
{
	cv::Rect rect = calcRect(faceData);
	std::cout << "  rect: " << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << std::endl;

	// FaceRectType ... 1 -> Calculated from feature coordinates with center_between_eyes as reference point
	//                 (0 would be manually annotated)
	int faceRectType = getCalcFromManuallyLabeledFeaturePointsAnnotationTypeID();

	// store rect
	_insertRectSqlStmt->bind(1,faceData->ID);
	_insertRectSqlStmt->bind(2,rect.x);
	_insertRectSqlStmt->bind(3,rect.y);
	_insertRectSqlStmt->bind(4,rect.width);
	_insertRectSqlStmt->bind(5,rect.height);
	_insertRectSqlStmt->bind(6,faceRectType);
	bool allOK = (_sqlConn->step(_insertRectSqlStmt) == SQLITE_DONE);
	if (allOK)
		_insertRectSqlStmt->reset();
	return allOK;
}

cv::Rect FaceRectCalculator::calcSingleRect(FaceData *faceData,SQLiteDBConnection *sqlConn)
{
	// init
	_sqlConn = sqlConn;
	_fcTypes.load(sqlConn);

	_meanFace3DModel.load(_sqlConn);

	//_lelcID = _fcTypes.getIDByCode("LELC"); // left eye left corner
	//_lercID = _fcTypes.getIDByCode("LERC"); // left eye right corner
	//_rercID = _fcTypes.getIDByCode("RERC"); // right eye left corner
	//_relcID = _fcTypes.getIDByCode("RELC"); // right eye right corner

	return calcRect(faceData);
}

cv::Rect FaceRectCalculator::calcRect(FaceData *faceData)
{
	//cout << "process face " << faceData->ID << endl;

	// normalized rect
	double faceRectWidth = 128;
	double faceRectHeight = 128;
	double normEyeDist = 50.0;              // distance between eyes in normalized rectangle
//	cv::Point2f targetCenter(64.0f, 37.0f); // center within normalized rectangle
	cv::Point2f centerOffset(0.0f, 25.0f); // shift from CENTER_BETWEEN_EYES to rectangle center

	cv::Point2f center;
	cv::Point2f upv;
	double scale = 0.0;
	calcCenterScaleAndUp(faceData,normEyeDist,center,scale,upv,CENTER_BETWEEN_EYES);

	/*
	int x = floor((center.x - targetCenter.x*scale) + 0.5);
	int y = floor((center.y - targetCenter.y*scale) + 0.5);
	*/
    int w = floor(faceRectWidth*scale + 0.5);
    int h = floor(faceRectHeight*scale + 0.5);

    cv::Point2f rectCenter = center;
    rectCenter -= cv::Point2f(upv.x*centerOffset.y*scale, upv.y*centerOffset.y*scale);
    rectCenter -= cv::Point2f(upv.y*centerOffset.x*scale, -upv.x*centerOffset.x*scale);

	int x = floor((rectCenter.x - faceRectWidth*scale/2) + 0.5);
	int y = floor((rectCenter.y - faceRectHeight*scale/2) + 0.5);

	return cv::Rect(x,y,w,h);
}

