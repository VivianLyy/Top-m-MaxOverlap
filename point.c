
#include "point.h"

point *point_new(int noOfDim)
{
	point *returnValue;

	returnValue = (point *) malloc(sizeof(point));
	memset(returnValue, 0, sizeof(point));

	returnValue->content = (R_TYPE *) malloc(sizeof(R_TYPE)*noOfDim);

	return returnValue;
}

void point_free(point **aPoint)
{
	free((*aPoint)->content);
	free((*aPoint));
}

point *point_clone(point *aPoint, int noOfDim)
{
	point *returnValue;

	returnValue = point_new(noOfDim);

	memcpy(returnValue->content, aPoint->content, sizeof(R_TYPE)*noOfDim);
	returnValue->id = aPoint->id;
	returnValue->weight = aPoint->weight;

	return returnValue;
}


pointList *pointList_new()
{
	pointList *returnValue;

	returnValue = (pointList *) malloc(sizeof(pointList));
	memset(returnValue, 0, sizeof(pointList));

	return returnValue;
}

void pointList_free(pointList **aPointList)
{
	int noOfPoint;
	int i;
	point **list;
	
	noOfPoint = (*aPointList)->noOfPoint;
	list = (*aPointList)->list;
	if (noOfPoint > 0)
	{
		for (i = 0; i < noOfPoint; i++)
		{
			point_free(&(list[i]));
		}
		free(list);
	}

	free((*aPointList));
}

void pointList_insert(pointList *aPointList, point *aPoint)
{
	point **list;
	int noOfPoint;

	list = aPointList->list;
	noOfPoint = aPointList->noOfPoint;
	
	if (noOfPoint == 0)
	{
		list = (point **) malloc(sizeof(point *)*(noOfPoint+1));
	}
	else
	{
		list = (point **) realloc(list, sizeof(point *)*(noOfPoint+1));
	}
	list[noOfPoint] = aPoint;

	aPointList->list = list;
	aPointList->noOfPoint++;
}




void pointLIst_free(pointList *aPointList)
{
	int i;

	if (aPointList->noOfPoint > 0)
	{
		for (i = 0; i < aPointList->noOfPoint; i++)
		{
			point_free(&(aPointList->list[i]));
		}
		free(aPointList->list);
	}
	free(aPointList);
}


pointList *pointList_clone(pointList *aPointList, int noOfDim)
{
	pointList *returnValue;
	int noOfPoint;
	int i;

	point **newList;
	point **oldList;

	point *aNewListEntry;
	point *aOldListEntry;

	returnValue = pointList_new();

	//memcpy(returnValue, aPointList, sizeof(pointList));
	returnValue->noOfPoint = aPointList->noOfPoint;
	returnValue->noOfRemain = aPointList->noOfRemain;

	noOfPoint = returnValue->noOfPoint;
	if (noOfPoint > 0)
	{
		oldList = aPointList->list;

		newList = (point **) malloc(sizeof(point *)*noOfPoint);
		for (i = 0; i < noOfPoint; i++)
		{
			aNewListEntry = point_new(noOfDim);
			newList[i] = aNewListEntry;

			aOldListEntry = oldList[i];

			aNewListEntry->id = aOldListEntry->id;
			aNewListEntry->weight = aOldListEntry->weight;

			memcpy(aNewListEntry->content, aOldListEntry->content, sizeof(int)*noOfDim);

			
		}

		returnValue->list = newList;
	}
	else
	{
		returnValue->list = NULL;
	}


	return returnValue;
}

twoData *twoData_new()
{
	twoData *returnValue;

	returnValue = (twoData *) malloc(sizeof(twoData));
	memset(returnValue, 0, sizeof(twoData));

	return returnValue;
}


twoData *twoData_clone(twoData *aTwoData, int noOfDim)
{
	twoData *returnValue;

	returnValue = twoData_new();

	returnValue->O = pointList_clone(aTwoData->O, noOfDim);
	returnValue->P = pointList_clone(aTwoData->P, noOfDim);

	return returnValue;
}



point *twoData_getOPoint(twoData *aTwoData, int oid)
{
	point *returnValue;

	pointList *O;
	int noOfPoint;

	O = aTwoData->O;

	noOfPoint = O->noOfPoint;
	if ((0 <= oid) && (oid < noOfPoint))
	{
		returnValue = O->list[oid];		
	}
	else
	{
		returnValue = NULL;
	}

	return returnValue;
}



point *twoData_getPPoint(twoData *aTwoData, int pid)
{
	point *returnValue;

	pointList *O;
	pointList *P;
	int noOfPointInO;
	int noOfPointInP;

	O = aTwoData->O;
	P = aTwoData->P;

	noOfPointInO = O->noOfPoint;
	noOfPointInP = P->noOfPoint;
	if ((noOfPointInO <= pid) && (pid < noOfPointInO + noOfPointInP))
	{
		returnValue = P->list[pid - noOfPointInO];
	}
	else
	{
		returnValue = NULL;
	}

	return returnValue;
}

void twoData_decrementO(twoData *aTwoData)
{
	pointList *O;

	O = aTwoData->O;
	O->noOfRemain--;
}


void twoData_decrementP(twoData *aTwoData)
{
	pointList *P;

	P = aTwoData->P;
	P->noOfRemain--;
}

int twoData_oIsEmpty(twoData *aTwoData)
{
	int returnValue;
	pointList *O;

	O = aTwoData->O;
	if (O->noOfRemain == 0)
	{
		returnValue = 1;
	}
	else
	{
		returnValue = 0;
	}

	return returnValue;
}


int twoData_pIsEmpty(twoData *aTwoData)
{
	int returnValue;
	pointList *P;

	P = aTwoData->P;
	if (P->noOfRemain == 0)
	{
		returnValue = 1;
	}
	else
	{
		returnValue = 0;
	}

	return returnValue;
}


pointListList *pointListList_new()
{
	pointListList *returnValue;

	returnValue = (pointListList *) malloc(sizeof(pointListList));
	memset(returnValue, 0, sizeof(pointListList));

	return returnValue;
}


void pointListList_insert(pointListList *aPointListList, pointList *aPointList)
{
	int noOfElt;
	pointList **elt;

	noOfElt = aPointListList->noOfElt;
	elt = aPointListList->elt;

	if (noOfElt == 0)
	{
		elt = (pointList **) malloc(sizeof(pointList *));
	}
	else
	{
		elt = (pointList **) realloc(elt, sizeof(pointList *)*(noOfElt+1));
	}
	elt[noOfElt] = aPointList;

	aPointListList->noOfElt++;
	aPointListList->elt = elt;
}

void pointListList_free(pointListList *aPointListList)
{
	int i;

	if (aPointListList->noOfElt > 0)
	{
		for (i = 0; i < aPointListList->noOfElt; i++)
		{
			pointList_free(&(aPointListList->elt[i]));
		}
		free(aPointListList->elt);
	}
	
	free(aPointListList);
}
