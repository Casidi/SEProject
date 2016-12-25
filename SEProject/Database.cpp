#include "Database.h"
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

using namespace std;

const string DataServer::defaultStaffPassword = "00000000";
const string DataServer::defaultStaffName = "username";
const string DataServer::defaultStaffAuthority = "labor";

DataServer::DataServer()
{
	string serverIP = getServerIPFromFile("config.txt");
	if (serverIP == "") {
		cout << "failed to read config.txt\n";
		isConnected = false;
		isLogined = false;
		return;
	}

	mysql_init(&server);
	if (!mysql_real_connect(&server, serverIP.c_str(), "root",
		"seproject12345678", NULL, 0, NULL, 0)) {
		cout << "failed to connect to server\n";
		isConnected = false;
		isLogined = false;
		return;
	}
	else {
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

bool DataServer::setLeave(string id, Date date, string reason)
{
	if (id.empty()) {
		return false;
	}

	if (reason.empty()) {
		cout << "error: set leave must have a reason." << endl;
		return false;
	}

	if ((checkLeave("business", reason, date.toString(), id)) == true) {
		return false;
	}
	addSchedule(date.toString(), id, "business", reason, "Y");
	return true;
}

string DataServer::getServerIPFromFile(string fileName)
{
	ifstream file(fileName.c_str(), ios::in);
	string result;
	file >> result;
	file.close();
	return result;
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

vector<Leave> DataServer::getAllLeave()
{
	DataFrame dataFromQuery;
	vector<Leave> result;
	if (currentUser.authority.compare("chief") == 0)
		dataFromQuery = makeQuery("SELECT date, a.id, name, status, reason FROM staff a, schedule b WHERE b.staff_id = a.id AND b.status != 'early' AND b.status != 'late' AND b.is_approved = 'N' ORDER BY date, a.id;");
	else if (currentUser.authority.compare("supervisor") == 0)
		dataFromQuery = makeQuery("SELECT date, a.id, name, status, reason FROM staff a, schedule b WHERE b.staff_id = a.id AND a.authority != 'chief' AND b.status != 'early' AND b.status != 'late' AND b.is_approved = 'N' ORDER BY date, a.id;");
	else {
		cout << "error: authority is error" << endl;
		return result;
	}

	Leave buffer;
	for (int i = 0; i < dataFromQuery.getHeight(); ++i) {
		buffer.date = dataFromQuery.getItem(i, 0);
		buffer.staffID = dataFromQuery.getItem(i, 1);
		buffer.name = dataFromQuery.getItem(i, 2);
		buffer.status = dataFromQuery.getItem(i, 3);
		buffer.reason = dataFromQuery.getItem(i, 4);
		result.push_back(buffer);
	}
	return result;
}

bool DataServer::approveLeave(string date, string staffID, string status, string reason)
{
	if (staffID.empty()) {
		return false;
	}

	if (status.empty()) {
		return false;
	}

	string query1 = formatQuery(
		"UPDATE schedule SET is_approved = 'Y' WHERE date = '%s' AND staff_id = '%s' AND status = '%s';",
		date.c_str(),
		staffID.c_str(),
		status.c_str()
		);
	makeQuery(query1);
	string query2 = formatQuery(
		"DELETE FROM schedule WHERE date = '%s' AND staff_id = '%s' AND status = '%s' AND reason != '%s';",
		date.c_str(),
		staffID.c_str(),
		status.c_str(),
		reason.c_str()
		);
	makeQuery(query2);
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

bool DataServer::checkLeave(string status, string reason, string date, string id) {
	string query = formatQuery(
		"SELECT * FROM schedule WHERE date = '%s' AND staff_id = '%s' AND status = '%s' AND reason = '%s';",
		date.c_str(),
		id.c_str(),
		status.c_str(),
		reason.c_str()
		);
	DataFrame result = makeQuery(query);
	if ((result.isEmpty()) == true) {
		cout << "empty" << endl;
		return false;
	}
	else return true;
}

bool DataServer::applyLeave(string status, string reason, Date seldate, Date today)
{
	if (status.compare("compensatory") == 0) {
		if (reason.empty()) {
			if ((checkLeave(status, "NULL", seldate.toString(), currentUser.id)) == true) {
				return false;
			}
			addSchedule(seldate.toString(), currentUser.id, "compensatory", "NULL", "N");
			cout << "apply compensatory without reason." << endl;
		}
		else {
			if ((checkLeave(status, reason, seldate.toString(), currentUser.id)) == true) {
				return false;
			}
			addSchedule(seldate.toString(), currentUser.id, "compensatory", reason, "N");
			cout << "apply compensatory with reason." << endl;
		}
		return true;
	}
	else if (status.compare("leave") == 0) {
		if (reason.empty()) {
			cout << "error: apply leave must have a reason." << endl;
			return false;
		}
		else {
			if ((checkLeave(status, reason, seldate.toString(), currentUser.id)) == true) {
				return false;
			}
			addSchedule(seldate.toString(), currentUser.id, "leave", reason, "N");
			cout << "apply leave with reason." << endl;
		}
		return true;
	}
	else if (status.compare("sick") == 0) {
		if (reason.empty()) {
			if ((checkLeave(status, "NULL", today.toString(), currentUser.id)) == true) {
				return false;
			}
			addSchedule(today.toString(), currentUser.id, "sick", "NULL", "Y");
			cout << "apply sick without reason." << endl;
		}
		else {
			if ((checkLeave(status, reason, today.toString(), currentUser.id)) == true) {
				return false;
			}
			addSchedule(today.toString(), currentUser.id, "sick", reason, "Y");
			cout << "apply sick with reason." << endl;
		}
		return true;
	}
	cout << "error: status error." << endl;
	return false;
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
	query = formatQuery("DELETE FROM schedule WHERE staff_id='%s';", staffID.c_str());
	makeQuery(query);
	return true;
}

bool DataServer::setStaffAuthority(string staffID, string staffAuthority)
{
	if (staffID.empty()) {
		return false;
	}

	if (staffAuthority.empty()) {
		return false;
	}

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
	if (staffPassword.empty()) {
		return false;
	}

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
	if (staffName.empty()) {
		return false;
	}

	currentUser.name = staffName;

	string query = formatQuery(
		"UPDATE staff SET name = '%s' WHERE id = '%s';",
		currentUser.name.c_str(),
		currentUser.id.c_str()
		);
	makeQuery(query);	
	return true;
}

Staff DataServer::getCurrentUser()
{
	return currentUser;
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
	for (int i = 0; i < base.size(); ++i)
		base[i].date = target.toString();

	Date firstDayInWeek = target;
	firstDayInWeek.subDays(firstDayInWeek.getWeekDay());

	int diff = firstDayInWeek.diffDays(Date(2016, 11, 13));
	diff /= 7;
	if (diff % 2 == 1) {
		for (int i = 0; i < base.size(); ++i) {
			if (getStaffPositionFromID(base[i].staffID) != "chief") {
				if (base[i].status == "early")
					base[i].status = "late";
				else if (base[i].status == "late")
					base[i].status = "early";
			}
		}
	}

	modifyScheduleBase(base, "leave");
	modifyScheduleBase(base, "compensatory");
	modifyScheduleBase(base, "business");
	modifyScheduleBase(base, "sick");

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

void DataServer::modifyScheduleBase(vector<Schedule>& base, string targetStatus)
{
	for (int i = 0; i < base.size(); ++i) {
		string query = formatQuery("SELECT * FROM schedule \
						WHERE staff_id='%s' AND date='%s' \
						AND is_approved='Y' AND status='%s';",
			base[i].staffID.c_str(),
			base[i].date.c_str(),
			targetStatus.c_str());
		DataFrame result = makeQuery(query);
		if (result.isEmpty() == false) {
			base[i].status = targetStatus;
			base[i].reason = result.getItem(0, 3);
		}
	}
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
			data[i][j] = row[j] ? row[j] : "NULL";
		}
	}
}

bool DataFrame::isEmpty()
{
	return data.empty();
}

int DataFrame::getWidth()
{
	return (data.size() == 0) ? 0 : data[0].size();
}

int DataFrame::getHeight()
{
	return data.size();
}

string DataFrame::getItem(int rowIndex, int columnIndex)
{
	if (rowIndex > getHeight() - 1 || columnIndex > getWidth() - 1 || isEmpty() == true)
		return string("");
	return data[rowIndex][columnIndex];
}

bool Staff::operator==(const Staff & a)
{
	return (id == a.id) && (password == a.password) && (name == a.name) && (authority == a.authority);
}