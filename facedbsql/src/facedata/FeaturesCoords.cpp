/*
 * FeaturesCoords.cpp
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#include "FeaturesCoords.h"

#include <sstream>

FeaturesCoords::FeaturesCoords(int faceId)
{
	_faceId = faceId;
//	_parent->setFeatureCoords(this);
}

FeaturesCoords::FeaturesCoords(const FeaturesCoords &coords)
{
	_faceId = coords._faceId;
	//_parent = parent;
	//_parent->setFeatureCoords(this);

	std::vector<int> featureIds = coords.getFeatureIds();
	for (unsigned int i = 0; i < featureIds.size(); ++i)
		setCoords(featureIds[i],coords.getCoords(featureIds[i]),coords.getAnnotType(featureIds[i]));
}

FeaturesCoords::~FeaturesCoords()
{
}

//FloatCoords FeaturesCoords::getCoords(int featureId) const
cv::Point2f FeaturesCoords::getCoords(int featureId) const
{
	//map<int,FloatCoords >::const_iterator it = _coordsByFeatureId.find(featureId);
	map<int,cv::Point2f>::const_iterator it = _coordsByFeatureId.find(featureId);
	if (it != _coordsByFeatureId.end())
		return it->second;
	else
		//return FloatCoords();
		return cv::Point2f(-1.0,-1.0);
}

int FeaturesCoords::getAnnotType(int featureId) const
{
	map<int,int>::const_iterator it = _annotTypeID.find(featureId);
	if (it != _annotTypeID.end())
		return it->second;
	else
		return -1;
}

//void FeaturesCoords::setCoords(int featureId, FloatCoords coords)
void FeaturesCoords::setCoords(int featureId, cv::Point2f coords, int annotType)
{
	_coordsByFeatureId[featureId] = coords;
	_annotTypeID[featureId] = annotType;
}

//int FeaturesCoords::recvData(int argc, char **argv, char **azColName) { }

int FeaturesCoords::dataRowReady(SQLiteDBConnection *sqlConn)
{
	int feature_id;
	double x;
	double y;
	int annotType;

	bool res = sqlConn->readIntColumn(0,feature_id);
	res = res && sqlConn->readDoubleColumn(1,x);
	res = res && sqlConn->readDoubleColumn(2,y);
	res = res && sqlConn->readIntColumn(3,annotType);
	if (res)
	{
		//FloatCoords coords(x,y);
		cv::Point2f coords(x,y);
		_coordsByFeatureId[feature_id] = coords;
		_annotTypeID[feature_id] = annotType;
		return 0;
	}
	else
		return 1;
}

void FeaturesCoords::clear()
{
	_coordsByFeatureId.clear();
	_annotTypeID.clear();
}

void FeaturesCoords::load(SQLiteDBConnection *sqlConn, int annotTypeID)
{
	clear();

	std::stringstream querySStr;
	querySStr << "SELECT feature_id,x,y,annot_type_id FROM featurecoords WHERE face_id = " << _faceId << " and annot_type_id = " << annotTypeID;
	//querySStr << "SELECT feature_id,x,y,annot_type_id FROM featurecoords WHERE face_id = " << _faceId;
	string query = querySStr.str();

	sqlConn->exec(query,this);
}

/*
void FeaturesCoords::save(SQLiteDBConnection *sqlConn)
{
	std::map<int,cv::Point2f>::const_iterator it;
	for (it = _coordsByFeatureId.begin(); it != _coordsByFeatureId.end(); ++it)
	{
		std::stringstream querySStr;
		querySStr << "UPDATE featurecoords SET x = " << it->second.x << ", y = " << it->second.y << " WHERE face_id = " << _faceId << " and feature_id = " << it->first << " and annot_type_id = " << _annotTypeID[it->first];
		string query = querySStr.str();

		sqlConn->exec(query);
	}
}
*/

void FeaturesCoords::debugPrint()
{
	cout << "  feature coordinates:" << endl;
	//map<int,FloatCoords>::const_iterator it;
	std::map<int,cv::Point2f>::const_iterator it;
	for (it = _coordsByFeatureId.begin(); it != _coordsByFeatureId.end(); ++it)
		cout << "    fid " << it->first << " => ( " << it->second.x << " , " << it->second.y << " )" << endl;
}

std::vector<int> FeaturesCoords::getFeatureIds() const
{
	std::vector<int> fids;
	std::map<int,cv::Point2f>::const_iterator it;
	for (it = _coordsByFeatureId.begin(); it != _coordsByFeatureId.end(); ++it)
		fids.push_back(it->first);
	return fids;
}
