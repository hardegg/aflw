#ifndef __UTILS_H__
#define __UTILS_H__

#include <QString>
#include <QStringList>

#ifdef WIN32
	#include <windows.h>
#endif

namespace utils
{
	//----------------------------------------------------------------------------
	#ifdef WIN32
		bool ListDirectoryContents(QString dir, QStringList &fileList)
		{
			WIN32_FIND_DATA fdFile;
			HANDLE hFind = NULL;

			char sPath[2048];

			sprintf(sPath, "%s/*.*", qPrintable(dir)); //, qPrintable(filter));

			if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
			{
				printf("Path not found: [%s]\n", qPrintable(dir));
				return false;
			}

			do
			{
				//Find first file will always return "."
				//    and ".." as the first two directories.
				if(strcmp(fdFile.cFileName, ".") != 0
						&& strcmp(fdFile.cFileName, "..") != 0)
				{
					//Build up our file path using the passed in
					//  [sDir] and the file/foldername we just found:
					sprintf(sPath, "%s/%s", qPrintable(dir), fdFile.cFileName);

					//Is the entity a File or Folder?
					if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
					{
						//printf("Directory: %s\n", sPath);
						ListDirectoryContents(QString(sPath),fileList); //Recursion, I love it!
					}
					else{
						//printf("File: %s\n", sPath);
						fileList.append(QString(sPath));
					}
				}
			}
			while(FindNextFile(hFind, &fdFile)); //Find the next file.

			FindClose(hFind); //Always, Always, clean things up!

			return true;
		}
	#endif



}
#endif //UTILS
