/*
 * AnnotationTypes.h
 *
 *  Created on: 15.01.2013
 *      Author: pwohlhart
 */

#ifndef ANNOTATIONTYPES_H_
#define ANNOTATIONTYPES_H_

#include <map>
#include <string>
#include "../dbconn/SQLiteDBConnection.h"

using namespace std;

struct AnnotType
{
	int id;
	string code;
	string description;
};

class AnnotationTypes {
public:
	AnnotationTypes(SQLiteDBConnection *sqlConn);
	virtual ~AnnotationTypes();

	int getIDByCode(string code);
	//string getCodeByID(int id) const { return ((_typesIt = _types.find(id)) != _types.end()) ? (*_typesIt).second.code : ""; }
	string getCodeByID(int id) const {
		map<int,AnnotType>::const_iterator it;
		return ((it = _types.find(id)) != _types.end()) ? (*it).second.code : "";
	}

private:
	map<int,AnnotType> _types;
	map<int,AnnotType>::const_iterator _typesIt;
};

#endif /* ANNOTATIONTYPES_H_ */
