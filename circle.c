
#include "circle.h"



circle *circle_new()
{
	circle *returnValue;

	returnValue = (circle *) malloc(sizeof(circle));
	memset(returnValue, 0, sizeof(circle));

	return returnValue;
}

circleList *circleList_new(int noOfCircle)
{
	circleList *returnValue;

	returnValue = (circleList *) malloc(sizeof(circleList));
	memset(returnValue, 0, sizeof(circleList));
	returnValue->list = (circle **) malloc(sizeof(circle *)*noOfCircle);
	returnValue->noOfCircle = noOfCircle;

	return returnValue;
}

circle *circle_getCircle(circleList *aCircleList, int circleID)
{
	circle *returnValue;

	returnValue = aCircleList->list[circleID];

	return returnValue;
}

void circle_print(circle *aCircle)
{
	double x, y, radius;

	x = aCircle->x;
	y = aCircle->y;
	radius = aCircle->radius;

	printf("(%.5f, %.5f) with radius = %.5f\n", x, y, radius);
}

void circleList_print(circleList *aCircleList)
{
	circle **list;
	circle *aCircle;

	int i;

	int noOfCircle;

	noOfCircle = aCircleList->noOfCircle;
	list = aCircleList->list;

	for (i = 0; i < noOfCircle; i++)
	{
		aCircle = list[i];
		printf(" Circle %d \n", i);
		circle_print(aCircle);
	}
}


int circle_checkWhetherCircleCoverPoint(circle *aCircle, double x, double y)
{
	int returnValue;

	double centerX, centerY;
	double radius;

	double leftSide, rightSide;

	double temp1, temp2;

	centerX = aCircle->x;
	centerY = aCircle->y;
	radius = aCircle->radius;

	temp1 = centerX - x;
	temp2 = centerY - y;

	leftSide = temp1*temp1 + temp2*temp2;
	rightSide = radius*radius;

	if (leftSide <= rightSide)
	{
		returnValue = 1;
	}
	else
	{
		returnValue = 0;
	}

	return returnValue;
}
