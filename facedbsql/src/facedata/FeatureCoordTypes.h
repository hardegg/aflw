/*
 * FeatureCoordTypes.h
 *
 *  Created on: 27.07.2010
 *      Author: pwohlhart
 */

#ifndef FEATURECOORDTYPES_H_
#define FEATURECOORDTYPES_H_

#include <vector>
#include <map>

#include "../dbconn/SQLiteDBConnection.h"

class FeatureCoordTypes: public IDataCallbackObj {
public:
	FeatureCoordTypes();
	virtual ~FeatureCoordTypes();

	int getIDByDescr(std::string descr);
	int getIDByCode(std::string code);
	vector<int> getTypeIds();
	int getNumIDs() const { return featureTypes.size(); };

	std::string getCode(int featureID) const;
	std::string getDescr(int featureID) const;

	bool load(SQLiteDBConnection *sqliteConn);
	//int recvData(int argc, char **argv, char **azColName);
	int dataRowReady(SQLiteDBConnection *sqlConn);

	void debugPrint();

	static const string queryString;
private:
	std::map<int,std::string> featureTypes;
	std::map<int,std::string> featureCodes;
};


#endif /* FEATURECOORDTYPES_H_ */
