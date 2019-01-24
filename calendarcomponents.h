#pragma once
/*
Author: Jon Shidal, Ethan Zheng
Purpose:
This file contains declarations for various components used in a Calendar, as well as the Calendar itself.
All components inherit from DisplayableComponent and have a member variable that is a std::tm object, representing
its date and time.
*/
#include "displayablecomponent.h"
#include<ctime>
#include<string>
#include<map>
#include<list>
#include<iostream>

// here is the layout of the tm struct, it is declared in <ctime>

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

// forward declarations needed to avoid circular includes, used to declare friends only
class FullCalendarBuilder;
class CalendarInterface;

class CalendarComponent : public DisplayableComponent {
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	// inherited
	virtual void display() = 0;
	virtual void display(std::string);
	virtual tm getDateInfo();
	virtual std::string getName();
	virtual std::string getCalendarName();
	virtual std::vector<std::shared_ptr<DisplayableComponent>> getEvents();
	virtual std::map<std::shared_ptr<DisplayableComponent>, std::string> getMap();
	virtual std::shared_ptr<DisplayableComponent> getTodolist();
	virtual std::shared_ptr<DisplayableComponent> decorate(std::shared_ptr<DisplayableComponent>, std::string);
protected:
	// stores date/time associated with this component
	// see struct tm details above
	std::tm dateInfo;

	// constructor, protected is ok. Builder class is a friend
	CalendarComponent(std::tm, std::shared_ptr<DisplayableComponent>);

	// some static constants to use in place of hardcoded calendar values
	static const std::vector<std::string> daysoftheweek;
	static const std::vector<std::string> months;
	static const std::vector<int> days;
	static const int DAYSINAWEEK;
	static const int MONTHS;
	static const int BASEYEAR;
	static const int DAYSINAYEAR;
};

class DisplayableEvent : public CalendarComponent {
	friend class DisplayableDay;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	DisplayableEvent(std::tm, std::shared_ptr<DisplayableComponent>, std::string);
	virtual void display() override;
	virtual std::string getName() override;
protected:
	std::string name;
	//Event is now the leaf node. No need to override addComponent()
};

class DisplayableDay : public CalendarComponent {
	friend class DisplayableMonth;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	// 1st argument = start date/timeof the day, 2nd argument = its parent
	DisplayableDay(std::tm, std::shared_ptr<DisplayableComponent>);
	virtual void display() override;
	virtual std::vector<std::shared_ptr<DisplayableComponent>> getEvents() override;
	virtual std::shared_ptr<DisplayableComponent> getTodolist() override;
protected:
	// Days contains events, so it is a composite object. override addComponent accordingly
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
	virtual int addTodolist(std::shared_ptr<DisplayableComponent>) override;
	std::shared_ptr<DisplayableComponent> todolist;
};

class DisplayableMonth : public CalendarComponent {
	friend class DisplayableYear;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	// arguments = date/time info, its parent, name of the month, days in the month
	DisplayableMonth(std::tm, std::shared_ptr<DisplayableComponent>, std::string monthname, unsigned int numdays);
	virtual void display() override;
	virtual std::string getName() override;
	virtual void display(std::string) override;
protected:
	std::string name;
	unsigned int numberOfDays;
	// Month contains days, so it is a composite object. override addComponent accordingly
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
};

class Calendar;

class DisplayableYear : public CalendarComponent {
	friend Calendar;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	// arguments: date/time info, parent, leap year or no?
	DisplayableYear(std::tm, std::shared_ptr<DisplayableComponent>, bool);
	virtual void display() override;
protected:
	bool leap;
	// year contains months - override accordingly
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
};

class Calendar : public CalendarComponent {
	// friends
	friend FullCalendarBuilder;
	friend CalendarInterface;
protected:
	std::string name;
	size_t yearsToHold;
	std::tm currentDate; // current date and time
	std::map<std::shared_ptr<DisplayableComponent>, std::string> eventMap;
	// dateInfo is the start date and time
	// Calendar contains years, so override accordingly
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
	virtual void updateMap(std::shared_ptr<DisplayableComponent>, std::string, std::string) override;	//update the event map
	virtual std::vector<std::shared_ptr<DisplayableComponent>> searchMap(std::string) override;		//search the event map
public:
	// arguments: name of the calendar, length of the calendar in years
	Calendar(std::string n, size_t y);
	// inherited methods
	virtual void display() override;
	virtual std::map<std::shared_ptr<DisplayableComponent>, std::string> getMap() override;
	virtual std::string getName() override;
	virtual std::shared_ptr<DisplayableComponent> decorate(std::shared_ptr<DisplayableComponent>, std::string) override;
};

//Decoration pattern for events
class DisplayableComponentDecorator : public CalendarComponent {
	// friends
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	DisplayableComponentDecorator(std::tm d, std::shared_ptr<DisplayableComponent> p);
	virtual std::string getName() override;
	virtual std::string getCalendarName() override;
protected:
	std::shared_ptr<DisplayableComponent> dc;
};

//Decorator for events. It has a distinct display method which adds the stored calendar name to the display
class EventDecorator : public DisplayableComponentDecorator {
	// friends
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	virtual void display();
	EventDecorator(std::tm, std::shared_ptr<DisplayableComponent>, std::string, std::string);
	virtual std::string getName() override;
	virtual std::string getCalendarName() override;
protected:
	std::string name;	//event name
	std::string calendarName;	//the name of the calendar it originally belongs to
};

class DisplayableTodolist : public CalendarComponent {
	//friends
	friend class DisplayableDay;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	DisplayableTodolist(std::tm, std::shared_ptr<DisplayableComponent>);
	virtual void display() override;
protected:
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
};

//Decoration pattern for tasks
class DisplayableTask : public CalendarComponent {
	//friends
	friend class DisplayableDay;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	DisplayableTask(std::tm, std::shared_ptr<DisplayableComponent>, std::string);
	virtual void display() override;
	virtual std::string getName() override;
protected:
	std::string name;
	//Task is now the leaf node, no need to override addComponent
};

//Decorator method for DisplayableTask
class CompleteTask : public DisplayableTask {
	//friends
	friend class DisplayableDay;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	CompleteTask(std::tm, std::shared_ptr<DisplayableComponent>, std::string);
	virtual void display() override;
	virtual std::string getName() override;
	//Task is now the leaf node, no need to override addComponent
};
