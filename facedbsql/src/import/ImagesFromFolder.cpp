/*
 * ImagesFromFolder.h
 *
 *  Created on: 19.04.2012
 *      Author: szweimueller
 */

#include "ImagesFromFolder.h"

#include <boost/lexical_cast.hpp>

#include <string>
#include <iostream>

ImagesFromFolder::ImagesFromFolder() 
{
	m_paths.clear();
}

ImagesFromFolder::~ImagesFromFolder()
{
}

PathVector ImagesFromFolder::getImagesFromFolder(const fs::path& folderPath, const std::string format)
{
    std::cout << "ImagesFromFolder::getImagesFromFolder -> Info: FolderPath '" << folderPath << "' StringFormat: '" 
              << format << "'" << std::endl;

    std::set<std::string> targetExtensions;
    targetExtensions.insert(format); //targetExtensions.insert(".PGM");

    // test if folderPath is valid
    int tmp = boost::filesystem::exists(folderPath);

    PathVector paths;
    paths.clear();
    
    fs::directory_iterator end;
        
    if (tmp) {
        fs::directory_iterator iter(folderPath);
        for (; iter != end; ++iter) 
        {
            if (!fs::is_regular_file(iter->status())) { 
                continue; 
            }

            std::string extension = iter->path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
            if (targetExtensions.find(extension) == targetExtensions.end()) {
                continue;
            }
            paths.push_back(iter->path());
        }
    }
    return paths;
}

PathVector ImagesFromFolder::getFilesFromFolderStruct(const fs::path& folderPath, const std::string format)
{
    std::cout << "ImagesFromFolder::getFilesFromFolderStruct -> Info: FolderPath '" << folderPath << "' StringFormat: '" 
              << format << "'" << std::endl;

    std::set<std::string> targetExtensions;
    targetExtensions.insert(format); //targetExtensions.insert(".PGM");

    fs::directory_iterator end;
    
	// test if folderPath is valid 
	if (boost::filesystem::exists(folderPath)) {
		fs::directory_iterator iter(folderPath);

		for (; iter != end; ++iter) 
		{
			if (fs::is_directory(iter->status())) {
				std::string subFolder = iter->path().string(); 
				getFilesFromFolderStruct(subFolder, format);
			}
			
			if (!fs::is_regular_file(iter->status())) { 
				continue; 
			} 

			std::string extension = iter->path().extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
			if (targetExtensions.find(extension) == targetExtensions.end()) {
				continue;
			}
			m_paths.push_back(iter->path());
		}
	}

    return m_paths;
}

void ImagesFromFolder::clearPathVector()
{
	std::cout << "ImagesFromFolder::clearPathVector" << std::endl;
	m_paths.clear();
}

bool ImagesFromFolder::printFoundFiles(PathVector paths)
{
    std::cout << "===============================" << std::endl;   
    std::cout << "                            " << std::endl;
    std::cout << "     Found '" << paths.size() << "' Files      |" << std::endl;
    std::cout << "                             " << std::endl;
    std::cout << "===============================" << std::endl; 

    return true;
}

