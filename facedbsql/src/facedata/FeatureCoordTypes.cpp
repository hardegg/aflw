/*
 * FeatureCoordTypes.cpp
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#include <iostream>
#include <string>

#include "FeatureCoordTypes.h"

const string FeatureCoordTypes::queryString = "SELECT * FROM featurecoordtypes";

using namespace std;

FeatureCoordTypes::FeatureCoordTypes()
{
}

FeatureCoordTypes::~FeatureCoordTypes()
{
}


int FeatureCoordTypes::getIDByDescr(std::string descr)
{
	std::map<int,std::string>::const_iterator it;
	for (it = featureTypes.begin(); it != featureTypes.end(); ++it)
	{
		if (it->second == descr)
			return it->first;
	}
	return -1;
}

vector<int> FeatureCoordTypes::getTypeIds()
{
	vector<int> ids;
	map<int,string>::iterator it;
	for (it = featureTypes.begin(); it != featureTypes.end(); ++it)
		ids.push_back(it->first);
	return ids;
}

bool FeatureCoordTypes::load(SQLiteDBConnection *sqliteConn)
{
	return sqliteConn->exec(FeatureCoordTypes::queryString,this) == 0;
}

int FeatureCoordTypes::dataRowReady(SQLiteDBConnection *sqlConn)
{
	//"feature_id"
	//"descr"
	int id;
	if (!sqlConn->readIntColumn(0,id))
		return 1;
	string descr;
	if (!sqlConn->readStringColumn(1,descr))
		return 1;
	string code;
	if (!sqlConn->readStringColumn(2,code))
		return 1;

	featureTypes[id] = descr;
	featureCodes[id] = code;
	return 0;
}

/*
int FeatureCoordTypes::recvData(int argc, char **argv, char **azColName)
{
	int id_idx = -1;
	int descr_idx = -1;
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(azColName[i],"feature_id") == 0)
			id_idx = i;
		if (strcmp(azColName[i],"descr") == 0)
			descr_idx = i;
	}
	if ((id_idx < 0) || (descr_idx < 0))
		return 1;

	stringstream ss(argv[id_idx]);
	int id;
	if((ss >> id).fail())
		return 2;
	string descr = argv[descr_idx];

	featureTypes[id] = descr;

	return 0;
}
*/

int FeatureCoordTypes::getIDByCode(std::string code)
{
	std::map<int,std::string>::iterator it;
	for (it = featureCodes.begin(); it != featureCodes.end(); ++it)
	{
		if (it->second == code)
			return it->first;
	}
	return -1;
}

void FeatureCoordTypes::debugPrint()
{
	std::cout << "FeatureCoordTypes: " << std::endl;
	std::map<int,std::string>::iterator it1;
	std::map<int,std::string>::iterator it2;
	for (it1 = featureTypes.begin(), it2 = featureCodes.begin() ; (it1 != featureTypes.end()) && (it2 != featureCodes.end()); ++it1, ++it2)
		std::cout << "  id " << it1->first << ": " << it1->second << ", " << it2->second << std::endl;
}

std::string FeatureCoordTypes::getCode(int featureID) const
{
	std::map<int,std::string>::const_iterator it = featureCodes.find(featureID);
	if (it != featureCodes.end())
		return it->second;
	else
		return "";
}

std::string FeatureCoordTypes::getDescr(int featureID) const
{
	std::map<int,std::string>::const_iterator it = featureTypes.find(featureID);
	if (it != featureTypes.end())
		return it->second;
	else
		return "";
}
