#pragma once
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
