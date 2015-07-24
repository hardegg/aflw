/*
 * CMUGTFilesReader.cpp
 *
 *  Created on: 08.11.2010
 *      Author: pwohlhart
 */

#include "CMUGTFilesReader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "boost/regex.hpp"

//const int CMUGTFilesReader::CMU_ANNOTTYPE = 87; // ? TODO: enter this in the AnnotationTypes table, with a description

CMUGTFilesReader::CMUGTFilesReader() {

}

CMUGTFilesReader::~CMUGTFilesReader() {
}

void CMUGTFilesReader::load(std::string gtFilesDir, SQLiteDBConnection* sqlConn,int annotTypeID_CMU)
{
	annotTypeID = annotTypeID_CMU;

	initFeatureIds(sqlConn);

	std::string profile_gt_file = gtFilesDir + "testing_profile_ground_truth.dat";
	readFile(profile_gt_file);

	std::string frontal_gt_file = gtFilesDir + "testing_frontal_ground_truth.dat";
	readFile(frontal_gt_file);
}

void CMUGTFilesReader::readFile(std::string filename)
{
	std::ifstream input;
	std::string line;
	input.open(filename.c_str());

	boost::regex lineFormat("(\\S*)( \\{(\\w*) ((\\d*(.\\d*)?) (\\d*(.\\d*)?)|\\w*)\\})*");
	boost::regex attrFormat("\\{(\\w*) ((\\d*(.\\d*)?) (\\d*(.\\d*)?)|\\w*)\\}\\s*");
	while (std::getline(input,line))
	{
		boost::smatch m;
		boost::regex_match(line,m,lineFormat);
		if (m[0].matched)
		{
			FeaturesCoords fc(0);

			std::string filename = m[1].str();
			std::string filepath = filename;
			int p = filename.find_last_of('/');
			if (p != std::string::npos)
				filename = filename.substr(p+1);
			_filenames.push_back(filename);
			_filepaths.push_back(filepath);
			line = line.substr(m[1].length()+1);

			std::cout << filename << std::endl;

			//std::cout << line << std::endl;

			boost::smatch res;
			boost::regex_search(line,res,attrFormat);
			while (res[0].matched)
			{
				//std::cout << std::string(res[0].first,res[0].second) << std::endl;

				if (res[3].length() > 0)
				{
					// point spec

					//for (int i = 0; i < res.size(); ++i)
					//	std::cout << i << ": " << res.str(i) << std::endl;

					cv::Point2f pt;
					std::string feature = res[1].str();
					std::stringstream xStr;
					xStr << res[3];
					xStr >> pt.x;
					std::stringstream yStr;
					yStr << res[5];
					yStr >> pt.y;

					std::cout << "'" << feature << "': (" << pt.x << "," << pt.y << ")" << std::endl;
					int featureID = getFeatureIdByCMUFeatureName(feature);

					if (featureID >= 0)
						fc.setCoords(featureID,pt,annotTypeID);
				}
				else
				{
					// valid/invalid spec
					// TODO: ?? what to do with it?
				}

				// next attr
				//int p = res[0].second - res[0].first;
				line = line.substr(res[0].length());

				//res = boost::smatch();
				boost::regex_search(line,res,attrFormat);
			}

			// line processed, filename available, feature coords are ready -> store
			_featureCoords.push_back(fc);
		}

	}

}

void CMUGTFilesReader::initFeatureIds(SQLiteDBConnection* sqlConn)
{
	FeatureCoordTypes fct;
	fct.load(sqlConn);

	// right and left are swapped
	_featureIDMap["reyecorner"] = fct.getIDByCode("LELC");
	_featureIDMap["reye"] = fct.getIDByCode("LEC");
	_featureIDMap["nose"] = fct.getIDByCode("NC");
	_featureIDMap["nosetip"] = fct.getIDByCode("NC");
	_featureIDMap["rmouth"] = fct.getIDByCode("MLC");
	_featureIDMap["cmouth"] = fct.getIDByCode("MC");
	_featureIDMap["lmouth"] = fct.getIDByCode("MRC");
	_featureIDMap["chin"] = fct.getIDByCode("CC");
	_featureIDMap["rightearlobe"] = fct.getIDByCode("LE");
	_featureIDMap["leyecorner"] = fct.getIDByCode("RERC");
	_featureIDMap["leye"] = fct.getIDByCode("REC");
	_featureIDMap["leftearlobe"] = fct.getIDByCode("RE");

	//_featureIDMap["righteartip"] = fct.getIDByCode("LET"); // ? we dont have LET and RET
	//_featureIDMap["lefteartip"] = fct.getIDByCode("RET");

	//"rightearvalid"
	//"leftearvalid"
	//"backheadvalid"
}

int CMUGTFilesReader::getFeatureIdByCMUFeatureName(std::string featureName)
{
	std::map<std::string,int>::iterator it = _featureIDMap.find(featureName);
	if (it != _featureIDMap.end())
		return it->second;
	else
		return -1;
}

int CMUGTFilesReader::numFaces()
{
	return _featureCoords.size();
}

FeaturesCoords CMUGTFilesReader::getFeatureCoords(int fileNum)
{
	if (fileNum < _featureCoords.size())
		return _featureCoords[fileNum];
	else
		return FeaturesCoords(0);
}

std::string CMUGTFilesReader::getFileName(int fileNum)
{
	if (fileNum < _filenames.size())
		return _filenames[fileNum];
	else
		return "";
}

std::string CMUGTFilesReader::getFilePath(int fileNum)
{
	if (fileNum < _filepaths.size())
		return _filepaths[fileNum];
	else
		return "";
}

