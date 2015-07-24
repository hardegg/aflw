/*
 * FaceDbImage.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEDBIMAGE_H_
#define FACEDBIMAGE_H_

#include <string>

class FaceDbImage {
public:
	FaceDbImage();
	virtual ~FaceDbImage();

	int image_id;
	std::string db_id;
	std::string file_id;
	std::string filepath;
	bool bw;
	int width;
	int height;
	std::string dbpath;
};

#endif /* FACEDBIMAGE_H_ */
