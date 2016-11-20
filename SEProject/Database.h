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
	bool operator==(const Staff& a);
};

class DataFrame {
public:
	DataFrame();
	DataFrame(MYSQL_RES* result);
	bool isEmpty();
	int  getWidth();
	int getHeight();
	string getItem(int, int);

private:
	vector< vector<string> > data;
};

class DataServer {
public:
	DataServer();
	~DataServer();
	bool login(string userID, string password);
	bool getIsLogined();
	bool getIsConnected();
	
	bool addStaff(string staffID, 
		string staffPassword=defaultStaffPassword,
		string staffName=defaultStaffName,
		string staffAuthority=defaultStaffAuthority);
	bool deleteStaff(string staffID);
	vector<Staff> getAllStaff();
	vector<Staff> getAllStaffExceptCurrentUser();

	static const string defaultStaffPassword;
	static const string defaultStaffName;
	static const string defaultStaffAuthority;

private:
	MYSQL server;
	bool isLogined;
	bool isConnected;
	Staff currentUser;

	string formatQuery(string format, ...);
	DataFrame makeQuery(string query);
	Staff getStaffFromID(string staffID);
};

