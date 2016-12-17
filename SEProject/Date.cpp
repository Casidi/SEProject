#include "Date.h"
#include <sstream>

using namespace std;

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
	month = m - 1;
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