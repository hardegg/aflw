/*
 * DBCreator.cpp
 *
 *  Created on: 31.08.2010
 *      Author: pwohlhart
 */

#include "DBCreator.h"

#include <iostream>
#include <fstream>

//#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "boost/filesystem.hpp"

#include "../util/StringUtil.h"

namespace bfs = boost::filesystem;

DBCreator::DBCreator() {

}

DBCreator::~DBCreator() {
}


void DBCreator::run(po::variables_map &vm)
{
	std::string dbFile = vm["sqldb-file"].as<string>();

	// check if file exists
	if (bfs::exists(dbFile))
	{
		std::cout << "File '" << dbFile << "' already exists. Exiting." << std::endl;
		return;
	}

	std::string sqlSchemaFileName = vm["schema-file"].as<string>();
	if (!bfs::exists(sqlSchemaFileName))
	{
		std::cout << "Cannot find sql schema file: '" << sqlSchemaFileName << "'" << std::endl;
		return;
	}

	std::string featCoordTypesFileName = vm["ftctype-file"].as<string>();
	if (!bfs::exists(featCoordTypesFileName))
	{
		std::cout << "Cannot find feature coord types sql file: '" << featCoordTypesFileName << "'" << std::endl;
		return;
	}

	std::ifstream schemaFile(sqlSchemaFileName.c_str());
	if (schemaFile.is_open())
	{
		SQLiteDBConnection sqlConn;
		if (sqlConn.open(dbFile))
		{
			std::string command;
			std::string line;

			while (! schemaFile.eof() )
			{
				command = "";

				while (! schemaFile.eof() )
				{
					std::getline (schemaFile,line);
					line = StringUtil::ltrim(line);

					if (line.size() > 0)
					{
						command += line + "\n";
						if (line[line.size()-1] == ';')
							break;
					}
				}

				if (command.size() > 0)
				{
					std::cout << "----- cmd -------- " << std::endl << command << std::endl;
					sqlConn.exec(command);
				}
			}
			schemaFile.close();

			std::ifstream featCoordTypesFile(featCoordTypesFileName.c_str());
			if (featCoordTypesFile.is_open())
			{
				while (! featCoordTypesFile.eof() )
				{
					command = "";

					while (! featCoordTypesFile.eof() )
					{
						std::getline (featCoordTypesFile,line);
						line = StringUtil::ltrim(line);

						if (line.size() > 0)
						{
							command += line + "\n";
							if (line[line.size()-1] == ';')
								break;
						}
					}

					if (command.size() > 0)
					{
						std::cout << "FCType: " << command;
						sqlConn.exec(command);
					}
				}
				featCoordTypesFile.close();
			}

			sqlConn.close();
		}
		else
		{
			std::cout << "Cannot open sql database file '" << dbFile << "'" << std::endl;
		}
	}
	else
	{
		std::cout << "Cannot open sql schema file'" << sqlSchemaFileName << "'" << std::endl;
	}

}
