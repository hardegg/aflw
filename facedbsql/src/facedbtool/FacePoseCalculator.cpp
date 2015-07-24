/*
 * FacePoseCalculator.cpp
 *
 *  Created on: 15.12.2011
 *      Author: pwohlhart
 */

#include "FacePoseCalculator.h"

#include "../facedata/FacePose.h"

FacePoseCalculator::FacePoseCalculator() : FaceStuffCalculator(), _insertPoseSqlStmt(0) {

}

FacePoseCalculator::~FacePoseCalculator() {
	delete _insertPoseSqlStmt;
}

void FacePoseCalculator::prepareQueryString()
{
	_faceQueryString = "SELECT Faces.face_id FROM Faces LEFT OUTER JOIN FacePose " \
									"ON Faces.face_id = FacePose.face_id " \
									"WHERE FacePose.face_id IS NULL";
}

void FacePoseCalculator::prepareSQLStuff()
{
	string _insertPoseSqlStmtStr = "INSERT INTO FacePose(face_id,yaw,pitch,roll,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5)";
	_insertPoseSqlStmt = _sqlConn->prepare(_insertPoseSqlStmtStr);
}

bool FacePoseCalculator::calcStuff(FaceData *faceData)
{
	FacePose pose = calcPose(faceData);
	std::cout << "  face: " << faceData->ID << std::endl;
	std::cout << "    pose: " << pose.yaw << " " << pose.pitch << " " << pose.roll << " " << std::endl;

	// FaceRectType ... 1 -> Calculated from feature coordinates with center_between_eyes as reference point
	//                 (0 would be manually annotated)
	pose.annotTypeID = getCalcFromManuallyLabeledFeaturePointsAnnotationTypeID();

	// store rect
	_insertPoseSqlStmt->bind(1,faceData->ID);
	_insertPoseSqlStmt->bind(2,static_cast<float>(pose.yaw));
	_insertPoseSqlStmt->bind(3,static_cast<float>(pose.pitch));
	_insertPoseSqlStmt->bind(4,static_cast<float>(pose.roll));
	_insertPoseSqlStmt->bind(5,pose.annotTypeID);
	bool allOK = (_sqlConn->step(_insertPoseSqlStmt) == SQLITE_DONE);
	if (allOK)
		_insertPoseSqlStmt->reset();
	return allOK;
}


FacePose FacePoseCalculator::calcPose(FaceData *faceData)
{
	FacePose fp;

	FaceDbImage *dbimg = faceData->getDbImg();
	if (!dbimg)
		return fp;

	std::cout << "  image: " << dbimg->file_id << std::endl;

	double focalLength = static_cast<double>(dbimg->width) * 1.5;
	cv::Point2f imgCenter = cv::Point2f(static_cast<float>(dbimg->width) / 2.0f, static_cast<float>(dbimg->height) / 2.0f);

	cv::Mat rot;
	cv::Point3f trans;
	if (!getPose(faceData,focalLength,imgCenter,rot,trans,true))
		return fp;

	/*
	// DEBUG
	cout << "fl: " << focalLength << endl;
	cout << "center: " << imgCenter.x << ", " << imgCenter.y << endl;
	cout << "trans: " << trans.x << ", " << trans.y << ", " << trans.z << endl;
	cout << "rot:" << endl;
	for(int i=0;i < 3; ++i)
	{
		for(int j=0;j < 3; ++j)
			cout << rot.at<float>(i,j) << ", ";
		cout << endl;
	}
	*/

	cv::Mat tfm = (cv::Mat_<float>(4,4) << 0,0,0,trans.x, 0,0,0,trans.y, 0,0,0,trans.z, 0,0,0,1);
	for(int i=0;i < 3; ++i)
		for(int j=0;j < 3; ++j)
			tfm.at<float>(i,j) = rot.at<float>(i,j);

	// the transformation tfm is a combination of the rotation that links the model coordinate system (roll,pitch,yaw)
	// and the coordinate system that is used in the POSIT algorithm where the z axis is perpendicular to the image
	// plane pointing positively into the image -> we get tfm as the rotation transforming the model into this POSIT
	// coord system so we use rotateCoordSystem to split tfm! (rotation matrix -> inverse == transpose)
	cv::Mat rotateCoordSystemM = (cv::Mat_<float>(4, 4) << 0,0,-1,0, -1,0,0,0, 0,1,0,0, 0,0,0,1);
	cv::Mat tmp = rotateCoordSystemM * tfm;

	splitRotationMatrixToRollPitchYaw(tmp, fp.roll, fp.pitch, fp.yaw);

	return fp;
}
