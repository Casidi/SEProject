#include "Database.h"
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <Windows.h>

using namespace std;

const string DataServer::defaultStaffPassword = "00000000";
const string DataServer::defaultStaffName = "username";
const string DataServer::defaultStaffAuthority = "labor";

DataServer::DataServer()
{
	mysql_init(&server);
	if (!mysql_real_connect(&server, "localhost", "root", 
		"seproject12345678", NULL, 0, NULL, 0)) {
		cout << "failed to connect to server\n";
		isConnected = false;
		isLogined = false;
		return;
	} else {
		cout << "Connection succeeded" << endl;
		makeQuery("USE se_project;");

		isConnected = true;
		isLogined = false;
	}	
}

DataServer::~DataServer()
{
	mysql_close(&server);
}

void DataServer::resetDatabase()
{
	makeQuery("DELETE FROM staff;");
	makeQuery("DELETE FROM schedule;");

	addStaff("A001", defaultStaffPassword, defaultStaffName, "chief");
	addStaff("A002", defaultStaffPassword, defaultStaffName, "supervisor");
	addStaff("A003", defaultStaffPassword, defaultStaffName, "supervisor");
	for (int i = 4; i <= 23; ++i) {
		char buffer[32];
		sprintf_s(buffer, "A%03d", i);
		addStaff(buffer);
	}
}

bool DataServer::login(string userID, string password)
{
	string query = formatQuery("SELECT * FROM staff WHERE id='%s' AND password='%s';",
		userID.c_str(), password.c_str());

	DataFrame df = makeQuery(query);

	if (df.isEmpty()) {
		cout << "failed to login" << endl;
		isLogined = false;
		return false;
	}

	currentUser = getStaffFromID(userID);
	isLogined = true;
	return true;
}

bool DataServer::getIsLogined()
{
	return isLogined;
}

bool DataServer::getIsConnected()
{
	return isConnected;
}

bool DataServer::addSchedule(string date, string staffID, string status, string reason, string isApproved)
{
	string query = formatQuery("INSERT INTO schedule VALUES('%s', '%s', '%s', '%s', '%s');",
		date.c_str(), staffID.c_str(), status.c_str(), reason.c_str(), isApproved.c_str());
	makeQuery(query);
	return true;
}

string DataServer::formatQuery(string format, ...)
{
	char queryBuffer[256];
	strcpy_s(queryBuffer, format.c_str());

	va_list vl;
	va_start(vl, format);
	vsprintf_s(queryBuffer, format.c_str(), vl);
	va_end(vl);

	return string(queryBuffer);
}

bool DataServer::addStaff(string staffID,
	string staffPassword,
	string staffName,
	string staffAuthority) {
	if (staffID.empty())
		return false;

	string query = formatQuery("SELECT * FROM staff WHERE id = '%s';", staffID.c_str());
	
	DataFrame result = makeQuery(query);
	if (result.isEmpty()) {
		query = formatQuery(
			"INSERT INTO staff VALUES('%s', '%s', '%s', '%s');",
			staffID.c_str(),
			staffPassword.c_str(),
			staffName.c_str(),
			staffAuthority.c_str()
			);
		makeQuery(query);

		//add initial schedule
		if (staffAuthority == "labor") {
			if (getNumberOfLabor() % 2 == 1) {
				addSchedule("2016/11/13", staffID, "early", "NULL", "Y");
			}
			else {
				addSchedule("2016/11/13", staffID, "late", "NULL", "Y");
			}
		}
		else if (staffAuthority == "supervisor") {
			if (getNumberOfSupervisor() % 2 == 1) {
				addSchedule("2016/11/13", staffID, "early", "NULL", "Y");
			}
			else {
				addSchedule("2016/11/13", staffID, "late", "NULL", "Y");
			}
		}
		else if (staffAuthority == "chief") {
			addSchedule("2016/11/13", staffID, "early", "NULL", "Y");
		}

		return true;
	}
	else {
		cout << "staff already exists" << endl;
		return false;
	}
}

bool DataServer::deleteStaff(string staffID) {
	string query = formatQuery("DELETE FROM staff WHERE id='%s';", staffID.c_str());
	makeQuery(query);
	return true;
}

bool DataServer::setStaffAuthority(string staffID, string staffAuthority)
{
	if (staffID.empty() || staffAuthority.empty())
		return false;

	string query = formatQuery(
		"UPDATE staff SET authority = '%s' WHERE id = '%s';",
		staffAuthority.c_str(),
		staffID.c_str()
		);
	makeQuery(query);
	return true;
}

bool DataServer::setCurrentUserPassword(string staffPassword)
{
	if (staffPassword.empty())
		return false;

	currentUser.password = staffPassword;

	string query = formatQuery(
		"UPDATE staff SET password = '%s' WHERE id = '%s';",
		currentUser.password.c_str(),
		currentUser.id.c_str()
		);
	makeQuery(query);

	return true;
}

bool DataServer::setCurrentUserName(string staffName)
{
	if (staffName.empty())
		return false;

	currentUser.name = staffName;

	string query = formatQuery(
		"UPDATE staff SET name = '%s' WHERE id = '%s';",
		currentUser.name.c_str(),
		currentUser.id.c_str()
		);
	makeQuery(query);

	return true;
}

vector<Staff> DataServer::getAllStaff()
{
	DataFrame dataFromQuery = makeQuery("SELECT * FROM staff ORDER BY id DESC;");
	vector<Staff> result;

	Staff buffer;
	for (int i = 0; i < dataFromQuery.getHeight(); ++i) {
		buffer.id = dataFromQuery.getItem(i, 0);
		buffer.password = dataFromQuery.getItem(i, 1);
		buffer.name = dataFromQuery.getItem(i, 2);
		buffer.authority = dataFromQuery.getItem(i, 3);
		result.push_back(buffer);
	}
	return result;
}

vector<Staff> DataServer::getAllStaffExceptCurrentUser()
{
	vector<Staff> result = getAllStaff();
	vector<Staff>::iterator iter = find(result.begin(), result.end(), currentUser);
	if (iter != result.end()) {
		result.erase(iter);
	}
	return result;
}

int DataServer::getNumberOfLabor()
{
	DataFrame result = makeQuery("SELECT COUNT(*) FROM staff WHERE authority='labor';");
	return atoi(result.getItem(0, 0).c_str());
}

int DataServer::getNumberOfSupervisor()
{
	DataFrame result = makeQuery("SELECT COUNT(*) FROM staff WHERE authority='supervisor';");
	return atoi(result.getItem(0, 0).c_str());
}

string DataServer::getStaffPositionFromID(string staffID)
{
	string query = formatQuery("SELECT authority FROM staff WHERE id='%s';", staffID.c_str());
	DataFrame result = makeQuery(query);
	return result.getItem(0, 0);
}

vector<Schedule> DataServer::getScheduleBase()
{
	vector<Schedule> result;
	DataFrame queryResult = makeQuery("SELECT * FROM schedule \
		WHERE date='2016/11/13' AND (status='early' OR status='late') \
		ORDER BY staff_id DESC;");
	for (int i = 0; i < queryResult.getHeight(); ++i) {
		Schedule temp;
		temp.date = queryResult.getItem(i, 0);
		temp.staffID = queryResult.getItem(i, 1);
		temp.status = queryResult.getItem(i, 2);
		temp.reason = queryResult.getItem(i, 3);
		temp.isApproved = queryResult.getItem(i, 4);
		result.push_back(temp);
	}
	return result;
}

vector<Schedule> DataServer::getDaySchedule(Date target)
{
	vector<Schedule> base = getScheduleBase();
	Date firstDayInWeek = target;
	firstDayInWeek.subDays(firstDayInWeek.getWeekDay());

	int diff = firstDayInWeek.diffDays(Date(2016, 11, 13));
	diff /= 7;
	if(diff%2 == 1) {
		for (int i = 0; i < base.size(); ++i) {
			if (getStaffPositionFromID(base[i].staffID) != "chief") {
				if (base[i].status == "early")
					base[i].status = "late";
				else if (base[i].status == "late")
					base[i].status = "early";
			}
		}
	}

	//TODO: for each staff, check if there are applications for sick, leave, or ....
	return base;
}

Staff DataServer::getStaffFromID(string staffID)
{
	string query = formatQuery("SELECT * FROM staff WHERE id='%s';", staffID.c_str());
	DataFrame queryResult = makeQuery(query);
	Staff result;

	result.id = queryResult.getItem(0, 0);
	result.password = queryResult.getItem(0, 1);
	result.name = queryResult.getItem(0, 2);
	result.authority = queryResult.getItem(0, 3);
	return result;
}

DataFrame DataServer::makeQuery(string query)
{
	int err = mysql_query(&server, query.c_str());
	if (err != 0) {
		cout << "Query failed: " << mysql_error(&server) << endl;
		return DataFrame();
	}

	MYSQL_RES *result = mysql_store_result(&server);
	if (result == NULL) {
		//cout << "Query returns empty result\n";
		//cout << "\t=>" << query << endl;
		return DataFrame();
	}

	return DataFrame(result);
}

DataFrame::DataFrame()
{
}

DataFrame::DataFrame(MYSQL_RES* result)
{
	int nRows = mysql_num_rows(result);
	int nColumns = mysql_num_fields(result);

	data.resize(nRows);
	for (size_t i = 0; i < data.size(); ++i) {
		data[i].resize(nColumns);
	}

	for (int i = 0; i < nRows; ++i) {
		MYSQL_ROW row = mysql_fetch_row(result);
		for (int j = 0; j < nColumns; ++j) {
			data[i][j] = row[j]? row[j]: "NULL";
		}
	}
}

bool DataFrame::isEmpty()
{
	return data.empty();
}

int DataFrame::getWidth()
{
	return (data.size() == 0)? 0: data[0].size();
}

int DataFrame::getHeight()
{
	return data.size();
}

string DataFrame::getItem(int rowIndex, int columnIndex)
{
	if (rowIndex > getHeight()-1 || columnIndex > getWidth()-1 || isEmpty() == true)
		return string("");
	return data[rowIndex][columnIndex];
}

bool Staff::operator==(const Staff & a)
{
	return (id == a.id) && (password == a.password) && (name == a.name) && (authority == a.authority);
}

vector<string> Date::getAllDatesInThisWeekAsStrings() {
	Date temp = getFirstDateThisWeek();

	vector<string> result;
	for (int i = 0; i < 7; ++i) {
		result.push_back(temp.toString());
		temp.addDays(1);
	}

	return result;
}

void Date::addDays(int nDays) {
	while (day + nDays > getNumDaysOfMonth(month)) {
		nDays -= (getNumDaysOfMonth(month) - day);
		day = 0;
		month = (month + 1) % 12;
		year = (month == 0) ? (year + 1) : year;
	}
	day += nDays;
}

void Date::subDays(int nDays) {
	if (nDays > day) {
		nDays -= day;
		day = 0;
		month = (month + 12 - 1) % 12;
		year = (month == 11) ? year - 1 : year;

		while (nDays > getNumDaysOfMonth(month)) {
			nDays -= getNumDaysOfMonth(month);
			month = (month + 12 - 1) % 12;
			year = (month == 11) ? year - 1 : year;
		}

		day = getNumDaysOfMonth(month) - nDays;
	}
	else if (nDays < day) {
		day -= nDays;
	}
	else {
		month = (month + 12 - 1) % 12;
		year = (month == 11) ? year - 1 : year;
		day = getNumDaysOfMonth(month);
	}
}

int Date::diffDays(Date target) {
	int result = 0;
	if (year > target.year) {
		while (year != target.year) {
			target.addDays(365);
			result += 365;
		}

		if (month > target.month) {
			while (month != target.month) {
				target.addDays(27);
				result += 27;
			}

			result += (day - target.day);
		}
		else if (month < target.month) {
			while (month != target.month) {
				target.subDays(27);
				result -= 27;
			}

			result += (day - target.day);
		}
		else {
			result += (day - target.day);
		}
	}
	else if (year < target.year) {
		while (year != target.year) {
			target.subDays(365);
			result -= 365;
		}

		if (month > target.month) {
			while (month != target.month) {
				target.addDays(27);
				result += 27;
			}

			result += (day - target.day);
		}
		else if (month < target.month) {
			while (month != target.month) {
				target.subDays(27);
				result -= 27;
			}

			result += (day - target.day);
		}
		else {
			result += (day - target.day);
		}
	}
	else {
		if (month > target.month) {
			while (month != target.month) {
				target.addDays(27);
				result += 27;
			}

			result += (day - target.day);
		}
		else if (month < target.month) {
			while (month != target.month) {
				target.subDays(27);
				result -= 27;
			}

			result += (day - target.day);
		}
		else {
			result += (day - target.day);
		}
	}

	return result;
}

Date::Date(string s)
{
	for (int i = 0; i < s.length(); ++i)
		if (s[i] == '/')
			s[i] = ' ';
	stringstream ss(s);
	int y, m, d;
	ss >> y >> m >> d;
	year = y;
	month = m-1;
	day = d;
}

int Date::getWeekDay() {
	Date origin(2016, 11, 20);
	int diff = this->diffDays(origin);
	if (diff < 0)
		diff += (abs(diff) / 7 + 1) * 7;
	return diff % 7;
}

string Date::toString() {
	stringstream ss;
	ss << year << "/"
		<< month + 1 << "/"
		<< day;
	return ss.str();
}

Date Date::getFirstDateThisWeek()
{
	Date temp = *this;
	temp.subDays(temp.getWeekDay());
	return temp;
}

int Date::getNumDaysOfMonth(int target_month) {
	const int numDaysOfMonths[12] = { 31, 28, 31, 30, 31, 30, 31,31,30,31,30,31 };
	const int numDaysOfMonthsLeap[12] = { 31, 29, 31, 30, 31, 30, 31,31,30,31,30,31 };
	return (year % 4 == 0) ? numDaysOfMonthsLeap[target_month] : numDaysOfMonths[target_month];
}