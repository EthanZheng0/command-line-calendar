/*
Author: Jon Shidal, Ethan Zheng
Purpose: Define Calendar component classes.
*/

#include "stdafx.h"
#include "calendarcomponents.h"
#include<iostream>
#include<string>
#include<algorithm>
#include<map>

using namespace std;

// from <ctime>
//struct tm {
//	int tm_sec;   // seconds of minutes from 0 to 61
//	int tm_min;   // minutes of hour from 0 to 59
//	int tm_hour;  // hours of day from 0 to 24
//	int tm_mday;  // day of month from 1 to 31
//	int tm_mon;   // month of year from 0 to 11
//	int tm_year;  // year since 1900
//	int tm_wday;  // days since sunday
//	int tm_yday;  // days since January 1st
//	int tm_isdst; // hours of daylight savings time
//}

// static class variables
const std::vector<string> CalendarComponent::daysoftheweek = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const std::vector<string> CalendarComponent::months = { "January", "February", "March", "April", "May", "June", "July", "August","September", "October", "November","December" };
const std::vector<int> CalendarComponent::days = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const int CalendarComponent::DAYSINAWEEK = 7;
const int CalendarComponent::MONTHS = 12;
const int CalendarComponent::BASEYEAR = 1900;
const int CalendarComponent::DAYSINAYEAR = 365;

// CalendarComponent
CalendarComponent::CalendarComponent(std::tm d, std::shared_ptr<DisplayableComponent> p) : DisplayableComponent(p), dateInfo(d){}

//All the default methods here. They don't do anything. Typically they are just overidden by specific methods defined in different types of calendar components

std::map<std::shared_ptr<DisplayableComponent>, std::string> CalendarComponent::getMap() {
	std::map<std::shared_ptr<DisplayableComponent>, std::string> m;
	return m;
}

tm CalendarComponent::getDateInfo() {
	return dateInfo;
}

std::string CalendarComponent::getName() {
	return "";
}

std::string CalendarComponent::getCalendarName() {
	return "";
}

std::shared_ptr<DisplayableComponent> CalendarComponent::getTodolist() {
	return nullptr;
}

void CalendarComponent::display(std::string s) {}

std::vector<std::shared_ptr<DisplayableComponent>> CalendarComponent::getEvents() {
	std::vector<std::shared_ptr<DisplayableComponent>> v;
	return v;
}

std::shared_ptr<DisplayableComponent> CalendarComponent::decorate(std::shared_ptr<DisplayableComponent>, std::string decoration) {
	return 0;
}

// Event
DisplayableEvent::DisplayableEvent(std::tm d, std::shared_ptr<DisplayableComponent> p, std::string eventName) : CalendarComponent(d, p), name(eventName){ }

void DisplayableEvent::display() {
	std::string hour = "", min = "";
	if ((int)dateInfo.tm_hour < 10) {	//add 0s to the display so that it's displayed as 07:05 not 7:5
		hour += "0";
	}
	if ((int)dateInfo.tm_min < 10) {	//add 0s to the display so that it's displayed as 07:05 not 7:5
		min += "0";
	}
	hour += std::to_string(dateInfo.tm_hour);
	min += std::to_string(dateInfo.tm_min);
	cout << "\t" << hour << ":" << min << " ";
	cout << name << endl;
}

std::string DisplayableEvent::getName() {
	return name;
}

// Day
DisplayableDay::DisplayableDay(std::tm d, std::shared_ptr<DisplayableComponent> p) : CalendarComponent(d, p){ }

void DisplayableDay::display() {
	cout << endl << daysoftheweek[dateInfo.tm_wday] << " ";
	cout << dateInfo.tm_mon + 1 << "/" << dateInfo.tm_mday << "/" << dateInfo.tm_year + CalendarComponent::BASEYEAR << endl;
	if (todolist != nullptr) {	//display a to-do list entrance if there exists one
		cout << "TODO LIST" << endl;
	}
	cout << "INDEX\tEVENT" << endl;
	for (size_t i = 0; i < children.size(); ++i) {
		cout << i;
		if (children[i] != nullptr) { // forward request to all children
			children[i]->display();
		}
	}
	cout << endl;
}

shared_ptr<DisplayableComponent> DisplayableDay::addComponent(shared_ptr<DisplayableComponent> comp) {
	// try to dynamically cast comp to a pointer to a DisplayableDay, will fail if the DisplayableComponent is not a day
	// can only cast regular pointers, not shared_ptrs
	// here we check if the component passed in is either a normal event or a decorated event
	DisplayableEvent* e = dynamic_cast<DisplayableEvent *>(comp.get());
	if (e == nullptr) {
		EventDecorator* ed = dynamic_cast<EventDecorator *>(comp.get());
		if(ed == nullptr){
			return nullptr;
		}
	}
	//proceed only if it is an event
	if (children.size() == 0) {
		children.push_back(comp);	//insert it if it's the first child. It is trivially sorted
	}
	else {
		//compare the time of the newly added child to all other existing children (already sorted)
		int time = 100 * comp->getDateInfo().tm_hour + comp->getDateInfo().tm_min;
		for (int i = 0; i < (int)children.size(); ++i) {
			if (time <= 100 * children[i]->getDateInfo().tm_hour + children[i]->getDateInfo().tm_min) {
				children.insert(children.begin() + i, comp);	//find the first event whose time is later than the one we have and insert our event before it
				return comp;
			}
		}
		children.push_back(comp);	//if no event has a time later than it, put it at the very end
	}
	return comp;
}

int DisplayableDay::addTodolist(shared_ptr<DisplayableComponent> comp) {
	//singleton pattern. Ensures that only one to-do list exists for each day
	if (todolist == nullptr) {
		todolist = comp;
		return 0;
	}
	return -1;
}

//get a vector of all the events in one single day. Need this for searching and editing
std::vector<std::shared_ptr<DisplayableComponent>> DisplayableDay::getEvents() {
	std::vector<std::shared_ptr<DisplayableComponent>> v;
	for (int i = 0; i < (int)children.size(); ++i) {
		v.push_back(children[i]);
	}
	return v;
}

//get the to-do list for the day. Will return nullptr if there isn't one
std::shared_ptr<DisplayableComponent> DisplayableDay::getTodolist() {
	return todolist;
}

// Month
DisplayableMonth::DisplayableMonth(std::tm d, std::shared_ptr<DisplayableComponent> p, string monthname, unsigned int numdays) : CalendarComponent(d, p), name(monthname), numberOfDays(numdays) {
	// initialize children vector, one spot for each day
	for (size_t i = 0; i < numberOfDays; ++i) {
		children.push_back(nullptr);
	}
}

void DisplayableMonth::display() {	//month view display
	cout << endl << name << ":" << endl;	//print the name of the month
	cout << "Sunday     Monday     Tuesday    Wednesday  Thursday   Friday     Saturday" << endl;	//print all the days of a week, each occupying 11 characters
	//the following block accounts for the first row in the calendar
	int wdayFirst = children[0]->getDateInfo().tm_wday;		//get the day of week of the first day in the month
	for (int i = 0;i < wdayFirst; ++i) {
		cout << "           ";	//print all the spaces needed. 11 spaces for each empty day
	}
	for (int i = wdayFirst; i < 7; ++i) {
		cout << i - wdayFirst + 1 << "          "; //print all the dates for the first rows. Since it can't exceed 10, just print 10 spaces for each
	}
	cout << endl;
	int mostEventNum = 0;
	for (int i = wdayFirst; i < 7; ++i) {	//iterate through every day in the first row and record the most number of events
		if ((int)(children[i - wdayFirst]->getEvents().size()) > mostEventNum) {
			mostEventNum = (int)(children[i - wdayFirst]->getEvents().size());
		}
	}
	for (int j = 0; j < mostEventNum; ++j) {	//this is the numbers of lines we need for this row's events
		for (int i = 0;i < wdayFirst; ++i) {	//as usual, print all the spaces needed
			cout << "           ";
		}
		for (int i = wdayFirst; i < 7; ++i) {
			vector<shared_ptr<DisplayableComponent>> events = children[i - wdayFirst]->getEvents();
			if (j < (int)events.size()) {	//if the jth event exists for a certain day, truncate it with spaces to 11 characters and print it out
				string name = events[j]->getName() + "           ";
				name.resize(11);
				cout << name;
			}
			else {	//if it does not exist for a certain day, print out 11 spaces
				cout << "           ";
			}
		}
		cout << endl;
	}
	cout << endl;

	//the following block is for every rows in the middle of the calendar
	int count = 0;	//set up a counter to keep track of which day we end on
	for (int i = 7 - wdayFirst; i < (int)children.size() - 7; i += 7) {		//stop if i exceeds the days of the month minus seven. Leaves the last row for the next block
		count = i;
		for (int j = i; j < i + 7; ++j) {	//print out all the dates
			if (j < 9) {
				cout << j + 1 << "          ";	//add 10 spaces if it's < 10
			}
			else {
				cout << j + 1 << "         ";	//add 9 spaces otherwise
			}
		}
		cout << endl;
		int mostEventNum = 0;
		for (int j = i; j < i + 7; ++j) {	//record the most number of events for each week
			if ((int)(children[j]->getEvents().size()) > mostEventNum) {
				mostEventNum = (int)(children[j]->getEvents().size());
			}
		}
		for (int k = 0; k < mostEventNum; ++k) {	//iterate this many times for the events
			for (int j = i; j < i + 7; ++j) {
				vector<shared_ptr<DisplayableComponent>> events = children[j]->getEvents();
				if (k < (int)events.size()) {	//truncate and print it if it exists
					string name = events[k]->getName() + "           ";
					name.resize(11);
					cout << name;
				}
				else {		//print out spaces if not
					cout << "           ";
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	//the following block is for the last row in each month
	for (int i = count + 7; i < (int)children.size(); ++i) {	//print out the dates starting from 7 days after where we stopped in the previous block
		cout << i + 1 << "         ";
	}
	cout << endl;
	mostEventNum = 0;
	for (int i = count + 7; i < (int)children.size(); ++i) {	//record the most number of events
		if ((int)(children[i]->getEvents().size()) > mostEventNum) {
			mostEventNum = (int)(children[i]->getEvents().size());
		}
	}
	for (int j = 0; j < mostEventNum; ++j) {	//iterate this many times
		for (int i = count + 7; i < (int)children.size(); ++i) {
			vector<shared_ptr<DisplayableComponent>> events = children[i]->getEvents();
			if (j < (int)events.size()) {	//truncate and print the event if it exists
				string name = events[j]->getName() + "           ";
				name.resize(11);
				cout << name;
			}
			else {	//print out spaces if not
				cout << "           ";
			}
		}
		cout << endl;
	}
	cout << endl;
}

//this is an overloaded display method designed for the year view, so that it can forward its display to all the months it contains
void DisplayableMonth::display(std::string) {
	cout << endl << "\t" << name << endl;
	cout << "\tS\tW\tT\tW\tT\tF\tS" << endl;
	int wdayFirst = children[0]->getDateInfo().tm_wday;	//get the day of week for the first day in the month
	cout << "\t";
	for (int i = 0; i < wdayFirst; ++i) {	//print out this many tabs
		cout << "\t";
	}
	for (int i = 0; i < (int)children.size(); ++i) {	//print out all the dates
		cout << i + 1 << "\t";
		if (children[i]->getDateInfo().tm_wday == 6) {	//start a new line when it reaches Saturday
			cout << endl << "\t";
		}
	}
	cout << endl;
}

std::string DisplayableMonth::getName() {	//get the name of month
	return name;
}

shared_ptr<DisplayableComponent> DisplayableMonth::addComponent(shared_ptr<DisplayableComponent> comp) {
	// try to dynamically cast comp to a pointer to a DisplayableDay, will fail if the DisplayableComponent is not a day
	DisplayableDay* d = dynamic_cast<DisplayableDay *>(comp.get()); // can only cast regular pointers, not shared_ptrs
	if (d == nullptr) { // the cast failed
		return nullptr;
	}
	// otherwise, add the day to the correct location
	int dayOfMonth = d->dateInfo.tm_mday - 1;
	if (children[dayOfMonth] == nullptr) { // day does not already exist
		children[dayOfMonth] = comp;
		return comp;
	}
	else {  // day already exist, return existing day
		return children[dayOfMonth];
	}
}

// Year
DisplayableYear::DisplayableYear(std::tm d, std::shared_ptr<DisplayableComponent> p, bool l) : CalendarComponent(d, p), leap(l) {
	for (size_t i = 0; i < CalendarComponent::MONTHS; ++i) {
		children.push_back(nullptr);
	}
}

void DisplayableYear::display() {
	cout << endl << "Year " << dateInfo.tm_year + CalendarComponent::BASEYEAR << ":" << endl;
	for (size_t i = 0; i < children.size(); ++i) { // forward request to all children
		if (children[i] != nullptr) {
			children[i]->display("yearView");	//use the overloaded display method for months designed for yearview. Technically can pass in any string
		}
	}
}

shared_ptr<DisplayableComponent> DisplayableYear::addComponent(shared_ptr<DisplayableComponent> comp) {
	// try to dynamically cast comp to a pointer to a DisplayableMonth
	DisplayableMonth * m = dynamic_cast<DisplayableMonth *>(comp.get());
	if (m == nullptr) { // if the cast fails, return nullptr
		return nullptr;
	}
	// otherwise, add the month to the correct location
	int monthOfYear = m->dateInfo.tm_mon;
	if (children[monthOfYear] == nullptr) { // month does not already exist
		children[monthOfYear] = comp;
		return comp;
	}
	else {  // month already exist, return existing month
		return children[monthOfYear];
	}
}

// Calendar
Calendar::Calendar(std::string n, size_t y) : CalendarComponent(tm(), nullptr), name(n), yearsToHold(y) { // just initialize with a default tm for now.
	time_t now = time(0); // get the current time
	tm now_tm;
	gmtime_s(&now_tm, &now); // create a struct tm(now_tm) from the current time
	currentDate = now_tm;    // set Calendar's date and time to now
	dateInfo = now_tm; // setup dateInfo to represent January 1st of the current year, start time of the calendar
	dateInfo.tm_sec = 0;
	dateInfo.tm_min = 0;
	dateInfo.tm_hour = 0;
	dateInfo.tm_mday = 1;
	dateInfo.tm_mon = 0;
	// calculate and set day of the week to that of January 1st, 2018. Very sloppy, I know
	dateInfo.tm_wday = (now_tm.tm_wday + CalendarComponent::DAYSINAWEEK - (now_tm.tm_yday % CalendarComponent::DAYSINAWEEK)) % CalendarComponent::DAYSINAWEEK;
	dateInfo.tm_yday = 0;
	dateInfo.tm_isdst = 0;
	// intialize calendar to hold __ years
	for (size_t i = 0; i < yearsToHold; ++i) {
		children.push_back(nullptr);
	}
}

void Calendar::display() {
	cout << endl << "Calendar: " << name << endl;
	cout << "INDEX\t\tYEAR" << endl;
	for (size_t i = 0; i < children.size(); ++i) {	//just show all the existing years here
		if (children[i] != nullptr) {
			cout << i+1 << "\t\t";
			cout << children[i]->getDateInfo().tm_year + 1900 << endl;
		}
	}
}

shared_ptr<DisplayableComponent> Calendar::addComponent(std::shared_ptr<DisplayableComponent> comp) {
	DisplayableYear* y = dynamic_cast<DisplayableYear *>(comp.get());
	if (y == nullptr) { // if the cast fails, return nullptr
		return nullptr;
	}
	int calendarYear = CalendarComponent::BASEYEAR + dateInfo.tm_year;
	int yearAdding = CalendarComponent::BASEYEAR + y->dateInfo.tm_year;
	int index = yearAdding - calendarYear; // which child?
	if (index >= 0 && index < (int)children.size() && children[index] == nullptr) {
		children[index] = comp;
		return comp;
	}
	else {
		return nullptr;
	}
}

void Calendar::updateMap(std::shared_ptr<DisplayableComponent> p, std::string name, std::string option) {	//update map method includes both insertion and deletion
	if (option == "insert") {
		eventMap.insert(make_pair(p, name));
	}
	else if (option == "erase") {
		eventMap.erase(p);
	}
}

std::string Calendar::getName() {	//getter for calendar name
	return name;
}

std::map<std::shared_ptr<DisplayableComponent>, std::string> Calendar::getMap() {	//getter for the map of all events. Designed for edit, find, restore, save and merge
	return eventMap;
}

std::vector<std::shared_ptr<DisplayableComponent>> Calendar::searchMap(std::string name) {
	vector<shared_ptr<DisplayableComponent>> v;
	auto it = eventMap.begin();
	while (it != eventMap.end()) {	//iterate through the map and for every pair that matches in value(event name), push the key(event pointer) to the vector we return
		if (it->second == name) {
			v.push_back(it->first);
		}
		it++;
	}
	return v;
}

//Decoration pattern
std::shared_ptr<DisplayableComponent> Calendar::decorate(std::shared_ptr<DisplayableComponent> comp, std::string decoration) {
	struct tm time = comp->getDateInfo();
	DisplayableEvent* e = dynamic_cast<DisplayableEvent *>(comp.get());
	if (e != nullptr) {	//if the component is an event, create a decorated event with it and replace it with the decorated event
		shared_ptr<DisplayableComponent> decorated = make_shared<EventDecorator>(time, comp, comp->getName(), decoration);
		return decorated;
	}
	return nullptr;
}

DisplayableComponentDecorator::DisplayableComponentDecorator(std::tm d, std::shared_ptr<DisplayableComponent> p) : CalendarComponent(d, p) {}

std::string DisplayableComponentDecorator::getName() {	//do nothing as default
	return "";
}

std::string DisplayableComponentDecorator::getCalendarName() {	//do nothing as default
	return "";
}

EventDecorator::EventDecorator(std::tm d, std::shared_ptr<DisplayableComponent> p, std::string s, std::string cName) : DisplayableComponentDecorator(d, p), name(s), calendarName(cName) {}

std::string EventDecorator::getName() {	//return the event name
	return name;
}

std::string EventDecorator::getCalendarName() {	//return the name of the calendar it was orignially in. Designed for merge
	return calendarName;
}

void EventDecorator::display() {
	std::string hour = "", min = "";
	if ((int)dateInfo.tm_hour < 10) {
		hour += "0";
	}
	if ((int)dateInfo.tm_min < 10) {
		min += "0";
	}
	hour += std::to_string(dateInfo.tm_hour);
	min += std::to_string(dateInfo.tm_min);
	cout << "\t" << hour << ":" << min << " ";
	cout << calendarName << "::" << name << endl;	//this is the only thing different for this display method, where it adds the decoration(calendar name) into display
}

//To-do list
DisplayableTodolist::DisplayableTodolist(std::tm d, std::shared_ptr<DisplayableComponent> p) : CalendarComponent(d, p) {}

void DisplayableTodolist::display() {
	cout << endl << "To-Do List\t" << dateInfo.tm_mon + 1 << "/" << dateInfo.tm_mday << "/" << dateInfo.tm_year + CalendarComponent::BASEYEAR << endl;
	cout << "INDEX\t\tTASK" << endl;
	for (int i = 0; i < (int)(children.size()); ++i) {
		cout << i << "\t\t";
		children[i]->display();
	}
	cout << endl;
}

std::shared_ptr<DisplayableComponent> DisplayableTodolist::addComponent(std::shared_ptr<DisplayableComponent> comp) {
	// try to dynamically cast comp to a pointer to a DisplayableTask
	// will fail if the DisplayableComponent is not a task
	// can only cast regular pointers, not shared_ptrs
	// check if the component passed in is either a normal task or a decorated task
	DisplayableTask* t = dynamic_cast<DisplayableTask *>(comp.get());
	if (t == nullptr) {
		CompleteTask* td = dynamic_cast<CompleteTask *>(comp.get());
		if (td == nullptr) {
			return nullptr;
		}
	}
	children.push_back(comp);
	return comp;
}

//Task
DisplayableTask::DisplayableTask(std::tm d, std::shared_ptr<DisplayableComponent> p, std::string s) : CalendarComponent(d, p), name(s) {}

void DisplayableTask::display() {
	cout << "To-do: " << name << endl;	//displays with a to-do by default
}

std::string DisplayableTask::getName() {
	return name;
}

//Decoration pattern for task
CompleteTask::CompleteTask(std::tm d, std::shared_ptr<DisplayableComponent> p, std::string s) : DisplayableTask(d, p, s) {}

void CompleteTask::display() {	//displays with a complete if it's a decorated task
	cout << "Complete: " << name << endl;
}

std::string CompleteTask::getName() {
	return name;
}
