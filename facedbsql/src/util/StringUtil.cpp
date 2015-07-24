/*
 * StringUtil.cpp
 *
 *  Created on: 29.07.2010
 *      Author: pwohlhart
 */

#include "StringUtil.h"

std::string StringUtil::ltrim(std::string input)
{
	std::string whitespaces (" \t\f\v\n\r");
	std::string::size_type p = input.find_first_not_of(whitespaces);
	if (p > 0)
	{
		if (p != std::string::npos)
			input = input.substr(p);
		else
			input.clear(); // input is all whitespace
	}

	return input;
}

std::string StringUtil::rtrim(std::string input)
{
	std::string whitespaces (" \t\f\v\n\r");
	std::string::size_type p = input.find_last_not_of(whitespaces);
	if (p != std::string::npos)
		input.erase(p+1);
	else
		input.clear();            // input is all whitespace

	return input;
}

std::string StringUtil::trim(std::string input)
{
	return StringUtil::ltrim(StringUtil::rtrim(input));
}

