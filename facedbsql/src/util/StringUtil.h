/*
 * StringUtil.h
 *
 *  Created on: 29.07.2010
 *      Author: pwohlhart
 */

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

#include <string>

class StringUtil {
public:
	static std::string ltrim(std::string input);
	static std::string rtrim(std::string input);
	static std::string trim(std::string input);
};

#endif /* STRINGUTIL_H_ */
