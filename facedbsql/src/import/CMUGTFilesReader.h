/*
 * CMUGTFilesReader.h
 *
 *  Created on: 08.11.2010
 *      Author: pwohlhart
 */

#ifndef CMUGTFILESREADER_H_
#define CMUGTFILESREADER_H_

#include <string>
#include <vector>
#include <map>

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"
#include "../facedata/FeaturesCoords.h"

class CMUGTFilesReader {
public:
	CMUGTFilesReader();
	virtual ~CMUGTFilesReader();

	void load(std::string gtFilesDir, SQLiteDBConnection* sqlConn, int annotTypeID_CMU);

	int numFaces();
	std::string getFileName(int fileNum);
	std::string getFilePath(int fileNum);
	FeaturesCoords getFeatureCoords(int fileNum);
private:
	void readFile(std::string filename);

	int getFeatureIdByCMUFeatureName(std::string featureName);
	void initFeatureIds(SQLiteDBConnection* sqlConn);

	std::map<std::string,int> _featureIDMap;

	std::vector<std::string> _filepaths;
	std::vector<std::string> _filenames;
	std::vector<FeaturesCoords> _featureCoords;

	int annotTypeID;
	//static const int CMU_ANNOTTYPE;
};

#endif /* CMUGTFILESREADER_H_ */
