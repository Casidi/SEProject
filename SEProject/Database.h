#pragma once
#include "mysql.h"
#include "Date.h"
#include <string>
#include <vector>

using namespace std;

class Leave {
public:
	string date;
	string staffID;
	string name;
	string status;
	string reason;
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
	bool DataServer::checkLeave(string status, string reason, string date, string id);

	bool addStaff(string staffID,
		string staffPassword = defaultStaffPassword,
		string staffName = defaultStaffName,
		string staffAuthority = defaultStaffAuthority);
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

	bool applyLeave(string status, string reason, Date seldate, Date today);
	bool approveLeave(string date, string staffID, string status, string reason);
	vector<Leave> getAllLeave();
	bool setLeave(string id, Date date, string reason);

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

