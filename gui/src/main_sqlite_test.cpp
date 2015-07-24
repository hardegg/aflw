/*
 * main.cpp
 *
 *  Created on: 02.08.2010
 *      Author: koestinger
 */

#include "dbconn/SQLiteDBConnection.h"
#include "facedata/FeatureCoordTypes.h"

#include "boost/program_options.hpp"
#include "facedbtool/DuplicateFaceFinder.h"

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
		("action", po::value< string >(), "import | finddup")
		("sqldb-file", po::value< string >()->default_value("C:\\projects\\facedbsql\\db\\facedb-data.db"), "sqlite database file")
		("imgdbname", po::value< string >()->default_value("flickr"), "image db name (default 'flickr')")
	;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
	po::notify(vm);

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

	std::string dbFile = vm["sqldb-file"].as<string>();

	SQLiteDBConnection sqlConn;
	if (!sqlConn.open(dbFile))
	{
		cout << "ERROR: couldn't open database '" << dbFile << "'" << endl;
		return 1;
	}

	if (action == "import")
	{
		cout << "importing" << endl;
	}
	else if (action == "finddup")
	{
		cout << "finding dups" << endl;
		DuplicateFaceFinder dupFinder;
		dupFinder.run(vm,&sqlConn);
	}
	else
		cout << "Unknown action '"<< action << "'" << endl;

	sqlConn.close();
}
