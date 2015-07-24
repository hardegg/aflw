/*
 * FaceData.h
 *
 *  Created on: 26.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEDATA_H_
#define FACEDATA_H_

#include <string>
#include <vector>
#include <map>
#include "../dbconn/SQLiteDBConnection.h"

// fwd decl
class FaceData;

#include "FaceDbImage.h"
#include "FeaturesCoords.h"
#include "FeatureCoordTypes.h"
#include "FacePose.h"
#include "FaceMetadata.h"
#include "FaceRect.h"


using namespace std;

struct FaceEllipse
{
	FaceEllipse();
	float x;
	float y;
	float ra;
	float rb;
	float theta;
	int annot_type_id;
	bool upsideDown;

	bool equals(const FaceEllipse &other);
};

class FaceData {
public:
	FaceData();
	FaceData(const FaceData &fd);
	virtual ~FaceData();

	int ID;
	string dbID;
	string fileID;

	void setDbImg(FaceDbImage *img);
	FaceDbImage *getDbImg() const { return _img; };

	void setMetadata(FaceMetadata *metadata);
	FaceMetadata *getMetadata() const { return _meta; };

	void setPose(FacePose *pose);
	FacePose *getPose() const { return _pose; };

	void setCamPose(Pose *camPose);
	Pose *getCamPose() const { return _camPose; };

	void setFeatureCoords(FeaturesCoords *fcoords);
	FeaturesCoords *getFeaturesCoords() const { return _fcoords; };

	void setEllipse(FaceEllipse ellipse);
	FaceEllipse getEllipse(int annotTypeID) const;
	void setEllipses(vector<FaceEllipse> ellipses);
	vector<FaceEllipse> getEllipses() const { return _ellipses; };
	bool hasEllipse() const { return _ellipses.size() > 0; }

	vector<FaceRect> getRects() { return _rects; }

	bool save(SQLiteDBConnection *sqlConn);

	void loadFeatureCoords(SQLiteDBConnection *sqlConn);
	void loadRects(SQLiteDBConnection *sqlConn);
	void loadEllipses(SQLiteDBConnection *sqlConn);

	void debugPrint();

        void SaveFaceRegion_rect(const string& rootFolder, const string& dstFolderPath, vector<ofstream*>& ofDetects, vector<ofstream*>& ofCalibs);

private:
	FaceDbImage *_img;
	FaceMetadata *_meta;
	FacePose *_pose;
	Pose *_camPose; // could be calculated online from _pose

	FeaturesCoords *_fcoords;
	//bool _hasEllipse;
	vector<FaceEllipse> _ellipses;
	vector<FaceRect> _rects;
};


#endif /* FACEDATA_H_ */
