/*
 * Author: Ethan Zheng
 */


// lab5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CalendarInterface.h"
#include <iostream>

using namespace std;

//main function
int main()
{
	CalendarInterface *c = new CalendarInterface("full", 3); //Constructs calendar from 2018 for 3 years
	c->display();
    return 0;
}

