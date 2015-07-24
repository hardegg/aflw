/*
 * main.cpp
 *
 *  Created on: 30.07.2010
 *      Author: pwohlhart
 */

#include "../dbconn/SQLiteDBConnection.h"
#include "../facedata/FeatureCoordTypes.h"

#include "boost/program_options.hpp"

//#include "../FeaturesFileReader.h"
//#include "PoseFileReader.h"
#include "DuplicateFaceFinder.h"
#include "DBConsistencyChecker.h"
#include "FaceFeaturesDisplay.h"
#include "FaceRectCalculator.h"
#include "FacePoseCalculator.h"
#include "CamPoseCalculator.h"
#include "FaceEllipsesCalculator.h"
#include "FaceRectViolaJonesCalculator.h"
#include "DBCreator.h"
#include "DBMerger.h"

namespace po = boost::program_options;


int main( int argc, char* argv[] )
{
	cout << "-------------------------" << endl;
	cout << "   FaceDB - Admin Tool "   << endl;
	cout << "-------------------------" << endl;

	//----------------- Program Options

	po::positional_options_description pod;
	pod.add("action",1);

	po::options_description desc("Options");
	desc.add_options()
		("help","print this help message")
		("action", po::value< string >(), "showfeat | calcrects | calcviolajonesrects | calcellipses | calcpose | calccampose | merge | consistency | finddup")
		("dup-action", po::value< string >()->default_value("report"), "action on duplicates: options are 'report' or 'delete'")
		("inconsistency-action", po::value< string >()->default_value("report"), "action on duplicates: options are 'report' or 'repair'")
		("sqldb-file", po::value< string >()->default_value("../../data/aflw.sqlite"), "sqlite database file")
		("sqldb-file-source", po::value< string >(), "source sqlite database file for merging")
		("imgdbname", po::value< string >()->default_value("flickr"), "image db name (default 'flickr')")
		("face-id", po::value<int>(), "face id")
		("image-id", po::value<int>(), "image id")
		("file-id", po::value<string>(), "image file id")
		("db-id", po::value<string>(), "image db id")
		("schema-file", po::value<string>()->default_value("../../data/aflw.sql"), "sql db schema file" )
		("ftctype-file", po::value<string>()->default_value("../../data/FeatureCoordTypes.sql"), "feature coord types sql file" )
		("report-file", po::value<string>(), "output file for all kinds of reports" )
		("report-format", po::value<string>(), "txt | sql" )
	;

	po::variables_map vm;
	try
	{
		po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
		po::notify(vm);
	}
	catch (exception & e)
	{
	     cerr << "Unable to parse command line: " << e.what() << endl;
	     return 1;
	}

	bool showHelp = false;

	string action;
	if (vm.count("action"))
		action = vm["action"].as<string>();
	else
		showHelp = true;

	if (vm.count("help") || showHelp)
	{
	    cout << desc << "\n";
	    return 1;
	}

	// ------------------ ACTION ----------------

	if (action == "createdb")
	{
		cout << "create DB" << endl;
		DBCreator dbCreator;
		dbCreator.run(vm);
		return 0;
	}

	std::string dbFile = vm["sqldb-file"].as<string>();

	SQLiteDBConnection sqlConn;
	if (!sqlConn.open(dbFile))
	{
		cout << "ERROR: couldn't open database '" << dbFile << "'" << endl;
		return 1;
	}

	if (action == "showfeat")
	{
		cout << "show features" << endl;
		FaceFeaturesDisplay faceFeatDisp;
		faceFeatDisp.run(vm,&sqlConn);
	}
	else if (action == "finddup")
	{
		cout << "finding dups" << endl;
		DuplicateFaceFinder dupFinder;
		dupFinder.run(vm,&sqlConn);
	}
	else if (action == "calcrects")
	{
		cout << "calculate face rectangles" << endl;
		FaceRectCalculator faceRectCalc;
		faceRectCalc.run(vm,&sqlConn);
	}
	else if (action == "calcpose")
	{
		cout << "calculate faces' pose" << endl;
		FacePoseCalculator facePoseCalc;
		facePoseCalc.run(vm,&sqlConn);
	}
	else if (action == "calccampose")
	{
		cout << "calculate cam poses" << endl;
		CamPoseCalculator camPoseCalc;
		camPoseCalc.run(vm,&sqlConn);
	}
	else if (action == "calcviolajonesrects")
	{
		cout << "calculate face rectangles" << endl;
		FaceRectViolaJonesCalculator faceRectVJCalc;
		faceRectVJCalc.run(vm,&sqlConn);
	}
	else if (action == "calcellipses")
	{
		cout << "calculate face ellipses" << endl;
		FaceEllipsesCalculator faceEllipsesCalc;
		faceEllipsesCalc.run(vm,&sqlConn);
	}
	else if (action == "merge")
	{
		cout << "merge sql db files" << endl;
		DBMerger dbMerger;
		dbMerger.run(vm,&sqlConn);
	}
	else if (action == "consistency")
	{
		DBConsistencyChecker dbCheck;
		dbCheck.run(vm,&sqlConn);
	}
	else
		cout << "Unknown action '"<< action << "'" << endl;

	sqlConn.close();
}
