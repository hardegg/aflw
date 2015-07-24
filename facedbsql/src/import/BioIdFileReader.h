/*
 * BioIdFileReader.h
 *
 *  Created on: 08.04.2012
 *      Author: szweimueller
 */

#ifndef BIOIDFILEREADER_H_
#define BIOIDFILEREADER_H_

#include <string>
#include <vector>
#include <map>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


class BioIdFileData {
public:
	std::string filenumber;
    std::string eyeFilename;
    std::string pointsFilename;
    std::string imgFilename;
	std::pair<int, int> leftEye;
    std::pair<int, int> rightEye;
    vector<std::pair<double, double> > points;
    int numPoints;

	bool operator==(const std::string &filename) const {
		return filenumber == filename;
	}

    static const string featuresFileFeatureCodes[];
	static const int numFeaturesFileFeatureCodes;
};

class BioIdFileReader {
public:
	BioIdFileReader();
	virtual ~BioIdFileReader();
    
    bool loadImage(std::string filename);
	bool loadEyes(std::string filename);
    bool loadPoints(std::string filename);
    bool printBioIdData();
    bool drawLandmarks();

	std::map<int, BioIdFileData> m_bioIdDataMap;
};


#endif /* BIOIDFILEREADER_H_ */
