#pragma once
#include "mysql.h"
#include <string>
#include <vector>

using namespace std;

class Staff {
public:
	string id;
	string password;
	string name;
	string authority;
};

class DataFrame {
public:
	DataFrame();
	DataFrame(MYSQL_RES* result);
	bool isEmpty();
	int  getWidth();
	int getHeight();
	string getItem(int x, int y);

private:
	vector< vector<string> > data;
};

class DataServer {
public:
	DataServer();
	~DataServer();
	bool login(string userName, string password);
	bool getIsLogined();
	bool getIsConnected();
	string formatQuery(string format, ...);
	DataFrame makeQuery(string query);

	bool addStaff(string staffID, 
		string staffPassword=defaultStaffPassword,
		string staffName=defaultStaffName,
		string staffAuthority=defaultStaffAuthority);
	bool deleteStaff(string staffID);
	bool setStaffAuthority(string staffID, string staffAuthority);
	vector<Staff> getAllStaff();

private:
	MYSQL server;
	bool isLogined;
	bool isConnected;

	static const string defaultStaffPassword;
	static const string defaultStaffName;
	static const string defaultStaffAuthority;
};

