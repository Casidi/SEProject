#pragma once
#include "mysql.h"
#include <string>
#include <vector>

using namespace std;

class DataFrame {
public:
	DataFrame();
	DataFrame(MYSQL_RES* result);
	inline bool isEmpty();
	inline int  getWidth();
	inline int getHeight();
	inline string getItem(int x, int y);

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

private:
	MYSQL server;
	bool isLogined;
	bool isConnected;

	DataFrame makeQuery(string query);
};

