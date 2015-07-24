/*
 * PutFileReader.h
 *
 *  Created on: 08.04.2012
 *      Author: szweimueller
 */

#ifndef PUTFILEREADER_H_
#define PUTFILEREADER_H_

#include <string>
#include <vector>
#include <map>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


class PutFileData {
public:
	std::string filenumber;
    std::string landmarksFilename;
    std::string regionsFilename;
    std::string imgFilename;
    vector<std::pair<double, double> > landmarks;
    vector<cv::Rect> regions;
    int numLandmarks;
    int numRegions;

	bool operator==(const std::string &filename) const {
		return filenumber == filename;
	}

    static const string featuresFileFeatureCodes[];
    static const string readFeaturesFileFeatureCodes[];
	static const int numFeaturesFileFeatureCodes;

    static const string regionsFileFeatureCodes[];
    static const string readRegionsFileFeatureCodes[];
	static const int numRegionsFileFeatureCodes;
};

class PutFileReader {
public:
	PutFileReader();
	virtual ~PutFileReader();
    
    bool loadImage(std::string filename);
	bool loadLandmarks(std::string filename);
    bool loadRegions(std::string filename);
    bool printPutData();
    bool drawLandmarks();

	std::map<int, PutFileData> m_putDataMap;
};


#endif /* PUTFILEREADER_H_ */
