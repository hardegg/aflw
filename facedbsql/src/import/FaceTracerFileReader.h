/*
 * FaceTracerReader.h
 *
 *  Created on: 01.07.2012
 *      Author: szweimueller
 */

#ifndef FACETRACERFILEREADER_H_
#define FAVETRACERFILEREADER_H_

#include <string>
#include <vector>
#include <map>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


class FaceTracerFileData {
public:
	std::string faceId;
	std::string faceIdFilename;
    std::string imageFoldername;
	std::string faceindexFilename;
    std::string facestatsFilename;
    std::string facelabelFilename;
	std::string attributeFilename;

    vector<std::pair<double, double> > facestats;
    vector<std::pair<std::string, std::string> > facelabels;
    int numFacestats;
    int numFacelabels;
    cv::Rect crop;
    float pitch;
    float roll;
    float yaw;
	std::string sex;
	std::string occluded;
	std::string glasses;
	std::string bw;

	bool operator==(const std::string &filename) const {
		return faceId == filename;
	}

    static const string featuresFileFeatureCodes[];
    static const int numFeaturesFileFeatureCodes;
    static const string fieldCodes[];
    static const int numFieldCodes;

    static const string attributeFileFeatureCodes[];
    static const string readAttributeFileFeatureCodes[];
	static const int numAttributeFileFeatureCodes;
};

class FaceTracerFileReader {
public:
	FaceTracerFileReader();
	virtual ~FaceTracerFileReader();
    
    bool loadFaceindex(const int faceId, std::string filename, std::string foldername);
	bool loadFacestats(const int faceId, std::string filename);
    bool loadFacelabel(const int faceId, std::string filename);
    vector<std::string> getLineById(const int faceId, std::string filename);
    cv::Mat getImageFromUrl(const char* url);
    bool printFaceTracerData();
    bool drawFacestats(const int faceId);

	std::map<int, FaceTracerFileData> m_faceTracerDataMap;
};


#endif /* FACETRACERFILEREADER_H_ */
