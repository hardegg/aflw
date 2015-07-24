/*
 * FeaturesFileReader.cpp
 *
 *  Created on: 28.07.2010
 *      Author: pwohlhart
 */

#include "FeaturesFileReader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../util/StringUtil.h"

//vector<string> FeaturesFileReader::featuresFileColumnNames;
//FeaturesFileReader::featuresFileColumnNames.
//const string FeaturesFileReader::featuresFileColumnNames[]

// LBLC LBC LBRC RBLC RBC RBRC LELC LEC LERC RELC REC RERC LE LN NC RN RE MLC MC MRC CC

const int FeaturesFileLineData::numFeaturesFileFeatureCodes = 21;

const string FeaturesFileLineData::featuresFileFeatureCodes[FeaturesFileLineData::numFeaturesFileFeatureCodes] =
	{ "LBLC", "LBC", "LBRC", "RBLC", "RBC", "RBRC", "LELC", "LEC", "LERC", "RELC", "REC", "RERC", "LE", "LN", "NC", "RN", "RE", "MLC", "MC", "MRC", "CC" };



FeaturesFileReader::FeaturesFileReader()
{
}

FeaturesFileReader::~FeaturesFileReader()
{
}

bool FeaturesFileReader::load(std::string filename)
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

		FeaturesFileLineData featuresData;
		featuresData.imageFilePath = line.substr(0,p);
		line = line.substr(p+1);

		p = featuresData.imageFilePath.find_last_of("\\/");
		if (p != string::npos)
			featuresData.imageFilename = featuresData.imageFilePath.substr(p+1);
		else
			featuresData.imageFilename = featuresData.imageFilePath;

		while (line.size() > 0)
		{
			p = line.find(" ");
			if (p == string::npos)
				p = line.size()-1;
			string coordStr = line.substr(0,p);
			line = StringUtil::ltrim(line.substr(p+1));

			if (coordStr.size() > 0)
			{
				p = coordStr.find(";");
				string xStr;
				string yStr;
				xStr = coordStr.substr(0,p);
				yStr = coordStr.substr(p+1);

				stringstream xss(xStr);
				int x;
				if((xss >> x).fail())
					cout << "Error converting '"<< xStr <<"' to integer" << endl;
				stringstream yss(yStr);
				int y;
				if((yss >> y).fail())
					cout << "Error converting '"<< yStr <<"' to integer" << endl;

				featuresData.coords.push_back(pair<int,int>(x,y));
			}
		}

		if ( (featuresData.imageFilename.size() > 0) && (featuresData.coords.size() > 0) )
			data.push_back(featuresData);
	}

	fstr.close();
	return true;
}
