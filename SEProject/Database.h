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
	bool setStaffAuthority(string staffID, string staffAuthority);
	bool setCurrentUserPassword(string staffPassword);
	bool setCurrentUserName(string staffName);
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

class Date {
public:
	Date(int y, int m, int d) :year(y), month(m - 1), day(d) {}
	int getYear() { return year; }
	int getMonth() { return month + 1; }
	int getDay() { return day; }
	int getWeekDay();

	string toString();
	vector<string> getAllDatesInThisWeekAsStrings();

	void addDays(int nDays);
	void subDays(int nDays);
	int diffDays(Date target);
private:
	int getNumDaysOfMonth(int target_month);

	//NOTE: month starts from zero
	int year, month, day;
};
