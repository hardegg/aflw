/*
 * FaceMetadata.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FACEMETADATA_H_
#define FACEMETADATA_H_

#include <string>

using namespace std;

class FaceMetadata {
public:
	FaceMetadata(int faceID);
	virtual ~FaceMetadata();

	string sex;
	int occluded;
	int glasses;
	int bw;
	int annot_type_id;

	static const std::string MALE;
	static const std::string FEMALE;
	static const std::string UNDEFINED;

	static const int CHECKED;
	static const int UNCHECKED;

	int getFaceID() { return _faceID; }

	bool equals(const FaceMetadata& other);
private:
	int _faceID;
	FaceMetadata();
};

#endif /* FACEMETADATA_H_ */
