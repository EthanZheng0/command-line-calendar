/*
Author: Jon Shidal, Ethan Zheng
Purpose: Definition of DisplayableComponent class. Defines the implementation that is
inherited by all concrete classes deriving from it.
*/
#include "stdafx.h"
#include "displayablecomponent.h"

using namespace std;

DisplayableComponent::DisplayableComponent(shared_ptr<DisplayableComponent> p) : parent(p) {}

// zero out all shared_ptr so resources are cleaned up
DisplayableComponent::~DisplayableComponent() {
	for (size_t i = 0; i < children.size(); ++i) {
		// parent is a weak_ptr, so no need to clear children's parent
		children[i] = nullptr;
	}
	children.clear();
}

// function for displayable day to add to-do lists
int DisplayableComponent::addTodolist(shared_ptr<DisplayableComponent> comp) {
	// do nothing as default
	return -1;
}

shared_ptr<DisplayableComponent> DisplayableComponent::addComponent(shared_ptr<DisplayableComponent> comp) {
	// do nothing as default, leaf nodes can't contain components.
	// Any composite classes that can contain components will override this
	return nullptr;
}
shared_ptr<DisplayableComponent> DisplayableComponent::removeComponent(unsigned int index) {
	// go ahead and define a default implementation here, this should be good enough for derived classes(leaf or composite).
	if (index < children.size()) { // leaf objects will have size of 0
		shared_ptr<DisplayableComponent> removed = children[index];
		children[index] = nullptr;
		return removed; // pointer to the removed component if successful
	}
	return nullptr; // nullptr is remove fails
}

weak_ptr<DisplayableComponent> DisplayableComponent::getParent() {
	return parent;
}

shared_ptr<DisplayableComponent> DisplayableComponent::getChild(unsigned int index) {
	// go ahead and define here, leaf classes with no children will return nullptr
	if (index < children.size()) {
		return children[index];
	}
	else {
		return nullptr; // nullptr if index is out of bounds
	}
}

//update the map for a calendar where all the pointers of events and their names are stored. Do nothing as default.
void DisplayableComponent::updateMap(std::shared_ptr<DisplayableComponent>, std::string, std::string) {}

//search the map for a vector of all the results. Do nothing as default.
std::vector<std::shared_ptr<DisplayableComponent>> DisplayableComponent::searchMap(std::string) {
	vector<std::shared_ptr<DisplayableComponent>> v;
	return v;
}
