/*
 * LFPWFilesReader.h
 *
 *  Created on: 08.10.2012
 *      Author: Elena Wirtl
 */

#ifndef LFPWFilesReader_H_
#define LFPWFilesReader_H_

#include <string>
#include <vector>
#include <list>
#include <map>

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"
#include "../facedata/FeaturesCoords.h"

class LFPWFilesReader {
public:
	LFPWFilesReader();
	virtual ~LFPWFilesReader();

	void load(std::string lfpwFilesDir, int annotTypeID_LFPW);

	int numFaces();
	std::string getFileName(int fileNum);
	std::string getFilePath(int fileNum);
	std::string getTotalFileName(int fileNum);
	bool getFileVisibility(int fileNum);
	FeaturesCoords getFeatureCoords(int fileNum);
private:
	void readFile(std::string filename, std::string dir, bool test);
	
	void readValidFile(std::string dir);

    int getAFLWFeatureNumber(int lfpwFeatureNumber);
    
    bool validImg(std::string number, bool test);

	std::vector<std::string> _filepaths;
	std::vector<std::string> _filenames;
	std::vector<std::string> _totalfilenames;
	std::vector<bool> _visibility;
	std::vector<FeaturesCoords> _featureCoords;
	
	std::vector<std::string> _invalidTest;
	std::vector<std::string> _invalidTrain;
	
	int annotTypeID;
	
	static const int numFeaturesFileFeatureCodes;
	static const int fileNameLength;
	static const std::string extension;
	static const std::string valitatorFileName;
};

#endif /* LFPWFilesReader_H_ */
