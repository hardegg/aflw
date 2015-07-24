/*
 * FaceStuffCalculator.cpp
 *
 *  Created on: 07.12.2010
 *      Author: pwohlhart
 */

#include "FaceStuffCalculator.h"

FaceStuffCalculator::FaceStuffCalculator() : _useTransaction(true),manAnnotID(-1) {
}

FaceStuffCalculator::~FaceStuffCalculator() {
}

void FaceStuffCalculator::run(po::variables_map &vm, SQLiteDBConnection *sqlConn)
{
	// init
	_sqlConn = sqlConn;
	_fcTypes.load(sqlConn);

	_meanFace3DModel.load(_sqlConn);

	_lelcID = _fcTypes.getIDByCode("LELC"); // left eye left corner
	_lercID = _fcTypes.getIDByCode("LERC"); // left eye right corner
	_rercID = _fcTypes.getIDByCode("RERC"); // right eye left corner
	_relcID = _fcTypes.getIDByCode("RELC"); // right eye right corner

	std::string dbFile = vm["sqldb-file"].as<string>();
	_basePath = "";
	std::string::size_type pos = dbFile.find_last_of("\\/");
	if (pos != std::string::npos)
		_basePath = dbFile.substr(0,pos+1);

	// check which faces do not have a calculated rectangle
	string db_id;
	if (vm.count("db-id") > 0)
		db_id = vm["db-id"].as<std::string>();

	prepareQueryString();

	FaceIDQuery q;
	q.setQueryString(_faceQueryString);
	if (!q.exec(sqlConn))
	{
		std::cout << "ERROR: couldn't execute query for faces without rects" << std::endl;
		return;
	}
	//std::vector<int> facesWithoutRectsIDs = q.face_ids;

	// get face data
	FaceDataByIDsQuery faceDataQuery;
	faceDataQuery.queryIds = q.face_ids;//facesWithoutRectsIDs;
	std::cout << "Get data for " << q.face_ids.size() << " faces" << std::endl;
	if (faceDataQuery.exec(sqlConn))
	{
		if (_useTransaction)
		{
			_sqlConn->exec("PRAGMA synchronous = OFF;");
			_sqlConn->exec("BEGIN TRANSACTION;");
		}

		prepareSQLStuff();

		bool allOK = true;

		int numFaces = faceDataQuery.data.size();
		int count = 0;
		map<int,FaceData*>::iterator it;
		for (it = faceDataQuery.data.begin(); it != faceDataQuery.data.end(); ++it)
		{
			++count;
			FaceData *faceData = it->second;
			cout << "process face " << faceData->ID << " (" << count << "/" << numFaces << ")" << endl;
			allOK = calcStuff(faceData);
			if (!allOK)
				break;
		}

		if (_useTransaction)
		{
			if (allOK)
				_sqlConn->exec("COMMIT;");
			else
				_sqlConn->exec("ROLLBACK TRANSACTION;");
			_sqlConn->exec("PRAGMA synchronous = NORMAL;");
		}

	}
	else
		std::cout << "ERROR: couldn't get face data" << std::endl;

	_sqlConn = 0;
}

bool FaceStuffCalculator::calcCenterScaleAndUp(FaceData *faceData, double normEyeDist, cv::Point2f &center, double &scale, cv::Point2f &upv, FaceCenterTypes center_type)
{
	FaceDbImage *dbimg = faceData->getDbImg();
	if (!dbimg)
		return false;

	std::cout << "  image: " << dbimg->file_id << std::endl;

	double focalLength = static_cast<double>(dbimg->width) * 1.5;
	cv::Point2f imgCenter = cv::Point2f(static_cast<float>(dbimg->width) / 2.0f, static_cast<float>(dbimg->height) / 2.0f);

	cv::Mat rot;
	cv::Point3f trans;
	if (!getPose(faceData,focalLength,imgCenter,rot,trans))
		return false;

	// project center of the model to the image
	//cv::Point3f modelCenter = _meanFace3DModel.center_between_eyes();
	cv::Point3f modelCenter = _meanFace3DModel.getCenter(center_type);
	cv::Point3f rotatedCenter = transform(modelCenter,rot,trans);
	center = project(rotatedCenter,focalLength,imgCenter);

	// calc x and y extents of the projection of a sphere
	// centered at the model center (between eyes) and
	// with a diameter of the left to right eye distance
	double modelCenterDist = sqrt(rotatedCenter.x*rotatedCenter.x + rotatedCenter.y*rotatedCenter.y + rotatedCenter.z*rotatedCenter.z);
	double cameraModel3dYAngle = atan(rotatedCenter.y/sqrt(rotatedCenter.z*rotatedCenter.z + rotatedCenter.x*rotatedCenter.x));
	double cameraModel3dXAngle = atan(rotatedCenter.x/sqrt(rotatedCenter.z*rotatedCenter.z + rotatedCenter.y*rotatedCenter.y));
	double sphereCenterBorderAngle = asin(_meanFace3DModel.sphereRadius() / modelCenterDist);
	double sphereProjTop    = tan(cameraModel3dYAngle - sphereCenterBorderAngle) * focalLength;
	double sphereProjBottom = tan(cameraModel3dYAngle + sphereCenterBorderAngle) * focalLength;
	double sphereProjLeft   = tan(cameraModel3dXAngle - sphereCenterBorderAngle) * focalLength;
	double sphereProjRight  = tan(cameraModel3dXAngle + sphereCenterBorderAngle) * focalLength;

	scale = max(abs(sphereProjRight - sphereProjLeft),abs(sphereProjBottom - sphereProjTop)) / normEyeDist;

	// up vector
	if (!calcUpvFromEyes(faceData,upv))
	{
		// cout << "upv from pose" << endl;
		//cv::Point3f modelCenterUp = _meanFace3DModel.center_between_eyes();
		cv::Point3f modelCenterUp = _meanFace3DModel.getCenter(center_type);
		modelCenterUp.y += 0.5;
		cv::Point3f rotatedCenterUp = transform(modelCenterUp,rot,trans);
		cv::Point2f centerUp = project(rotatedCenterUp,focalLength,imgCenter);
		upv = centerUp - center;
		double upvlen = sqrt(upv.x*upv.x + upv.y*upv.y);
		upv.x /= upvlen;
		upv.y /= upvlen;
	}
	return true;
}


bool FaceStuffCalculator::getPose(FaceData *faceData, double focalLength, cv::Point2f imgCenter, cv::Mat &rot, cv::Point3f &trans, bool ogl)
{
	ModernPosit modernPosit;

	// prepare image pts and world pts
	FeaturesCoords *fc = faceData->getFeaturesCoords();
	if(fc == 0)
	{
		faceData->loadFeatureCoords(_sqlConn);
		fc = faceData->getFeaturesCoords();
	}

	std::vector<int> availableFeatureIds = fc->getFeatureIds();

	if (availableFeatureIds.size() < 2)
		return false;

	std::vector<cv::Point2f> imagePts;
	std::vector<cv::Point3f> worldPts;
	for (unsigned int i = 0; i < availableFeatureIds.size(); ++i)
	{
		int featureID = availableFeatureIds[i];
		cv::Point2f imgPt = fc->getCoords(featureID);
		if ((imgPt.x >= 0) && (imgPt.y >= 0))
		{
			imagePts.push_back(imgPt);
			std::string featureName = _fcTypes.getDescr(featureID);
			cv::Point3f worldPt = _meanFace3DModel.getCoordsByDescr(featureName);
			if (ogl)
				worldPt = cv::Point3f(worldPt.z,-worldPt.x,-worldPt.y);
			worldPts.push_back(worldPt);
		}
	}

	modernPosit.run(rot,trans,imagePts,worldPts,focalLength,imgCenter);
	return true;
}

cv::Point3f FaceStuffCalculator::transform(cv::Point3f pt, cv::Mat rot, cv::Point3f trans)
{
	cv::Point3f res;
	res.x = rot.at<float>(0,0)*pt.x + rot.at<float>(0,1)*pt.y + rot.at<float>(0,2)*pt.z + trans.x;
	res.y = rot.at<float>(1,0)*pt.x + rot.at<float>(1,1)*pt.y + rot.at<float>(1,2)*pt.z + trans.y;
	res.z = rot.at<float>(2,0)*pt.x + rot.at<float>(2,1)*pt.y + rot.at<float>(2,2)*pt.z + trans.z;
	return res;
}

cv::Point2f FaceStuffCalculator::project(cv::Point3f pt, double focalLength, cv::Point2f imgCenter)
{
	cv::Point2f res;
	res.x = (pt.x/pt.z) * focalLength + imgCenter.x;
	res.y = (pt.y/pt.z) * focalLength + imgCenter.y;
	return res;
}

bool FaceStuffCalculator::calcUpvFromEyes(FaceData *faceData, cv::Point2f &upv)
{
	FeaturesCoords *fc = faceData->getFeaturesCoords();
	if(fc == 0)
	{
		faceData->loadFeatureCoords(_sqlConn);
		fc = faceData->getFeaturesCoords();
	}

	cv::Point2f lelc = fc->getCoords(_lelcID);
	cv::Point2f lerc = fc->getCoords(_lercID);
	cv::Point2f rerc = fc->getCoords(_rercID);
	cv::Point2f relc = fc->getCoords(_relcID);

	if ((lelc.x >= 0) && (lerc.x >= 0) && (rerc.x >= 0) && (relc.x >= 0))
	{
		//cout << "upv from eyes" << endl;
		// all defined take mean of left-eye-center and right-eye-center
		cv::Point2f lec = lelc + lerc;
		lec.x /= 2;
		lec.y /= 2;
		cv::Point2f rec = relc + rerc;
		rec.x /= 2;
		rec.y /= 2;
		cv::Point2f lrV = rec - lec;
		double vlen = sqrt(lrV.x*lrV.x + lrV.y*lrV.y);
		upv.x = lrV.y / vlen;
		upv.y = - lrV.x / vlen;
		return true;
	}
	else
		return false;
}

int FaceStuffCalculator::getCalcFromManuallyLabeledFeaturePointsAnnotationTypeID()
{
	if (manAnnotID < 0)
		manAnnotID = getAnnotationTypeIDByCode("CALC");
	return manAnnotID;
}

int FaceStuffCalculator::getAnnotationTypeIDByCode(string code)
{
	int annotTypeId = 0;
	SQLiteStmt *findStmt = _sqlConn->prepare("SELECT annot_type_id FROM AnnotationType WHERE CODE=?1");
	findStmt->bind(1,code);
	int res = _sqlConn->step(findStmt);
	if (res == SQLITE_ROW)
	{
		if (!findStmt->readIntColumn(0,annotTypeId))
		{
			stringstream msgStrm;
			msgStrm << "error: couldn't read annotation type id for '" << code << "' from database" << endl;
			throw runtime_error(msgStrm.str());
		}
	}
	delete findStmt;
	return annotTypeId;
}

void FaceStuffCalculator::splitRotationMatrixToRollPitchYaw(cv::Mat &rot_matrix, double& roll, double& pitch, double& yaw)
{
	   // do we have to transpose here?
   const double a11 = rot_matrix.at<float>(0,0), a12 = rot_matrix.at<float>(0,1), a13 = rot_matrix.at<float>(0,2);
   const double a21 = rot_matrix.at<float>(1,0), a22 = rot_matrix.at<float>(1,1), a23 = rot_matrix.at<float>(1,2);
   const double a31 = rot_matrix.at<float>(2,0), a32 = rot_matrix.at<float>(2,1), a33 = rot_matrix.at<float>(2,2);

   //replaces vnl_math header
   const double epsilon = 2.2204460492503131e-16;
   double pi = 3.14159265358979323846;
   double pi_over_2 = 1.57079632679489661923;
   double pi_over_4 = 0.78539816339744830962;


   if ( abs(1.0 - a31) <= epsilon ) // special case a31 == +1
   {
      std::cout << "gimbal lock case a31 == " << a31 << std::endl;
      pitch = -pi_over_2;
      yaw   = pi_over_4; // arbitrary value
      roll  = atan2(a12,a13) - yaw;
   }
   else if ( abs(-1.0 - a31) <= epsilon ) // special case a31 == -1
   {
      std::cout << "gimbal lock case a31 == " << a31 << std::endl;
      pitch = pi_over_2;
      yaw   = pi_over_4; // arbitrary value
      roll  = atan2(a12,a13) + yaw;
   }
   else // standard case a31 != +/-1
   {
      pitch = asin(-a31);
      //two cases depending on where pitch angle lies
      if ( (pitch < pi_over_2) && (pitch > -pi_over_2) )
      {
         roll = atan2(a32,a33);
         yaw  = atan2(a21,a11);
      }
      else if ( (pitch < 3.0 * pi_over_2) && (pitch > pi_over_2) )
      {
         roll = atan2(-a32,-a33);
         yaw  = atan2(-a21,-a11);
      }
      else
      {
         std::cerr << "this should never happen in pitch roll yaw computation!" << std::endl;
         roll = 2.0 * pi;
         yaw  = 2.0 * pi;
      }
   }
}


cv::Mat FaceStuffCalculator::rotMatX(double alpha)
{
	float sa = sin(alpha);
	float ca = cos(alpha);
	return (cv::Mat_<float>(4, 4) << 1,0,0,0, 0,ca,-sa,0, 0,sa,ca,0, 0,0,0,1);
}

cv::Mat FaceStuffCalculator::rotMatY(double alpha)
{
	float sa = sin(alpha);
	float ca = cos(alpha);
	return (cv::Mat_<float>(4, 4) << ca,0,sa,0, 0,1,0,0, -sa,0,ca,0, 0,0,0,1);
}

cv::Mat FaceStuffCalculator::rotMatZ(double alpha)
{
	float sa = sin(alpha);
	float ca = cos(alpha);
	return (cv::Mat_<float>(4, 4) << ca,-sa,0,0, sa,ca,0,0, 0,0,1,0, 0,0,0,1);
}
