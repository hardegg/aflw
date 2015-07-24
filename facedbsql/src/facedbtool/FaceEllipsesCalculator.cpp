/*
 * FaceEllipsesCalculator.cpp
 *
 *  Created on: 07.12.2010
 *      Author: pwohlhart
 */

#include "FaceEllipsesCalculator.h"

//#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

#ifndef M_PI
  #define M_PI           3.14159265358979323846
#endif

FaceEllipsesCalculator::FaceEllipsesCalculator() : FaceStuffCalculator()
{
}

FaceEllipsesCalculator::~FaceEllipsesCalculator()
{
}

void FaceEllipsesCalculator::prepareQueryString()
{
	_faceQueryString = "SELECT Faces.face_id FROM Faces LEFT OUTER JOIN FaceEllipse " \
									"ON Faces.face_id = FaceEllipse.face_id " \
									"WHERE FaceEllipse.face_id IS NULL";
}

void FaceEllipsesCalculator::prepareSQLStuff()
{
	string insertRectSqlStmtStr = "INSERT INTO FaceEllipse(face_id,x,y,ra,rb,theta,upsidedown,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8)";
	_insertEllipseSqlStmt = _sqlConn->prepare(insertRectSqlStmtStr);
}

bool FaceEllipsesCalculator::calcStuff(FaceData *faceData)
{
	FaceEllipse ellipse;
	if (!calcEllipse(faceData,ellipse))
	{
		std::cout << "  ellipse calculation failed!" << std::endl;
		return true;
	}

	std::cout << "  ellipse: " << ellipse.x << " " << ellipse.y << " " << ellipse.ra << " " << ellipse.rb << " " << ellipse.theta << std::endl;
	std::cout << "           " << ellipse.theta*180.0/M_PI << std::endl;
	std::cout << "           " << (ellipse.upsideDown ? "upside down" : "normal" ) << std::endl;

	// FaceRectType ... 1 -> Calculated from feature coordinates with center_between_eyes as reference point
	//                 (0 would be manually annotated)
	int faceRectType = getCalcFromManuallyLabeledFeaturePointsAnnotationTypeID();

//	bool allOK = true;
//	showEllipse(faceData,ellipse);

 	// store rect
	_insertEllipseSqlStmt->bind(1,faceData->ID);
	_insertEllipseSqlStmt->bind(2,ellipse.x);
	_insertEllipseSqlStmt->bind(3,ellipse.y);
	_insertEllipseSqlStmt->bind(4,ellipse.ra);
	_insertEllipseSqlStmt->bind(5,ellipse.rb);
	_insertEllipseSqlStmt->bind(6,ellipse.theta);
	_insertEllipseSqlStmt->bind(7,ellipse.upsideDown);
	_insertEllipseSqlStmt->bind(8,faceRectType);
	bool allOK = (_sqlConn->step(_insertEllipseSqlStmt) == SQLITE_DONE);
	if (allOK)
		_insertEllipseSqlStmt->reset();

	return allOK;
}

bool FaceEllipsesCalculator::calcEllipse(FaceData *faceData, FaceEllipse &ellipse)
{
	//cout << "process face " << faceData->ID << endl;

	//cv::Point2f targetCenter(64.0f, 37.0f); // all faces are centered on this point
	double w_out = 110;
	double h_out = 155;
	double normEyeDist = 50.0;

	cv::Point2f center;
	cv::Point2f upv;
	double scale = 0.0;
	if (!calcCenterScaleAndUp(faceData,normEyeDist,center,scale,upv,CENTER_FOR_ELLIPSE))
		return false;

	ellipse.x = static_cast<float>(center.x);
	ellipse.y = static_cast<float>(center.y);
	ellipse.ra = static_cast<float>(h_out*scale/2.0);
	ellipse.rb = static_cast<float>(w_out*scale/2.0);
	//std::cout << "upv " << upv.x << " " << upv.y << std::endl;
	if (abs(upv.x) > 0.00001)
		ellipse.theta = atan2(upv.y,upv.x);
	else
		ellipse.theta = M_PI/2.0;

	if (ellipse.theta > M_PI/2.0)
		ellipse.theta -= M_PI;
	if (ellipse.theta < -M_PI/2.0)
		ellipse.theta += M_PI;

	ellipse.upsideDown = upv.y > 0;

	return true;
}

void FaceEllipsesCalculator::showEllipse(FaceData *faceData, FaceEllipse ellipse)
{
	// load image
	FaceDbImage *dbImg = faceData->getDbImg();
	string fullFileName = _basePath + dbImg->dbpath + dbImg->filepath;
	fs::path tmppath(fullFileName);

	fullFileName = tmppath.string();
	cout << "showing: " << fullFileName << endl;

	cv::Mat img = cv::imread(fullFileName);

	cv::Point2f cnt(ellipse.x,ellipse.y);
    cv::Size axes(ellipse.ra,ellipse.rb);
    double angle = -static_cast<double>(ellipse.theta)*180.0/M_PI;
	cv::ellipse(img,cnt,axes,angle,0.0,360.0,cv::Scalar(0.0,0.0,255.0));


	cv::imshow("ellipse",img);
	cv::waitKey();
}
