/*
 * FeaturesFileReader.h
 *
 *  Created on: 28.07.2010
 *      Author: pwohlhart
 */

#ifndef FEATURESFILEREADER_H_
#define FEATURESFILEREADER_H_

#include <string>
#include <vector>

using namespace std;


class FeaturesFileLineData {
public:
	string imageFilePath;
	string imageFilename;
	vector<pair<int,int> > coords;

	bool operator==(const std::string &filename) const {
		return imageFilename == filename;
	}

	static const string featuresFileFeatureCodes[];
	static const int numFeaturesFileFeatureCodes;
};

class FeaturesFileReader {
public:
	FeaturesFileReader();
	virtual ~FeaturesFileReader();

	bool load(std::string filename);

	vector<FeaturesFileLineData> data;
};


#endif /* FEATURESFILEREADER_H_ */
