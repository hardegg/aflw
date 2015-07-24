/*
 * MeanFace3DModel.cpp
 *
 *  Created on: 22.10.2010
 *      Author: pwohlhart
 */

#include "MeanFace3DModel.h"

MeanFace3DModel::MeanFace3DModel() {
/*
    _coords["MouthCenter"] =           	cv::Point3f(-0.072, -0.346, -0.333);
    _coords["MouthLeftCorner"] =       	cv::Point3f(-0.296, -0.348, -0.472);
    _coords["MouthRightCorner"] =      	cv::Point3f(0.156, -0.366, -0.417);
    _coords["LeftEyeCenter"] =         	cv::Point3f(-0.361, 0.309, -0.521);
    _coords["LeftEyeLeftCorner"] =     	cv::Point3f(-0.4796, 0.3, -0.604);
    _coords["LeftEyeRightCorner"] =    	cv::Point3f(-0.221, 0.296, -0.512);
    _coords["RightEyeCenter"] =        	cv::Point3f(0.235, 0.296, -0.46);
    _coords["RightEyeLeftCorner"] =    	cv::Point3f(0.105, 0.291, -0.474);
    _coords["RightEyeRightCorner"] =   	cv::Point3f(0.369, 0.285, -0.512);
    _coords["NoseLeft"] =              	cv::Point3f(-0.225169, -0.0925778, -0.407935);
    _coords["NoseCenter"] =            	cv::Point3f(-0.071, -0.023, -0.13);
    _coords["NoseRight"] =             	cv::Point3f(-0.0945821, -0.0943744, -0.362066);
    _coords["LeftEar"] =               	cv::Point3f(-0.649, -0.198, -1.269);
    _coords["RightEar"] =              	cv::Point3f(0.593, -0.236, -1.119);
    _coords["LeftBrowCenter"] =        	cv::Point3f(-0.44, 0.502, -0.469);
    _coords["RightBrowCenter"] =       	cv::Point3f(0.283, 0.486, -0.376);
    _coords["LeftBrowLeftCorner"] =    	cv::Point3f(-0.575, 0.479, -0.64);
    _coords["LeftBrowRightCorner"] =   	cv::Point3f(-0.207, 0.444, -0.388);
    _coords["RightBrowLeftCorner"] =   	cv::Point3f(0.11, 0.445, -0.358);
    _coords["RightBrowRightCorner"] =  	cv::Point3f(0.46, 0.472, -0.510);
    _coords["ChinCenter"] = 				cv::Point3f(-0.068, -0.703, -0.387);

    _coords["MouthCenterUpperLipOuterEdge"] =  cv::Point3f(-0.0682077, -0.284391, -0.291888);
    _coords["MouthCenterLowerLipOuterEdge"] =  cv::Point3f(-0.0662728, -0.383458, -0.318104);
    _coords["LeftNostril"] =                   cv::Point3f(-0.142852, -0.079903, -0.269774);
    _coords["RightNostril"] =                  cv::Point3f(0.0108861, -0.0784876, -0.259027);
    _coords["LeftTemple"] =                    cv::Point3f(-0.593385, 0.303609, -0.719594);
    _coords["RightTemple"] =                   cv::Point3f(0.497715, 0.276949, -0.613964);

    //featurenames = fieldnames(model3d);

    _center_between_eyes = cv::Point3f(-0.056, 0.3, -0.530 );
    //_center_for_ellipse = cv::Point3f(-0.056, 0.18, -0.686 );
    _center_for_ellipse = cv::Point3f(-0.056, 0.18, -0.75 );

    _sphereRadius = 0.63 / 2.0;
    */
	_sphereRadius = 1.0;
	_center_between_eyes = cv::Point3f(0.0,0.0,0.0);
	_center_for_ellipse = cv::Point3f(0.0,0.0,0.0);
}

MeanFace3DModel::~MeanFace3DModel() {
}

cv::Point3f MeanFace3DModel::getCoordsByDescr(std::string featureName) const
{
	std::map<std::string,cv::Point3f>::const_iterator it = _coords.find(featureName);
	if (it != _coords.end())
		return it->second;
	else
		return cv::Point3f(0.0f,0.0f,0.0f);
}

cv::Point3f MeanFace3DModel::getCoordsByCode(std::string featureCode) const
{
	std::map<std::string,cv::Point3f>::const_iterator it = _coordsByID.find(featureCode);
	if (it != _coordsByID.end())
		return it->second;
	else
		return cv::Point3f(0.0f,0.0f,0.0f);
}

cv::Point3f MeanFace3DModel::getCenter(FaceCenterTypes centerType)
{
	switch (centerType) {
		case CENTER_BETWEEN_EYES:
			return _center_between_eyes;
			break;
		case CENTER_FOR_ELLIPSE:
			return _center_for_ellipse;
			break;
		default:
			return _center_between_eyes;
	}
}

void MeanFace3DModel::load(SQLiteDBConnection *sqlConn)
{
	SQLiteStmt *stmt = sqlConn->prepare("SELECT descr,code,x,y,z FROM FeatureCoordTypes");
	int res = 0;
	do
	{
		bool allOK = true;
		string descr;
		string code;
		double x = 0, y = 0, z = 0;

		res = sqlConn->step(stmt);
		allOK = allOK && stmt->readStringColumn(0,descr);
		allOK = allOK && stmt->readStringColumn(1,code);
		allOK = allOK && stmt->readDoubleColumn(2,x);
		allOK = allOK && stmt->readDoubleColumn(3,y);
		allOK = allOK && stmt->readDoubleColumn(4,z);

		_coords[descr] = cv::Point3f(x,y,z);
		_coordsByID[code] = cv::Point3f(x,y,z);

	} while (res == SQLITE_ROW);

	cv::Point3f lec =  _coordsByID["LEC"];
	cv::Point3f rec =  _coordsByID["REC"];

	_center_between_eyes = cv::Point3f( (lec.x + rec.x)/2.0, (lec.y + rec.y)/2.0, (lec.z + rec.z)/2.0 );

    _center_for_ellipse = _coordsByID["HC"];

    double dx = lec.x - rec.x;
    double dy = lec.y - rec.y;
    double dz = lec.z - rec.z;
    double d = sqrt(dx*dx + dy*dy + dz*dz);
    _sphereRadius = d / 2.0;

	delete stmt;
}
