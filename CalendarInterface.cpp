/*
 * Author: Ethan Zheng
 * Purpose: Implementation of an interactive UI in command line that accepts user input and rejects illegal input
 */


#include "stdafx.h"
#include "CalendarInterface.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>

using namespace std;

CalendarInterface::CalendarInterface(std::string builderType, size_t years) : builder(nullptr),cal(nullptr),currentDisplay(nullptr) {
	if (builderType == "full") {
		builder = make_shared<FullCalendarBuilder>();
		std::string name = "untitled";
		cout << "Name of calendar:" << endl;
		getline(cin, name);
		cal = builder->buildCalendar(name, years);
		currentDisplay = cal;
	}
}

void CalendarInterface::display() {
	currentDisplay->display();
	//check what view we are in and print the options accordingly
	DisplayableDay * d = dynamic_cast<DisplayableDay*>(currentDisplay.get());
	if (d == nullptr) {
		DisplayableTodolist * tdl = dynamic_cast<DisplayableTodolist*>(currentDisplay.get());
		if (tdl == nullptr) {	//if the current view is a year or month
			cout << "add event: add\nfind event: find\nsave calendar: save\nrestore calendar: restore\nmerge calendar: merge\njump to a date: jump\nzoom out: out\nzoom in: in\nquit: q" << endl;
		}
		else {	//if the current view is a to-do list
			cout << "add task: task\nmark complete: complete\nsave calendar: save\nrestore calendar: restore\nmerge calendar:merge\njump to a date: jump\nzoom out: out\nquit: q" << endl;
		}
	}
	else {	//if the current view is a day
		cout << "add event: add\nfind event: find\nedit event: edit\nadd To-do list: todo\nsave calendar: save\nrestore calendar: restore\nmerge calendar:merge\njump to a date: jump\nzoom out: out\nview to-do list: in\nquit: q" << endl;
	}
	string in;
	cin >> in;
	while (		//keep prompting the user until they input a legal option
		in != "in" &&
		in != "out" &&
		in != "jump" &&
		in != "add" &&
		in != "edit" &&
		in != "find" &&
		in != "todo" &&
		in != "task" &&
		in != "complete" &&
		in != "save" &&
		in != "restore" &&
		in != "merge" &&
		in != "q"
		) {
		cout << "Illegal input. Please try again." << endl;
		cin >> in;
	}
	if (in == "in") {
		DisplayableDay * d = dynamic_cast<DisplayableDay *>(currentDisplay.get());
		if (d == nullptr) {	//if the current view is not a day, ask the user to input the index of which child they are zooming into
			cout << "index? (1-" << currentDisplay->children.size() << ")" << endl;
			int index = 1;
			cin >> index;
			zoomIn(index - 1);
		}
		else {	//if the current view is a day, just zoom into the to-do list. Do not prompt the user
			zoomIn(0);
		}
	}
	else if (in == "out") {
		zoomOut();
	}
	else if (in == "jump") {
		jumpTo();
	}
	else if (in == "add") {
		addEvent();
	}
	else if (in == "edit") {
		editEvent();
	}
	else if (in == "find") {
		find();
	}
	else if (in == "todo") {
		todo();
	}
	else if (in == "task") {
		task();
	}
	else if (in == "complete") {
		complete();
	}
	else if (in == "save") {
		save();
	}
	else if (in == "restore") {
		restore();
	}
	else if (in == "merge") {
		merge();
	}
	else if (in == "q") {
		return;
	}
}

void CalendarInterface::zoomIn(unsigned int index) {
	DisplayableDay * d = dynamic_cast<DisplayableDay *>(currentDisplay.get());
	if (d != nullptr) {	//if the current view is a day
		if (currentDisplay->getTodolist() == nullptr) {	//if the to-do list has not yet been constructed
			cout << "Please first add a to-do list." << endl;
		}
		else {
			currentDisplay = currentDisplay->getTodolist();	//zoom into the to-do list
		}
	}
	else {
		shared_ptr<DisplayableComponent> temp = currentDisplay->getChild(index);	//zoom into whichever child it is zooming into
		if (temp != nullptr) {	//nothing changes if the index is out of bound
			currentDisplay = temp;
		}
	}
	display();
}
void CalendarInterface::zoomOut() {
	if (currentDisplay->getParent().lock() != nullptr) {	//get the parent node and set the current view to the parent
		currentDisplay = currentDisplay->getParent().lock();
	}
	display();
}

void CalendarInterface::addEvent() {
	int year = 2018, month = 1, day = 1, hour = 0, min = 0, recur = 0, last = 0;
	std::string name = "(Untitled Event)", dateInput = "", setRecur = "n";
	struct tm time = { 0 };
	regex r("[1-2][0-9][0-9][0-9]\\s[0-9]+\\s[0-9]+\\s[0-9]+\\s[0-9]+");
	cout << "Please input year, month, day, hour, minute:" << endl;
	cin.ignore();
	std::getline(cin, dateInput);	//use a regex to parse the entire line so that the spaces does not mess with the istringstream
	if (regex_match(dateInput.begin(), dateInput.end(), r)) {	//if matches
		istringstream iss(dateInput);	//wrap the input with an istringstream
		iss >> year >> month >> day >> hour >> min;
		if (year >= 2018 &&
			year < 2018 + (int)(cal->yearsToHold) &&
			month >= 1 &&
			month <= 12 &&
			day >= 1 &&
			day <= (int)(cal->children[year - 2018]->children[month - 1]->children.size()) &&
			hour >= 0 &&
			hour <= 23 &&
			min >= 0 &&
			min <= 59)
		{	//check if none of the time criteria are out of bound
			time.tm_year = year - 2018;
			time.tm_mon = month - 1;
			time.tm_mday = day - 1;
			time.tm_hour = hour;
			time.tm_min = min;
			cout << "Is this a recurring event? y/n" << endl;
			cin >> setRecur;	//prompt the user for recur options
			if (setRecur == "y" || setRecur == "n") {
				if (setRecur == "y") {	//if it is recurring, ask the user for recur settings
					cout << "Event recurs every __ days? Lasts for __ days? Both should be >= 1." << endl;
					cin >> recur >> last;
				}
				else {	//if not, set both to 0
					recur = 0; last = 0;
				}
				cout << "Please input your event:" << endl;	//prompt the user for event name
				cin.ignore();
				getline(cin, name);	//use getline here so the event name can have spaces in it
				builder->buildEvent(cal, name, time, recur, last, "");
			}
			else {
				cout << "Illegal recur argument." << endl;
			}
		}
		else {
			cout << "Illegal time or date." << endl;
		}
	}
	else {
		cout << "Illegal time or date." << endl;
	}
	display();
}

void CalendarInterface::editEvent() {
	std::string indexString = "";
	DisplayableDay * d = dynamic_cast<DisplayableDay *>(currentDisplay.get());
	if (d != nullptr) {	//can only edit event in day view
		regex r("[0-9]+");
		struct tm date = currentDisplay->getDateInfo();
		cout << "Please input the index of the event you want to edit:" << endl;
		cin >> indexString;
		while (!regex_match(indexString.begin(), indexString.end(), r)) {	//keep prompting the user until the index they gives is valid
			cout << "Illegal input." << endl;
			cin >> indexString;
		}
		builder->editEvent(cal, date, stoi(indexString));	//forwards the edit request to the builder
	}
	else {
		cout << "You can only edit an event in day view." << endl;
	}
	display();
}

void CalendarInterface::jumpTo() {
	string dateInput = "", gran = "";
	int year = 2018, month = 1, day = 1, hour = 0, min = 0;
	struct tm time = { 0 };
	regex r("[1-2][0-9][0-9][0-9]\\s[0-9]+\\s[0-9]+");
	cout << "Please input year, month, day:" << endl;
	cin.ignore();
	std::getline(cin, dateInput);
	if (regex_match(dateInput.begin(), dateInput.end(), r)) {
		istringstream iss(dateInput);
		iss >> year >> month >> day;
		if (year >= 2018 &&
			year < 2018 + (int)(cal->yearsToHold) &&
			month >= 1 &&
			month <= 12 &&
			day >= 1 &&
			day <= (int)(cal->children[year - 2018]->children[month - 1]->children.size())
		){	//check if the date they enter is valid
			time.tm_year = year - 2018;
			time.tm_mon = month - 1;
			time.tm_mday = day - 1;
			cout << "Please input granularity: y/m/d" << endl;	//prompt the user for granularity
			cin >> gran;
			if (gran == "y" || gran == "m" || gran == "d") {
				shared_ptr<DisplayableComponent> date = builder->getComponentByDate(cal, time, gran);
				if (date != 0) {
					currentDisplay = date;	//set current display to the component acquired from the builder
				}
				else {
					cout << "Illegal date." << endl;
				}
			}
			else {
				cout << "Illegal granularity." << endl;
			}
		}
		else {
			cout << "Illegal date or time" << endl;
		}
	}
	else {
		cout << "Illegal date." << endl;
	}
	display();
}

void CalendarInterface::find() {
	cout << "Please input event name to start search:" << endl;
	cin.ignore();
	std::string name = "";
	getline(cin, name);
	std::vector<std::shared_ptr<DisplayableComponent>> v = cal->searchMap(name); //forward the search request to the calendar
	if (v.size() == 0) {	//if the returned vector is empty
		cout << "No event found." << endl;
	}
	else {
		cout << "INDEX\t\t\tEVENT" << endl;
		for (int i = 0; i < (int)v.size(); ++i) {	//display all the events found
			tm time = v[i]->getDateInfo();
			cout << i << "\t";
			cout << time.tm_year + 2018 << "/" << time.tm_mon + 1 << "/" << time.tm_mday + 1<< " ";
			v[i]->display();
		}
		if (v.size() != 1) {	//if there are multiple events with the same name, prompt the user to give a index to zoom in
			cout << "Please input an index to display a specific event." << endl;
			int index = -1;
			std::string indexString = "";
			regex r("[0-9]+");
			bool valid = false;
			do {	//keep prompting the user until they gives a valid index
				cin >> indexString;
				if (regex_match(indexString.begin(), indexString.end(), r)) {
					index = stoi(indexString);
					valid = (index < (int)v.size());
				}
				else {
					valid = false;
				}
				if (!valid) {
					cout << "Invalid index. Please input again." << endl;
				}
			} while (!valid);
			cout << "INDEX\t\t\tEVENT" << endl;
			tm time = v[index]->getDateInfo();
			cout << index << "\t";
			cout << time.tm_year + 2018 << "/" << time.tm_mon + 1 << "/" << time.tm_mday + 1<< " ";
			v[index]->display();	//display the final event found
		}
	}
	display();
}

void CalendarInterface::todo() {
	DisplayableDay * d = dynamic_cast<DisplayableDay *>(currentDisplay.get());
	if (d != nullptr) { //build a to-do list if we are in day view
		struct tm date = currentDisplay->getDateInfo();
		builder->buildTodolist(cal, date);
	}
	else {
		cout << "You can only add a To-do list in day view" << endl;
	}
	display();
}

void CalendarInterface::task() {
	int index = -1;
	DisplayableTodolist * tdl = dynamic_cast<DisplayableTodolist *>(currentDisplay.get());
	if (tdl != nullptr) {	//build a task if we are in to-do list view
		struct tm date = currentDisplay->getDateInfo();
		std::string nameTask;
		cout << "Please input the task name:" << endl;
		cin.ignore();
		getline(cin, nameTask);
		builder->buildTask(cal, date, nameTask);	//forward the build request to the builder
	}
	else {
		cout << "You can only add a task in to-do list view" << endl;
	}
	display();
}

//decorator method
void CalendarInterface::complete() {
	DisplayableTodolist * tdl = dynamic_cast<DisplayableTodolist*>(currentDisplay.get());
	if (tdl == nullptr) {	//check if we are in to-do list view
		cout << "You can only mark a task as complete in to-do list view" << endl;
	}
	else {
		cout << "Please input the index of a event to mark it as complete:" << endl;
		int index = -1;
		std::string indexString = "";
		regex r("[0-9]+");
		bool valid = false;
		do {	//keep prompting the user until they gives a valid index
			cin >> indexString;
			valid = regex_match(indexString.begin(), indexString.end(), r);
			if (valid) {
				index = stoi(indexString);
				struct tm date = currentDisplay->getDateInfo();
				builder->markComplete(cal, date, index);	//forward the mark request to the builder
			}
			if (!valid) {
				cout << "Invalid index. Please input again." << endl;
			}
		} while (!valid);
	}
	display();
}

void CalendarInterface::save() {
	builder->saveCalendar(cal);	//forward the save request to the builder
}

void CalendarInterface::restore() {
	cout << "Name of calendar file (*.txt):" << endl;
	std::string name = "";
	cin.ignore();
	getline(cin, name);
	std::ifstream ifs(name);
	if (ifs.is_open()) {	//open the file
		std::string name, yearLine, eventLine, eventName;
		int years, year, month, day, hour, min;
		getline(ifs, name);	//read the first line(name of calendar)
		getline(ifs, yearLine);	//read the second line(years it contains)
		istringstream iss(yearLine);
		iss >> years;
		cal = builder->buildCalendar(name, years);	//build an empty calendar with the name and years
		currentDisplay = cal;
		while (getline(ifs, eventLine)) {	//parse the file line by line to insert all the events
			istringstream eventIss(eventLine);
			eventIss >> year >> month >> day >> hour >> min >> eventName;
			struct tm time = { 0 };
			time.tm_year = year - 2018;
			time.tm_mon = month - 1;
			time.tm_mday = day - 1;
			time.tm_hour = hour;
			time.tm_min = min;
			builder->buildEvent(cal, eventName, time, 0, 0, "");	//forward the add event request to the builder
		}
	}
	else {	//not opened successfully
		cout << "File not exist." << endl;
	}
	display();
}

void CalendarInterface::merge() {
	cout << "Name of calendar file (*.txt):" << endl;
	std::string name = "";
	cin.ignore();
	getline(cin, name);	//prompt the user for the text file the user want to merge into the current one
	std::ifstream ifs(name);
	if (ifs.is_open()) {	//if file exists
		std::string name, yearLine, eventLine, eventName;
		int years, year, month, day, hour, min;
		getline(ifs, name);	//read the first line(name of calendar)
		getline(ifs, yearLine);	//read the second line(years it contains)
		istringstream iss(yearLine);
		iss >> years;
		if (years < (int)(cal->children.size())) {
			years = cal->children.size();
		}
		shared_ptr<Calendar> temp = builder->buildCalendar(cal->getName() + " & " + name, years);	//build a new calendar with the larger number of years and their names combined
		map<shared_ptr<DisplayableComponent>, std::string> thisMap = cal->getMap();	//iterate through the current map and insert everything into the new calendar
		auto it = thisMap.begin();
		while (it != thisMap.end()) {
			struct tm date = it->first->getDateInfo();
			EventDecorator * ed = dynamic_cast<EventDecorator*>(it->first.get());
			if (ed == nullptr) {	//if the events in the current calendar is not yet decorated
				builder->buildEvent(temp, it->second, it->first->getDateInfo(), 0, 0, cal->getName());
			}
			else {	//if the events in the current calendar is already decorated, we just go by the orignal calendar names so that they don't get recursively added
				builder->buildEvent(temp, it->second, it->first->getDateInfo(), 0, 0, it->first->getCalendarName());
			}
			it++;
		}
		while (getline(ifs, eventLine)) {	//parse every line in the file and insert the events with decorator(name)
			istringstream eventIss(eventLine);
			eventIss >> year >> month >> day >> hour >> min >> eventName;
			struct tm time = { 0 };
			time.tm_year = year - 2018;
			time.tm_mon = month - 1;
			time.tm_mday = day - 1;
			time.tm_hour = hour;
			time.tm_min = min;
			builder->buildEvent(temp, eventName, time, 0, 0, name);
		}
		cal = temp;
		currentDisplay = cal;
	}
	else {	//file isn't successfully opened
		cout << "File not exist." << endl;
	}
	display();
}
