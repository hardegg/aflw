/*
 * CamPoseCalculator.cpp
 *
 *  Created on: 06.12.2012
 *      Author: pwohlhart
 */

#include "CamPoseCalculator.h"

#include "../facedata/FacePose.h"

CamPoseCalculator::CamPoseCalculator() :
	FaceStuffCalculator(),
	_insertPoseSqlStmt(0)
{
}

CamPoseCalculator::~CamPoseCalculator()
{
	delete _insertPoseSqlStmt;
}


void CamPoseCalculator::prepareQueryString()
{
	// faces with FacePose but without CamPose
 _faceQueryString = "SELECT Faces.face_id FROM Faces INNER JOIN FacePose ON Faces.face_id = FacePose.face_id " \
									"LEFT OUTER JOIN CamPose ON Faces.face_id = CamPose.face_id " \
									"WHERE CamPose.face_id IS NULL";
//	_faceQueryString = "SELECT Faces.face_id FROM Faces WHERE face_id = 49271"; //39346";
}

void CamPoseCalculator::prepareSQLStuff()
{
	string _insertPoseSqlStmtStr = "INSERT INTO CamPose(face_id,camYaw,camPitch,camRoll,annot_type_id) VALUES (?1, ?2, ?3, ?4, ?5)";
	_insertPoseSqlStmt = _sqlConn->prepare(_insertPoseSqlStmtStr);
}

bool CamPoseCalculator::calcStuff(FaceData *faceData)
{
	Pose pose = calcCamPose(faceData);
	std::cout << "  face: " << faceData->ID << std::endl;
	//std::cout << "    cam pose: " << pose.yaw << " " << pose.pitch << " " << pose.roll << " " << std::endl;

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

	//cout << "allOK ? " << (allOK ? "yes" : "no") << endl;

	if (allOK)
		_insertPoseSqlStmt->reset();
	return allOK;

	//return true;
}


Pose CamPoseCalculator::calcCamPose(FaceData *faceData)
{
	Pose camPose;
	FacePose *fp = faceData->getPose();

	cout << "r " << fp->roll << ", p " << fp->pitch << ", y " << fp->yaw << endl;

	/**********************************************************************************************************************
	 * attention:
	 *   1.) yaw and pitch in our DB are not counterclockwise when looking from the top of the rotation axis, as they should be
	 *   2.) in the model yaw is around Y, pitch is around X and roll is around Z, (ie, rotY(yaw)*rotX(pitch)*rotZ(roll))
	 *        BUT (and this is a big BUTT) splitRotationMatrixToRollPitchYaw assumes rotZ(yaw)*rotY(pitch)*rotX(roll)
	 **********************************************************************************************************************/
	cv::Mat objRot = FaceStuffCalculator::rotMatZ(-fp->yaw);
	//cout << objRot << endl;
	objRot = objRot * FaceStuffCalculator::rotMatY(-fp->pitch);
	//cout << objRot << endl;
	objRot = objRot * FaceStuffCalculator::rotMatX(fp->roll);
	//cout << objRot << endl;

	cv::Mat camRot = objRot.t(); // inverse == transpose
	//cout << camRot << endl;

	// DEBUG
	/*
	Pose tmp;
	splitRotationMatrixToRollPitchYaw(objRot, tmp.roll, tmp.pitch, tmp.yaw);
	cout << "    r " << tmp.roll << ", p " << tmp.pitch << ", y " << tmp.yaw << " ; " << tmp.roll*180.0/M_PI  << ", " << tmp.pitch*180.0/M_PI  << ", " << tmp.yaw*180.0/M_PI  << endl;
	 */

	splitRotationMatrixToRollPitchYaw(camRot, camPose.roll, camPose.pitch, camPose.yaw);

	//cout << "cam r " << camPose.roll << ", p " << camPose.pitch << ", y " << camPose.yaw << " ; " << camPose.roll*180.0/M_PI  << ", " << camPose.pitch*180.0/M_PI  << ", " << camPose.yaw*180.0/M_PI  <<  endl;

	return camPose;
}
