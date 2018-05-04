
#ifndef CIRCLE_H
#define CIRCLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct circle_s
{
	double radius;

	double x;
	double y;
};

typedef struct circle_s circle;

struct circleList_s
{
	int noOfCircle;

	circle **list;
};

typedef struct circleList_s circleList;

circle *circle_new();
circleList *circleList_new(int noOfCircle);
circle *circle_getCircle(circleList *aCircleList, int circleID);
void circleList_print(circleList *aCircleList);

int circle_checkWhetherCircleCoverPoint(circle *aCircle, double x, double y);

#endif
