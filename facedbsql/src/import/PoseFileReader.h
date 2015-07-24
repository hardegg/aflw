/*
 * PoseFileReader.h
 *
 *  Created on: 29.07.2010
 *      Author: pwohlhart
 */

#ifndef POSEFILEREADER_H_
#define POSEFILEREADER_H_

#include <string>
#include <vector>

using namespace std;

class PoseFileLineData {
public:
	string imageFilename;
	float yaw;
	float pitch;
	float roll;
};

class PoseFileReader {
public:
	PoseFileReader();
	virtual ~PoseFileReader();

	bool load(std::string filename);

	vector<PoseFileLineData> data;
};

#endif /* POSEFILEREADER_H_ */
