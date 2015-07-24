/*
 * PoseFileReader.cpp
 *
 *  Created on: 29.07.2010
 *      Author: pwohlhart
 */

#include "PoseFileReader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../util/StringUtil.h"

PoseFileReader::PoseFileReader()
{
}

PoseFileReader::~PoseFileReader()
{
}

bool PoseFileReader::load(std::string filename)
{
	std::fstream fstr;
	fstr.open(filename.c_str(), std::ios::in );

	string line;

	while(!fstr.eof())
	{
		getline(fstr,line);

		// parse the line
		//    ex: \\jupiter\public\projects\mdl\data\facedb\flickr\3\image00035.jpg -0.128444 -0.611222 -0.587789

		string::size_type p = line.find(" ");

		PoseFileLineData poseData;

		// first is in the line if the image file
		poseData.imageFilename = line.substr(0,p);
		line = StringUtil::ltrim(line.substr(p+1));

		p = poseData.imageFilename.find_last_of("\\/");
		if (p != string::npos)
			poseData.imageFilename = poseData.imageFilename.substr(p+1);

		// read roll, pitch, yaw
		bool poseValid = false;
		for (int i = 0; i < 3; ++i)
		{
			p = line.find(" ");
			if (p == string::npos)
				p = line.size()-1;

			if (p > 0)
			{
				string valStr = line.substr(0,p);
				line = StringUtil::ltrim(line.substr(p+1));

				if (valStr.size() > 0)
				{
					std::stringstream valss(valStr);
					float val;
					if((valss >> val).fail())
						std::cout << "Error converting '"<< valStr <<"' to integer" << std::endl;

					switch (i) {
					case 0: poseData.roll = val; break;
					case 1: poseData.pitch = val; break;
					case 2: poseData.yaw = val; poseValid = true; break;
					}
				}
			}
			else
				break;
		}

		if ( (poseData.imageFilename.size() > 0) && poseValid )
			data.push_back(poseData);
	}

	fstr.close();
	return true;
}

