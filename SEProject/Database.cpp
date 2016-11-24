#include "Database.h"
#include <cstdio>
#include <cstdarg>
#include <iostream>
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

string DataServer::formatQuery(string format, ...)
{
	char queryBuffer[128];
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

//以下修改過
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

bool DataServer::setStaffPassword(string staffID, string staffPassword)
{
	if (staffID.empty() || staffPassword.empty())
		return false;

	string query = formatQuery(
		"UPDATE staff SET password = '%s' WHERE id = '%s';",
		staffPassword.c_str(),
		staffID.c_str()
		);
	makeQuery(query);
	return true;
}

bool DataServer::setStaffName(string staffID, string staffName)
{
	if (staffID.empty() || staffName.empty())
		return false;

	string query = formatQuery(
		"UPDATE staff SET name = '%s' WHERE id = '%s';",
		staffName.c_str(),
		staffID.c_str()
		);
	makeQuery(query);
	return true;
}

string DataServer::getcurrentUserID() {
	return currentUser.id;
}
//到這為止

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
		cout << "Query returns empty result\n";
		cout << "\t=>" << query << endl;
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
