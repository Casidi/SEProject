#include "Database.h"
#include <cstdio>
#include <iostream>
#include <Windows.h>

using namespace std;

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

bool DataServer::login(string userName, string password)
{
	char query_buffer[128];
	sprintf_s(query_buffer, 128, "SELECT * FROM staff WHERE name='%s' AND password='%s';",
		userName.c_str(), password.c_str());

	DataFrame df = makeQuery(query_buffer);

	if (df.isEmpty()) {
		cout << "failed to login" << endl;
		isLogined = false;
		return false;
	}

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

string DataFrame::getItem(int x, int y)
{
	return data[x][y];
}
