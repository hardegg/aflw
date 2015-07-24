/*
 * FeaturesFileReader.cpp
 *
 *  Created on: 28.07.2010
 *      Author: pwohlhart
 */

#include "MetaDataFileReader.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "boost/algorithm/string.hpp"

#include "../util/StringUtil.h"

MetaDataFileLineData::MetaDataFileLineData() : FaceMetadata(-1)
{

}

MetaDataFileReader::MetaDataFileReader()
{

}

MetaDataFileReader::~MetaDataFileReader()
{
}

bool MetaDataFileReader::load(std::string filename)
{
	fstream fstr;
	fstr.open(filename.c_str(), ios::in );

	string line;
	getline(fstr,line); // skip first line

	while(!fstr.eof())
	{
		getline(fstr,line);

		// parse the line

		string::size_type p = line.find(" ");

		MetaDataFileLineData metaData;
		metaData.imageFilePath = line.substr(0,p);
		line = line.substr(p+1);

		p = metaData.imageFilePath.find_last_of("\\/");
		if (p != string::npos)
			metaData.imageFilename = metaData.imageFilePath.substr(p+1);
		else
			metaData.imageFilename = metaData.imageFilePath;

		bool metaDataValid = false;
		for(int i = 0; i < 4; ++i)
		{
			p = line.find(" ");
			if (p == string::npos)
				p = line.size()-1;

			if (p >= 0)
			{
				string valStr = line.substr(0,p > 0 ? p : 1);
				line = StringUtil::ltrim(line.substr(p+1));

				if (valStr.size() > 0)
				{
					std::stringstream valss(valStr);
					bool val;
					if((valss >> val).fail())
						std::cout << "Error converting '"<< valStr <<"' to bool" << std::endl;

					switch (i) {
					case 0: metaData.sex = val ? FaceMetadata::FEMALE : FaceMetadata::MALE; break;
					case 1: metaData.glasses = !val ? FaceMetadata::UNCHECKED : FaceMetadata::CHECKED; break;
					case 2: metaData.occluded = !val ? FaceMetadata::UNCHECKED : FaceMetadata::CHECKED; true; break;
					case 3: metaData.bw = !val ? FaceMetadata::UNCHECKED : FaceMetadata::CHECKED; metaDataValid = true; break;
					}
				}
			}
			else
				break;
		}

		if ( (metaData.imageFilename.size() > 0) && metaDataValid )
			data[metaData.imageFilename] = metaData;
	}

	fstr.close();
	return true;
}
