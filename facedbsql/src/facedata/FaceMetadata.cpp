/*
 * FaceMetadata.cpp
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#include "FaceMetadata.h"

const std::string FaceMetadata::MALE = "m";
const std::string FaceMetadata::FEMALE = "f";
const std::string FaceMetadata::UNDEFINED = "";

const int FaceMetadata::UNCHECKED = 0;
const int FaceMetadata::CHECKED = 1;

FaceMetadata::FaceMetadata(int faceID) :
		_faceID(faceID),
		sex(FaceMetadata::UNDEFINED),
		occluded(FaceMetadata::UNCHECKED),
		glasses(FaceMetadata::UNCHECKED),
		bw(FaceMetadata::UNCHECKED),
		annot_type_id(0)
{

}

FaceMetadata::FaceMetadata():
		sex(FaceMetadata::UNDEFINED),
		occluded(FaceMetadata::UNCHECKED),
		glasses(FaceMetadata::UNCHECKED),
		bw(FaceMetadata::UNCHECKED),
		annot_type_id(0)
{

}

FaceMetadata::~FaceMetadata()
{
}

bool FaceMetadata::equals(const FaceMetadata& other)
{
	return (sex.compare(other.sex) == 0) && (occluded == other.occluded) && (glasses == other.glasses) && (bw == other.bw) && (annot_type_id == other.annot_type_id);
}
