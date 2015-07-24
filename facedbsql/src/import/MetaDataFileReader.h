/*
 * FeaturesFileReader.h
 *
 *  Created on: 28.07.2010
 *      Author: koestinger
 */

#ifndef METADATAFILEREADER_H_
#define METADATAFILEREADER_H_

#include <string>
#include <map>

#include "../facedata/FaceMetadata.h"

class MetaDataFileLineData : public FaceMetadata {
public:
	MetaDataFileLineData();
	std::string imageFilePath;
	std::string imageFilename;
};

class MetaDataFileReader {
public:
	MetaDataFileReader();
	virtual ~MetaDataFileReader();

	bool load(std::string filename);

	std::map<std::string,MetaDataFileLineData> data;
};


#endif /* METADATAFILEREADER_H_ */
