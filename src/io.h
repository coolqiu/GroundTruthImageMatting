#pragma once
#include <string>
#include <sstream>

void Error(const std::string& msg);
void Warning(const std::string& msg);
void Inform(const std::string& msg);

template <class T>
std::string ToString(const T t)
{
	std::ostringstream ss;
	ss << t;
	return ss.str();
}

template <class T>
std::string ToHexString(const T t)
{
	std::ostringstream ss;
	ss << std::hex << t;
	return ss.str();
}

std::string appendNameToPath(const std::string& name, const std::string& path);

std::string EdsCodeToString(long code);