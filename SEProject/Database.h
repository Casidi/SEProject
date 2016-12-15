#pragma once
#include "mysql.h"
#include <string>
#include <vector>

using namespace std;

class Date {
public:
	Date(int y, int m, int d) :year(y), month(m - 1), day(d) {}
	Date(string s);
	int getYear() { return year; }
	int getMonth() { return month + 1; }
	int getDay() { return day; }
	int getWeekDay();

	string toString();
	Date getFirstDateThisWeek();
	vector<string> getAllDatesInThisWeekAsStrings();

	void addDays(int nDays);
	void subDays(int nDays);
	int diffDays(Date target);
private:
	int getNumDaysOfMonth(int target_month);

	//NOTE: month starts from zero
	int year, month, day;
};

class Staff {
public:
	string id;
	string password;
	string name;
	string authority;
	bool operator==(const Staff& a);
};

class Schedule {
public:
	string date;
	string staffID;
	string status;
	string reason;
	string isApproved;
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

	//reset all record in the database
	void resetDatabase();

	string getServerIPFromFile(string fileName);

	bool login(string userID, string password);
	bool getIsLogined();
	bool getIsConnected();

	bool addSchedule(string date, string staffID, string status, string reason, string isApproved);
	
	bool addStaff(string staffID, 
		string staffPassword=defaultStaffPassword,
		string staffName=defaultStaffName,
		string staffAuthority=defaultStaffAuthority);
	bool deleteStaff(string staffID);
	bool setStaffAuthority(string staffID, string staffAuthority);
	bool setCurrentUserPassword(string staffPassword);
	bool setCurrentUserName(string staffName);
	Staff getCurrentUser();

	vector<Staff> getAllStaff();
	vector<Staff> getAllStaffExceptCurrentUser();
	int getNumberOfLabor();
	int getNumberOfSupervisor();
	string getStaffPositionFromID(string staffID);

	vector<Schedule> getScheduleBase();
	vector<Schedule> getDaySchedule(Date target);

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
	void modifyScheduleBase(vector<Schedule>& base, string targetStatus);
};


