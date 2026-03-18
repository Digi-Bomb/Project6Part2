#pragma once

#include <string>

//manual implementation of ClientID stuff
//class ClientID {
//public:
//	static std::string generateClientID();
//
//private:
//	//string for MAC
//	static std::string getMacAddress();
//
//	//unisgned long is non-negative, can contain PID size from Windows and other OS
//	static unsigned long getPIDValue(); 
//	
//	//unsigned long long can contain large timestamp values, ensuring uniqueness even in high-frequency client connections
//	static unsigned long long getTimestampValue();
//
//	//without hash, ID is messy and long, so we hash it to make it shorter and more manageable
//	//this will make client IDs unreadable, we may want debugmode without hash for readability
//	static std::string hashToHex(const std::string& input);
//};

//#include <chrono>
//#include <sstream>
//#include <iomanip>
//#include <string>
//
////windows only dev for now, can add IFNDEF for other OS 
//#include <iphlpapi.h>
//#include <winsock2.h>
//#include <Windows.h>
//
//#pragma comment(lib, "iphlpapi.lib")
//
////generate complete client ID by combining MAC address, PID, and timestamp
//std::string ClientID::generateClientID() {
//	std::string macAddress = getMacAddress();
//	unsigned long pidValue = getPIDValue();
//	unsigned long long timestampValue = getTimestampValue();
//	std::stringstream ss;
//	ss << macAddress << "-" << pidValue << "-" << timestampValue;
//	return hashToHex(ss.str());
//}

