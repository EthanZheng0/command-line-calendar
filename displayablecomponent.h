/*
Author: Jon Shidal, Ethan Zheng
Purpose: Declaration of DisplayableComponent. DisplayableComponent is the common base class of all displayable objects.
It declares and defines the interface that all objects in the composite share.
*/
#pragma once
#include<memory>
#include<vector>
#include<map>

// forward declaration, needed for friend declaration
class FullCalendarBuilder;
class CalendarInterface;

// abstract base class representing a composite object
class DisplayableComponent {
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	// defined by concrete base classes deriving from DisplayableComponent
	virtual void display() = 0;
	virtual void display(std::string) = 0;
	virtual tm getDateInfo() = 0;
	virtual std::string getName() = 0;
	virtual std::string getCalendarName() = 0;
	virtual std::vector<std::shared_ptr<DisplayableComponent>> getEvents() = 0;
	virtual std::map<std::shared_ptr<DisplayableComponent>, std::string> getMap() = 0;
	virtual std::shared_ptr<DisplayableComponent> getTodolist() = 0;
	virtual std::shared_ptr<DisplayableComponent> decorate(std::shared_ptr<DisplayableComponent>, std::string) = 0;
protected:
	// member variables
	std::vector<std::shared_ptr<DisplayableComponent> > children;
	std::weak_ptr<DisplayableComponent> parent; // weak_ptr to avoid cycles, does not contribute to reference count

	// construction, destruction
	DisplayableComponent(std::shared_ptr<DisplayableComponent>);
	virtual ~DisplayableComponent();

	// composite and traversal functions
	virtual std::weak_ptr<DisplayableComponent> getParent();
	virtual std::shared_ptr<DisplayableComponent> getChild(unsigned int);
	virtual int addTodolist(std::shared_ptr<DisplayableComponent>);
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>);
	virtual std::shared_ptr<DisplayableComponent> removeComponent(unsigned int);
	virtual void updateMap(std::shared_ptr<DisplayableComponent>, std::string, std::string);
	virtual std::vector<std::shared_ptr<DisplayableComponent>> searchMap(std::string);
};
