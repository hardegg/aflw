/*
 * FeaturesCoords.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FEATURESCOORDS_H_
#define FEATURESCOORDS_H_

#include <vector>
#include <map>

#include "cv.h"

// fwd decl
class FeaturesCoords;

#include "../dbconn/SQLiteDBConnection.h"
#include "FaceData.h"


class FeaturesCoords : public IDataCallbackObj {
public:
	FeaturesCoords(int faceId);
	FeaturesCoords(const FeaturesCoords &coords);
	virtual ~FeaturesCoords();

	//FloatCoords getCoords(int featureId) const;
	//void setCoords(int featureId, FloatCoords coords);
	std::vector<int> getFeatureIds() const;
	cv::Point2f getCoords(int featureId) const;
	void setCoords(int featureId, cv::Point2f coords, int annotType = 0);

	void clear();
	void load(SQLiteDBConnection *sqlConn, int annotTypeID = 0);
	//void save(SQLiteDBConnection *sqlConn);

	//int recvData(int argc, char **argv, char **azColName);
	int dataRowReady(SQLiteDBConnection *sqlConn);

	int getAnnotType(int featureId) const;

	void debugPrint();
private:
	int _faceId;
	//map<int,FloatCoords > _coordsByFeatureId;
	std::map<int,cv::Point2f > _coordsByFeatureId;
	std::map<int,int > _annotTypeID;
};


/*
class FeatureCoords {
public:
	FeatureCoords();
	//FeatureCoords(const FeatureCoords &coords);
	virtual ~FeatureCoords();
private:
	int face_id;
	int feature_id;
	float x;
	float y;
};
*/


#endif /* FEATURESCOORDS_H_ */
