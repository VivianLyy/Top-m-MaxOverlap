
#include "planeSweep.h"

void findLowerLeftInNewDim(double orgX, double orgY, double radius, double *newX, double *newY)
{
	double rootTwo;

	rootTwo = sqrt(2);

	(*newX) =  ((orgX + orgY - radius)/rootTwo);
	(*newY) =  ((-1*orgX + orgY - radius)/rootTwo);
}

void findUpperRightInNewDim(double orgX, double orgY, double radius, double *newX, double *newY)
{
	double rootTwo;

	rootTwo = sqrt(2);

	(*newX) = ((orgX + orgY + radius)/rootTwo);
	(*newY) = ((-1*orgX + orgY + radius)/rootTwo);
}

interval *interval_new()
{
	interval *returnValue;

	returnValue = (interval *) malloc(sizeof(interval));
	memset(returnValue, 0, sizeof(interval));

	return returnValue;
}

void transformFromCircleToNewDimInterval(circle *aCircle, int weight, interval **intervalX, interval **intervalY, double scale)
{
	double orgX, orgY, radius;

	double lowerLeftX, lowerLeftY;
	double upperRightX, upperRightY;

	double scaleRadius;

	(*intervalX) = interval_new();
	(*intervalY) = interval_new();

	orgX = aCircle->x;
	orgY = aCircle->y;
	radius = aCircle->radius;

	scaleRadius = radius*scale;

	findLowerLeftInNewDim(orgX, orgY, scaleRadius, &lowerLeftX, &lowerLeftY);
	findUpperRightInNewDim(orgX, orgY, scaleRadius, &upperRightX, &upperRightY);

	(*intervalX)->left = lowerLeftX;
	(*intervalX)->right = upperRightX;
	(*intervalX)->weight = weight;

	(*intervalY)->left = lowerLeftY;
	(*intervalY)->right = upperRightY;
	(*intervalY)->weight = weight;

}

intervalList *intervalList_new(int noOfElt)
{
	intervalList *returnValue;

	returnValue = (intervalList *) malloc(sizeof(intervalList));
	memset(returnValue, 0, sizeof(intervalList));

	returnValue->elt = (interval **) malloc(sizeof(interval *)*noOfElt);
	returnValue->noOfElt = noOfElt;

	return returnValue;
}

void transformFromCircleListToNewDimIntervalList(circleList *aCircleList, intervalList **intervalListX_out, intervalList **intervalListY_out, double scale)
{
	intervalList *intervalListX;
	intervalList *intervalListY;


	int noOfCircle;
	circle **list;

	int i;

	int weight;

	circle *aCircle;

	interval *intervalX;
	interval *intervalY;

	noOfCircle = aCircleList->noOfCircle;
	list = aCircleList->list;

	intervalListX = intervalList_new(noOfCircle);
	intervalListY = intervalList_new(noOfCircle);

	for (i = 0; i < noOfCircle; i++)
	{
		aCircle = list[i];

		// assume the simplest case
		weight = 1;

		transformFromCircleToNewDimInterval(aCircle, weight, &intervalX, &intervalY, scale);

		intervalListX->elt[i] = intervalX;
		intervalListY->elt[i] = intervalY;
	}

	(*intervalListX_out) = intervalListX;
	(*intervalListY_out) = intervalListY;

}




void transformFromIntervalListToVector(intervalList *intervalList, double **value_out, int **type_out, int **objNo_out)
{
	double *value;
	int *type;
	int *objNo;

	int noOfEntry;

	int noOfCircle;
	interval **elt;
	interval *aInterval;

	int i;

	int index1, index2;

	noOfCircle = intervalList->noOfElt;
	elt = intervalList->elt;

	noOfEntry = noOfCircle*2;

	value = (double *) malloc(sizeof(double)*noOfEntry);
	type = (int *) malloc(sizeof(int)*noOfEntry);
	objNo = (int *) malloc(sizeof(int)*noOfEntry);

	for (i = 0; i < noOfCircle; i++)
	{
		index1 = i*2;
		index2 = i*2+1;

		aInterval = elt[i];

		value[index1] = aInterval->left;
		type[index1] = 0; // left
		objNo[index1] = i;

		value[index2] = aInterval->right;
		type[index2] = 1; // right
		objNo[index2] = i;

	}

	(*value_out) = value;
	(*type_out) = type;
	(*objNo_out) = objNo;
}

// assume weight = 1
// assume L = 1
intervalList *findTopLIntervalAxis(int L, double *value, int *type, int *objNo, int noOfCircle)
{
	intervalList *returnValue;

	int noOfEntry;

	interval *currInterval;
	interval *bestInterval;

	int influenceValue;
	int best_influenceValue;

	int weight;

	int i;

	int *index;
	int *index_reverse;
	double *new_value;
	double *new_value_reverse;

	int aIndex;

	returnValue = intervalList_new(L);

	for (i = 0; i < L; i++)
	{
		returnValue->elt[i] = interval_new();
	}

	noOfEntry = noOfCircle*2;

	currInterval = interval_new();

	index = (int *) malloc(sizeof(int)*noOfEntry);
	index_reverse = (int *) malloc(sizeof(int)*noOfEntry);
	new_value_reverse = (double *) malloc(sizeof(double)*noOfEntry);
	new_value = (double *) malloc(sizeof(double)*noOfEntry);
	for (i = 0; i < noOfEntry; i++)
	{
		index_reverse[i] = i;
		new_value_reverse[i] = value[i];
	}

	quicksort(index_reverse, new_value_reverse, 0, noOfEntry-1);

	for (i = 0; i < noOfEntry; i++)
	{
		new_value[i] = new_value_reverse[noOfEntry-i-1];
		index[i] = index_reverse[noOfEntry-i-1];
	}


	// assume the min. possible value is -10000000000
	currInterval->left = -10000000000.0;
	best_influenceValue = 0;
	influenceValue = 0;
	for (i = 0; i < noOfEntry; i++)
	{
		currInterval->right = new_value[i];

		aIndex = index[i];
		if (type[aIndex] == 0)
		{
			// left
			// simplify
			weight = 1;
			influenceValue += weight;
		}
		else
		{
			// right
			// simplify
			weight = 1;
			influenceValue -= weight;
		}

		if (influenceValue > best_influenceValue)
		{
			best_influenceValue = influenceValue;

			printf("   InfluenceValue : %d\n", influenceValue);

			// assume L is equal to 1
			bestInterval = returnValue->elt[0];

			bestInterval->left = currInterval->left;
			bestInterval->right = currInterval->right;
			bestInterval->weight = best_influenceValue;
		}

		// update
		currInterval->left = currInterval->right;
	}

	free(currInterval);

	free(index);
	free(index_reverse);
	free(new_value);
	free(new_value_reverse);

	return returnValue;
}

// assume weight = 1
// assume L = 1
mySetSet *filter_findTopLRegion(int L, double *value, int *type, int *objNo, int noOfCircle, configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo)
{
	mySetSet *returnValue;

	int noOfEntry;

	interval *currInterval;
	//interval *bestInterval;

	int influenceValue;
	int best_influenceValue;

	int weight;

	int i;

	int aObjNo;

	int *index;
	int *index_reverse;
	double *new_value;
	double *new_value_reverse;

	int aIndex;

	mySet *boxUnionSet;

	int realInfluenceValue;
	mySet *realIntersection;
	mySet *best_intersection;

	mySetSet *tempResultSetSet;
	mySet *bestIntersection;

	noOfEntry = noOfCircle*2;

	currInterval = interval_new();

	index = (int *) malloc(sizeof(int)*noOfEntry);
	index_reverse = (int *) malloc(sizeof(int)*noOfEntry);
	new_value_reverse = (double *) malloc(sizeof(double)*noOfEntry);
	new_value = (double *) malloc(sizeof(double)*noOfEntry);
	for (i = 0; i < noOfEntry; i++)
	{
		index_reverse[i] = i;
		new_value_reverse[i] = value[i];
	}

	quicksort(index_reverse, new_value_reverse, 0, noOfEntry-1);

	for (i = 0; i < noOfEntry; i++)
	{
		new_value[i] = new_value_reverse[noOfEntry-i-1];
		index[i] = index_reverse[noOfEntry-i-1];
	}

	boxUnionSet = mySet_new();
	best_intersection = mySet_new();

	// assume the min. possible value is -10000000000
	currInterval->left = -10000000000.0;
	best_influenceValue = 0;
	influenceValue = 0;
	for (i = 0; i < noOfEntry; i++)
	{
		currInterval->right = new_value[i];

		aIndex = index[i];

		aObjNo = objNo[aIndex];
		if (type[aIndex] == 0)
		{
			// left
			// simplify
			weight = 1;
			influenceValue += weight;

			mySet_insert_sorted(boxUnionSet, aObjNo);
		}
		else
		{
			// right
			// simplify
			weight = 1;
			influenceValue -= weight;

			mySet_removeElt(boxUnionSet, aObjNo);
		}

	
		if (influenceValue > best_influenceValue)
		{
			//
			rtreeInfo->norm = 2;

			tempResultSetSet = filter_calculateGreatestNoOfIntersection(boxUnionSet, aConfig, overlapTable, rootC, sortedIndex, aCircleList, rtreeInfo);
			// assume top 1 result
			realIntersection = tempResultSetSet->elt[0];
			// assume w(c) = 1 for all c
			realInfluenceValue = realIntersection->noOfElt;
			if (realInfluenceValue > best_influenceValue)
			{
				best_influenceValue = realInfluenceValue;

				printf("   RealInfluenceValue : %d\n", realInfluenceValue);

				mySet_free(&best_intersection);
				best_intersection = realIntersection;
			}
			else
			{
				mySet_free(&realIntersection);
			}
		}

		// update
		currInterval->left = currInterval->right;
	}

	free(currInterval);

	free(index);
	free(index_reverse);
	free(new_value);
	free(new_value_reverse);

	// assume one element
	returnValue = mySetSet_new(1);
	returnValue->elt[0] = best_intersection;

	return returnValue;
}

fourPoint *fourPoint_new()
{
	fourPoint *returnValue;

	returnValue = (fourPoint *) malloc(sizeof(fourPoint));
	memset(returnValue, 0, sizeof(fourPoint));

	return returnValue;
}

fourPointList *fourPointList_new(int noOfElt)
{
	fourPointList *returnValue;

	int i;

	returnValue = (fourPointList *) malloc(sizeof(fourPointList));
	returnValue->noOfElt = noOfElt;
	returnValue->elt = (fourPoint **) malloc(sizeof(fourPoint *)*noOfElt);
	for (i = 0; i < noOfElt; i++)
	{
		returnValue->elt[i] = fourPoint_new();
	}

	return returnValue;
}

fourPointList *findOptimalInNewDim(int L, circleList *aCircleList)
{
	fourPointList *returnValue;

	fourPoint *aFourPoint;

	int noOfCircle;

	double *valueX;
	double *valueY;

	int *typeX;
	int *typeY;

	int *objNoX;
	int *objNoY;

	double scale;

	intervalList *intervalListX;
	intervalList *intervalListY;

	interval *intervalX;
	interval *intervalY;

	noOfCircle = aCircleList->noOfCircle;

	scale = 1.0;
	transformFromCircleListToNewDimIntervalList(aCircleList, &intervalListX, &intervalListY, scale);

	transformFromIntervalListToVector(intervalListX, &valueX, &typeX, &objNoX);
	transformFromIntervalListToVector(intervalListY, &valueY, &typeY, &objNoY);

	intervalListX = findTopLIntervalAxis(L, valueX, typeX, objNoX, noOfCircle);
	intervalListY = findTopLIntervalAxis(L, valueY, typeY, objNoY, noOfCircle);

	
	returnValue = fourPointList_new(L);

	// assume L = 1
	aFourPoint = returnValue->elt[0];
	intervalX = intervalListX->elt[0];
	intervalY = intervalListY->elt[0];

	aFourPoint->x1 = intervalX->left;
	aFourPoint->y1 = intervalY->left; 

	aFourPoint->x2 = intervalX->left;
	aFourPoint->y2 = intervalY->right; 

	aFourPoint->x3 = intervalX->right;
	aFourPoint->y3 = intervalY->left; 

	aFourPoint->x4 = intervalX->right;
	aFourPoint->y4 = intervalY->right; 

	free(valueX);
	free(valueY);

	free(typeX);
	free(typeY);

	free(objNoX);
	free(objNoY);

	return returnValue;
}


mySetSet *filter_findOptimalInNewDim(int L, circleList *rhombusList, configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo)
{
	mySetSet *returnValue;

	//fourPoint *aFourPoint;

	int noOfCircle;

	double *valueX;
	double *valueY;

	int *typeX;
	int *typeY;

	int *objNoX;
	int *objNoY;

	double scale;

	intervalList *intervalListX;
	intervalList *intervalListY;

	//interval *intervalX;
	//interval *intervalY;

	noOfCircle = rhombusList->noOfCircle;

	scale = sqrt(2);
	//scale = 10;
	transformFromCircleListToNewDimIntervalList(rhombusList, &intervalListX, &intervalListY, scale);

	transformFromIntervalListToVector(intervalListX, &valueX, &typeX, &objNoX);
	transformFromIntervalListToVector(intervalListY, &valueY, &typeY, &objNoY);

	//intervalListX = findTopLIntervalAxis(L, valueX, typeX, objNoX, noOfCircle);
	//intervalListY = findTopLIntervalAxis(L, valueY, typeY, objNoY, noOfCircle);

	// assume L = 1
	returnValue = filter_findTopLRegion(L, valueX, typeX, objNoX, noOfCircle, aConfig, overlapTable, rootC, sortedIndex, aCircleList, rtreeInfo);


	free(valueX);
	free(valueY);

	free(typeX);
	free(typeY);

	free(objNoX);
	free(objNoY);

	return returnValue;
}


void transformBackToOrgDim(double newX, double newY, double *orgX, double *orgY)
{
	double sqrtTwo;

	sqrtTwo = sqrt(2);

	(*orgX) = (newX - newY)/sqrtTwo;
	(*orgY) = (newX + newY)/sqrtTwo;
}

fourPointList *findOptimalByPlaneSweep(int L, circleList *aCircleList)
{
	fourPointList *returnValue;
	fourPointList *fourPointListNewDim;

	fourPoint *newFourPoint;
	fourPoint *orgFourPoint;

	returnValue = fourPointList_new(L);
	
	fourPointListNewDim = findOptimalInNewDim(L, aCircleList);

	// assume L is equal to 1
	orgFourPoint = returnValue->elt[0];
	newFourPoint = fourPointListNewDim->elt[0];

	transformBackToOrgDim(newFourPoint->x1, newFourPoint->y1, &(orgFourPoint->x1), &(orgFourPoint->y1));
	transformBackToOrgDim(newFourPoint->x2, newFourPoint->y2, &(orgFourPoint->x2), &(orgFourPoint->y2));
	transformBackToOrgDim(newFourPoint->x3, newFourPoint->y3, &(orgFourPoint->x3), &(orgFourPoint->y3));
	transformBackToOrgDim(newFourPoint->x4, newFourPoint->y4, &(orgFourPoint->x4), &(orgFourPoint->y4));

	// free fourPointListNewDim
	// to be done

	return returnValue;
}

