/*
Author: Jon Shidal, Ethan Zheng
Purpose: Define builder classes, responsible for building and managing the structure of the calendar
*/
#include "stdafx.h"
#include "calendarbuilder.h"
#include "calendarcomponents.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

using namespace std;

shared_ptr<Calendar> FullCalendarBuilder::buildCalendar(string name, size_t years) {
	currentCalendar = make_shared<Calendar>(name, years);
	// construct each year in a recursive way, add each year as a child of the calendar
	for (int i = 0; i < (int)years; ++i) {
		tm y = currentCalendar->dateInfo;
		y.tm_year += i;
		y.tm_wday = (y.tm_wday + CalendarComponent::DAYSINAYEAR * i) % CalendarComponent::DAYSINAWEEK; // calculate day of the week for first day of the year
		currentCalendar->addComponent(buildYear(y, currentCalendar));
	}
	return currentCalendar;
}

//save a calendar to a file of its name
void FullCalendarBuilder::saveCalendar(std::shared_ptr<DisplayableComponent> cal) {
	std::string name = cal->getName();
	std::string nameOutput = name;
	name += ".txt";
	ofstream ofs(name);
	if (ofs.is_open()) {
		ofs << nameOutput << endl;
		ofs << cal->children.size() << endl;
		std::map<std::shared_ptr<DisplayableComponent>, std::string> eventMap = cal->getMap();	//get the map from calendar
		auto it = eventMap.begin();
		while (it != eventMap.end()) {	//iterate through the map and save every event in a readable format
			ofs << it->first->getDateInfo().tm_year + 2018 << " ";
			ofs << it->first->getDateInfo().tm_mon + 1 << " ";
			ofs << it->first->getDateInfo().tm_mday + 1 << " ";
			ofs << it->first->getDateInfo().tm_hour << " ";
			ofs << it->first->getDateInfo().tm_min << " ";
			ofs << it->first->getName() << endl;
			it++;
		}
		ofs.close();
	}
}

//build an event and add it to the calendar (add it to a day)
shared_ptr<DisplayableComponent> FullCalendarBuilder::buildEvent(shared_ptr<DisplayableComponent> cal, string name, tm when, int recurEvery, int recurDays, std::string decoration) {
	shared_ptr<DisplayableComponent> date = 0;
	if (recurEvery != 0 && recurDays != 0) {	//check recur
		for (int i = 0; i < recurDays; ++i) {
			shared_ptr<DisplayableEvent> event = make_shared<DisplayableEvent>(when, cal, name);
			date = getComponentByDate(cal, when, "d");
			if (date == 0) {	//when an event goes out of bound of the calendar
				return date;
			}
			date->addComponent(event);	//add the event to the date by calling addComponent method
			cal->updateMap(event, name, "insert");	//insert the event into the map of all events
			when.tm_mday += recurEvery;	//jump to the next date
			//increment the month and year accordingly
			if (when.tm_mon == 0 || when.tm_mon == 2 || when.tm_mon == 4 || when.tm_mon == 6 || when.tm_mon == 7 || when.tm_mon == 9 || when.tm_mon == 11) {
				if (when.tm_mday >= 31) {
					when.tm_mday -= 31;
					when.tm_mon += 1;
				}
			}
			else if (when.tm_mon == 3 || when.tm_mon == 5 || when.tm_mon == 8 || when.tm_mon == 10) {
				if (when.tm_mday >= 30) {
					when.tm_mday -= 30;
					when.tm_mon += 1;
				}
			}
			else {
				if (when.tm_mday >= 28) {
					when.tm_mday -= 28;
					when.tm_mon += 1;
				}
			}
			if (when.tm_mon >= 12) {
				when.tm_mon -= 12;
				when.tm_year += 1;
			}
		}
	}
	else {	//event doesn't recur
		shared_ptr<DisplayableComponent> event = make_shared<DisplayableEvent>(when, cal, name);
		if (decoration != "") {	//if the event is built with a decoration (like a calendar name), decorate it with its decoration
			event = cal->decorate(event, decoration);
		}
		date = getComponentByDate(cal, when, "d");
		date->addComponent(event);
		cal->updateMap(event, name, "insert");
	}
	return date;
}

//edit the event. This includes deleting and editing. Editing is basically just deleting and constructing a new event according to the user's need
shared_ptr<DisplayableComponent> FullCalendarBuilder::editEvent(shared_ptr<DisplayableComponent> cal, tm date, int index) {
	shared_ptr<DisplayableComponent> d = cal->children[date.tm_year - 118]->children[date.tm_mon]->children[date.tm_mday - 1];
	if (index < (int)(d->children.size())) {	//check if the index given is out of bound
		std::string option = "";
		cout << "Do you want to edit or delete? e/d" << endl;
		cin >> option;
		while (option != "e" && option != "d") {	//keep prompting the user until they gives a valid option
			cout << "Illegal option." << endl;
			cin >> option;
		}
		if (option == "d") {	//delete event from the map first and then from the children vector
			cal->updateMap(d->children[index], "", "erase");
			d->children.erase(d->children.begin() + index);
			return d;
		}
		else {
			cal->updateMap(d->children[index], "", "erase");
			d->children.erase(d->children.begin() + index);
			//asking for a new event and build it
			int year = 2018, month = 1, day = 1, hour = 0, min = 0;
			std::string name = "(Untitled Event)", dateInput = "";
			struct tm time = { 0 };
			regex r("[1-2][0-9][0-9][0-9]\\s[0-9]+\\s[0-9]+\\s[0-9]+\\s[0-9]+");
			cout << "Please input the new time info (year, month, day, hour, minute):" << endl;
			cin.ignore();
			std::getline(cin, dateInput);
			if (regex_match(dateInput.begin(), dateInput.end(), r)) {
				istringstream iss(dateInput);
				iss >> year >> month >> day >> hour >> min;
				time.tm_year = year - 2018;
				time.tm_mon = month - 1;
				time.tm_mday = day - 1;
				time.tm_hour = hour;
				time.tm_min = min;
				cout << "Please input the new name of your event:" << endl;
				getline(cin, name);
				this->buildEvent(cal, name, time, 0, 0, "");
			}
			return d;
		}
	}
	else {
		cout << "Illegal index" << endl;
		return d;
	}
}

//build to-do list
shared_ptr<DisplayableComponent> FullCalendarBuilder::buildTodolist(shared_ptr<DisplayableComponent> cal, std::tm date) {
	shared_ptr<DisplayableComponent> d = cal->children[date.tm_year - 118]->children[date.tm_mon]->children[date.tm_mday - 1];
	shared_ptr<DisplayableComponent> todolist = make_shared<DisplayableTodolist>(date, d);
	d->addTodolist(todolist);	//calls the function of adding a to-do list to a specific date. It has a singleton pattern that ensures only one to-do list exists for every day.
	return todolist;
}

//build a task and push it on to the children of the to-do list
shared_ptr<DisplayableComponent> FullCalendarBuilder::buildTask(shared_ptr<DisplayableComponent> cal, std::tm date, std::string name) {
	shared_ptr<DisplayableComponent> tdl = cal->children[date.tm_year - 118]->children[date.tm_mon]->children[date.tm_mday - 1]->getTodolist();
	shared_ptr<DisplayableComponent> task = make_shared<DisplayableTask>(date, tdl, name);
	tdl->children.push_back(task);
	return task;
}

//mark a task as complete
shared_ptr<DisplayableComponent> FullCalendarBuilder::markComplete(std::shared_ptr<DisplayableComponent> cal, std::tm date, int index) {
	shared_ptr<DisplayableComponent> tdl = cal->children[date.tm_year - 118]->children[date.tm_mon]->children[date.tm_mday - 1]->getTodolist();
	if (index < (int)(tdl->children.size())) {
		shared_ptr<DisplayableComponent> ct = make_shared<CompleteTask>(date, tdl, tdl->children[index]->getName());	//construct a decorator task with the original task and push it to its original position
		tdl->children[index] = ct;
	}
	else {
		cout << "Illegal index." << endl;
	}
	return tdl;
}

//get a component by its date
shared_ptr<DisplayableComponent> FullCalendarBuilder::getComponentByDate(shared_ptr<DisplayableComponent> cal, tm d, string granularity) {
	shared_ptr<DisplayableComponent> date = 0;
	if (d.tm_year >= 0 &&
		d.tm_year < (int)cal->children.size() &&
		d.tm_mon >= 0 &&
		d.tm_mon <= 12 &&
		d.tm_hour >= 0 &&
		d.tm_hour <= 23 &&
		d.tm_min >= 0 &&
		d.tm_min <= 59)
	{
		if (d.tm_mday >= 0 && d.tm_mday < (int)cal->children[d.tm_year]->children[d.tm_mon]->children.size()) {
			//specify granularity
			if (granularity == "y") {
				date = cal->children[d.tm_year];
			}
			else if (granularity == "m") {
				date = cal->children[d.tm_year]->children[d.tm_mon];
			}
			else if (granularity == "d") {
				date = cal->children[d.tm_year]->children[d.tm_mon]->children[d.tm_mday];
			}
		}
	}
	return date;
}

shared_ptr<DisplayableComponent> FullCalendarBuilder::buildDay(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	shared_ptr<DisplayableComponent> day = make_shared<DisplayableDay>(d, p);
	return day;
}

shared_ptr<DisplayableComponent> FullCalendarBuilder::buildMonth(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	int index = d.tm_mon;
	shared_ptr<DisplayableComponent> m = make_shared<DisplayableMonth>(d, p, CalendarComponent::months[index], CalendarComponent::days[index]);
	for (int i = 0; i < CalendarComponent::days[index]; ++i) { // for each day in the month
		m->addComponent(buildDay(d, m)); // construct day and add as a child of the month
		++(d.tm_mday); // increment day of the month
		d.tm_wday = (d.tm_wday + 1) % CalendarComponent::DAYSINAWEEK; // increment weekday, reset to 0 if needed
	}
	return m;
}
shared_ptr<DisplayableComponent> FullCalendarBuilder::buildYear(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	shared_ptr<DisplayableComponent> y = make_shared<DisplayableYear>(d,p,false);
	// construct each month and add it as a child of the year
	for (int i = 0; i < CalendarComponent::MONTHS; ++i) {
		d.tm_mon = i;
		y->addComponent(buildMonth(d, y));
		// set week day of first day of the next month
		d.tm_wday = (d.tm_wday + CalendarComponent::days[i]) % CalendarComponent::DAYSINAWEEK;
	}
	return y;
}
