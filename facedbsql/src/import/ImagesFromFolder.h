/*
 * ImagesFromFolder.h
 *
 *  Created on: 19.04.2012
 *      Author: szweimueller
 */

#ifndef IMAGESFROMFOLDER_H_
#define IMAGESFROMFOLDER_H_

#define BOOST_FILESYSTEM_VERSION 3

#include <set>
#include <string>
#include <vector>

#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

typedef std::vector<fs::path> PathVector;

class ImagesFromFolder {
public:
	ImagesFromFolder();
	virtual ~ImagesFromFolder();
    
    PathVector getImagesFromFolder(const fs::path& folderPath, const std::string format);
	PathVector getFilesFromFolderStruct(const fs::path& folderPath, const std::string format);

	void clearPathVector();
	
    bool printFoundFiles(PathVector paths);

private:
	PathVector m_paths;
};

#endif /* IMAGESFROMFOLDER_H_ */
