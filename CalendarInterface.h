/*
Author: Jon Shidal, Ethan Zheng
Purpose: declare the user interface for our calendar
*/
#pragma once
#include "calendarbuilder.h"
#include "calendarcomponents.h"
#include <memory>


class CalendarInterface {
	std::shared_ptr<Calendar> cal; // the calendar
	std::shared_ptr<CalendarBuilder> builder; // builder for managing construction of the calendar
	std::shared_ptr<DisplayableComponent> currentDisplay; // which component the user is currently viewing
public:
	// constructor
	// arguments: 1 - what type of builder? 2 - how many years to hold?
	CalendarInterface(std::string builderType, size_t years);

	// calendar traversal functions
	void zoomIn(unsigned int index); // zoom in on a child of the current_display
	void zoomOut(); // zoom out to the parent of the current_display
	void display(); // display the current view to the user
	void jumpTo(); //jump to a specific date
	void addEvent(); // add an event
	void editEvent(); //delete an event when current_display is a day node
	void find(); //search for an event
	void todo(); //add a To-do list
	void task(); //add a Task in the To-do list
	void complete(); //mark a Task as complete
	void save(); //save the calendar to a .txt file
	void restore(); //restore a calendar from a .txt file
	void merge(); //merge a calendar into the current calendar
};
