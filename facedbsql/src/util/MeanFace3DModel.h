/*
 * MeanFace3DModel.h
 *
 *  Created on: 22.10.2010
 *      Author: pwohlhart
 */

#ifndef MEANFACE3DMODEL_H_
#define MEANFACE3DMODEL_H_

#include <string>
#include <map>

#include "cv.h"

#include "../dbconn/SQLiteDBConnection.h"

enum FaceCenterTypes { CENTER_BETWEEN_EYES, CENTER_FOR_ELLIPSE };

class MeanFace3DModel {
public:
	MeanFace3DModel();
	virtual ~MeanFace3DModel();

	cv::Point3f getCoordsByDescr(std::string featureName) const;
	cv::Point3f getCoordsByCode(std::string featureCode) const;

    //cv::Point3f center_between_eyes() const { return _center_between_eyes; }
    //cv::Point3f center_for_ellipse() const { return _center_for_ellipse; }
	cv::Point3f getCenter(FaceCenterTypes centerType);

	void load(SQLiteDBConnection *sqlConn);

    double sphereRadius() const { return _sphereRadius; }
private:
    std::map<std::string,cv::Point3f> _coords;
    std::map<std::string,cv::Point3f> _coordsByID;
    cv::Point3f _center_between_eyes;
    cv::Point3f _center_for_ellipse;
    double _sphereRadius;
};

#endif /* MEANFACE3DMODEL_H_ */
