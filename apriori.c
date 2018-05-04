
#include "apriori.h"

pointList *transformTwoPointToPointList(double x1, double y1, double x2, double y2)
{
	pointList *returnValue;

	point *aPoint1;
	point *aPoint2;

	returnValue = pointList_new();

	returnValue->noOfPoint = 2;
	returnValue->list = (point **) malloc(sizeof(point *)*2);

	aPoint1 = point_new(2);
	aPoint2 = point_new(2);

	aPoint1->content[0] = (R_TYPE) x1;
	aPoint1->content[1] = (R_TYPE) y1;

	aPoint2->content[0] = (R_TYPE) x2;
	aPoint2->content[1] = (R_TYPE) y2;

	returnValue->list[0] = aPoint1;
	returnValue->list[1] = aPoint2;

	return returnValue;
}

mySetSet *generateSizeTwoCandidate(mySetSet *overlapTable, pointListList **intersectSet, circleList *aCircleList)
{
	mySetSet *returnValue;

	int noOfCircle;
	mySet **overlapWholeList;
	mySet *oneOverlapList;

	int noOfOverlapCircle;
	int *overlapCircleList;

	int i, j;

	int overlapCircleID;
	int circleID;

	mySet *oneCircleSet;

	double x1, x2, y1, y2;

	pointList *aPointList;

	circle *aCircle;
	circle *overlapCircle;

	returnValue = mySetSet_new(0);

	noOfCircle = overlapTable->noOfElt;
	overlapWholeList = overlapTable->elt;

	(*intersectSet) = pointListList_new();

	for (i = 0; i < noOfCircle; i++)
	{
		if (i%100 == 0)
		{
			printf("  Processing %d out of %d...\n", i, noOfCircle);
		}

		circleID = i;
		oneOverlapList = overlapWholeList[i];

		aCircle = circle_getCircle(aCircleList, circleID);

		noOfOverlapCircle = oneOverlapList->noOfElt;
		overlapCircleList = oneOverlapList->elt;
		for (j = 0; j < noOfOverlapCircle; j++)
		{
			overlapCircleID = overlapCircleList[j];

			overlapCircle = circle_getCircle(aCircleList, overlapCircleID);

			if (circleID < overlapCircleID)
			{
				oneCircleSet = mySet_new();

				oneCircleSet->noOfElt = 2;
				oneCircleSet->elt = (int *) malloc(sizeof(int)*2);
				oneCircleSet->elt[0] = circleID;
				oneCircleSet->elt[1] = overlapCircleID;

				mySetSet_insert(returnValue, oneCircleSet);

				// calculate intersection pts
				inter_computeIntersectionTwoCirlce(aCircle, overlapCircle, &x1, &y1, &x2, &y2);

				aPointList = transformTwoPointToPointList(x1, y1, x2, y2);
				pointListList_insert((*intersectSet), aPointList);
			}
		}
	}


	return returnValue;
}

// assume firstCAndidate and secondCandidate are sorted
mySet *generateCommonSizeMinusOne(mySet *firstCandidate, mySet *secondCandidate, int *diffOne, int *diffTwo)
{
	mySet *returnValue;

	int i;
	int size;

	int *elt;

	int isExist;

	int noOfDiff;

	int oneElt;

	size = firstCandidate->noOfElt;

	elt = firstCandidate->elt;

	noOfDiff = 0;
	returnValue = mySet_new();
	for (i = 0; i < size; i++)
	{
		oneElt = elt[i];

		isExist = mySet_isExist(secondCandidate, oneElt);
		if (isExist == 1)
		{
			mySet_insert(returnValue, oneElt);	
		}
		else
		{
			(*diffOne) = oneElt;
			noOfDiff++;
		}

		if (noOfDiff > 1)
		{
			break;
		}
	}
	
	if (noOfDiff == 1)
	{
		elt = secondCandidate->elt;

		for (i = 0; i < size; i++)
		{
			oneElt = elt[i];

			isExist = mySet_isExist(firstCandidate, oneElt);
			if (isExist == 1)
			{
				//mySet_insert(returnValule, oneElt);	
			}
			else
			{
				(*diffTwo) = oneElt;
				break;
			}
		}
	}

	if (noOfDiff > 1)
	{
		mySet_free(&returnValue);
		returnValue = NULL;
	}
	
	return returnValue;
}

// assume firstCAndidate and secondCandidate are sorted
mySet *mergeTwoCandidate(mySet *firstCandidate, mySet *secondCandidate, int *indexNew)
{
	mySet *returnValue;

	int diffOne, diffTwo;

	mySet *commonSetSizeMinusOne;

	commonSetSizeMinusOne = generateCommonSizeMinusOne(firstCandidate, secondCandidate, &diffOne, &diffTwo);
	if (commonSetSizeMinusOne != NULL)
	{
		returnValue = commonSetSizeMinusOne;
		mySet_insert_sorted(commonSetSizeMinusOne, diffOne);
		mySet_insert_sorted(commonSetSizeMinusOne, diffTwo);

		(*indexNew) = mySet_findIndex(commonSetSizeMinusOne, diffTwo);
	}
	else
	{
		returnValue = NULL;
	}

	return returnValue;
}

pointList *getRealIntersect_forIntersectPoint(mySet *circleSet, pointList *aPointList, circleList *aCircleList, int circleID, int noOfDim)
{
	pointList *returnValue;

	int i;
	point *aPoint;

	circle *aCircle;

	point *aClonePoint;

	double x, y;

	int isCover;

	returnValue = pointList_new();

	aCircle = circle_getCircle(aCircleList, circleID);

	//
	for (i = 0; i < aPointList->noOfPoint; i++)
	{
		aPoint = aPointList->list[i];

		x = aPoint->content[0];
		y = aPoint->content[1];

		isCover = circle_checkWhetherCircleCoverPoint(aCircle, x, y);

		if (isCover == 1)
		{
			aClonePoint = point_clone(aPoint, noOfDim);
			pointList_insert(returnValue, aClonePoint);
		}
	}

	//

	return returnValue;
}

int isCoverByAllCircle(double x, double y, mySet *circleSet, circleList *aCircleList)
{
	int returnValue;

	circle *aCircle;
	int circleID;

	int i;

	int isCover;

	returnValue = 1;
	for (i = 0; i < circleSet->noOfElt; i++)
	{
		circleID = circleSet->elt[i];

		aCircle = circle_getCircle(aCircleList, circleID);

		isCover = circle_checkWhetherCircleCoverPoint(aCircle, x, y);
		if (isCover == 0)
		{
			returnValue = 0;
			break;
		}
	}

	return returnValue;
}

pointList *getRealIntersect_forNewCut(mySet *circleSet, circleList *aCircleList, int circleID, int noOfDim)
{
	pointList *returnValue;

	int i;
	point *aPoint;

	circle *aCircle;

	int isCover;

	circle *setOneCircle;

	int setOneCircleID;

	double x1, x2, y1, y2;

	returnValue = pointList_new();

	aCircle = circle_getCircle(aCircleList, circleID);


	for (i = 0; i < circleSet->noOfElt; i++)
	{
		setOneCircleID = circleSet->elt[i];

		setOneCircle = circle_getCircle(aCircleList, setOneCircleID);

		// find the cut poitns between setOneCircle and aCircle
		inter_computeIntersectionTwoCirlce(setOneCircle, aCircle, &x1, &y1, &x2, &y2);

		// Intersection pt 1
		// check whether the cut points are covered by all circles in circleSet
		isCover = isCoverByAllCircle(x1, y1, circleSet, aCircleList);

		// if yes 
		// put in the set
		if (isCover == 1)
		{
			aPoint = point_new(noOfDim);
			aPoint->content[0] = (R_TYPE) x1;
			aPoint->content[1] = (R_TYPE) y1;
			pointList_insert(returnValue, aPoint);
		}

		// Intersection pt 2
		// check whether the cut points are covered by all circles in circleSet
		isCover = isCoverByAllCircle(x2, y2, circleSet, aCircleList);

		// if yes 
		// put in the set
		if (isCover == 1)
		{
			aPoint = point_new(noOfDim);
			aPoint->content[0] = (R_TYPE) x2;
			aPoint->content[1] = (R_TYPE) y2;
			pointList_insert(returnValue, aPoint);
		}
	}

	return returnValue;
}

pointList *getRealIntersect(mySet *circleSet, circleList *aCircleList, int circleID, int noOfDim, pointList *intersectPointSet)
{
	pointList *pointSet_existing;
	pointList *pointSet_new;
	pointList *returnValue;

	pointSet_existing = getRealIntersect_forIntersectPoint(circleSet, intersectPointSet, aCircleList, circleID, noOfDim);
	pointSet_new = getRealIntersect_forNewCut(circleSet, aCircleList, circleID, noOfDim);

	returnValue = pointList_new();
	returnValue->noOfPoint = pointSet_existing->noOfPoint + pointSet_new->noOfPoint;
	returnValue->list = (point **) malloc(sizeof(point *)*returnValue->noOfPoint);
	if (pointSet_existing->noOfPoint > 0)
	{
		memcpy(returnValue->list, pointSet_existing->list, sizeof(point *)*pointSet_existing->noOfPoint);
		
	}
	if (pointSet_new->noOfPoint > 0)
	{
		memcpy(&(returnValue->list[pointSet_existing->noOfPoint]), pointSet_new->list, sizeof(point *)*pointSet_new->noOfPoint);
	
	}

	if (pointSet_existing->noOfPoint > 0)
	{
		free(pointSet_existing->list);
		free(pointSet_existing);
	}
	if (pointSet_new->noOfPoint > 0)
	{
		free(pointSet_new->list);
		free(pointSet_new);
	}

	return returnValue;
}

mySetSet *generateSizeGreaterThanTwoCandidate(mySetSet *candidateSet, int size, pointListList *intersectSet, pointListList **newIntersectSet_out, circleList *aCircleList, int noOfDim)
{
	mySetSet *returnValue;

	int noOfCandidate;
	mySet **candidateList;

	mySet *firstCandidate;
	mySet *secondCandidate;

	int i, j;

	int isExist;

	mySet *mergeCandidate;

	pointListList *newIntersectSet;

	int diffOne, diffTwo;

	pointList **intersectList;

	mySet *commonSetSizeMinusOne;
	pointList *finalIntersectList;
	pointList *firstIntersectList;

	returnValue = mySetSet_new(0);
	newIntersectSet = pointListList_new();
	(*newIntersectSet_out) = newIntersectSet;

	noOfCandidate = candidateSet->noOfElt;
	candidateList = candidateSet->elt;

	intersectList = intersectSet->elt;

	for (i = 0; i < noOfCandidate; i++)
	{
		if (i%100 == 0)
		{
			printf("   Processing %d out of %d...\n", i, noOfCandidate);
		}
		firstCandidate = candidateList[i];
		firstIntersectList = intersectList[i];
		for (j = i+1; j < noOfCandidate; j++)
		{
			secondCandidate = candidateList[j];

			commonSetSizeMinusOne = generateCommonSizeMinusOne(firstCandidate, secondCandidate, &diffOne, &diffTwo);
			if (commonSetSizeMinusOne != NULL)
			{


				mergeCandidate = commonSetSizeMinusOne;
				mySet_insert_sorted(mergeCandidate, diffOne);
				mySet_insert_sorted(mergeCandidate, diffTwo);


				isExist = mySetSet_isExist(returnValue, mergeCandidate);
				if (isExist == 1)
				{
					mySet_free(&mergeCandidate);
				}
				else
				{
					finalIntersectList = getRealIntersect(firstCandidate, aCircleList, diffTwo, noOfDim, firstIntersectList);
					if (finalIntersectList->noOfPoint != 0)
					{
						mySetSet_insert(returnValue, mergeCandidate);
						pointListList_insert(newIntersectSet, finalIntersectList);
					}
					else
					{
						mySet_free(&mergeCandidate);
						pointList_free(&finalIntersectList);
					}
				}
			}
		}
	}

	return returnValue;
}


mySetSet *apriori_mainStep(configure *config, mySetSet *overlapTable, circleList *aCircleList)
{
	mySetSet *returnValue;

	mySetSet *candidateSet;
	mySetSet *newCandidateSet;

	mySetSet **tempCandidateSet;
	int size;

	int count;
	int index;

	int noOfDim;

	mySet *circleSet;

	pointListList *intersectSet;
	pointListList *newIntersectSet;

	returnValue = mySetSet_new(0);

	noOfDim = config->rtreeInfo->dim;

	candidateSet = generateSizeTwoCandidate(overlapTable, &intersectSet, aCircleList);
	size = 2;

	tempCandidateSet = (mySetSet **) malloc(sizeof(mySetSet *)*overlapTable->noOfElt*2);
	memset(tempCandidateSet, 0, sizeof(mySetSet *)*overlapTable->noOfElt*2);
	tempCandidateSet[size] = candidateSet;

	//statistics
	statistics_updateCandidateSize(globalStat, candidateSet, size, intersectSet);

	while (candidateSet->noOfElt > 0)
	{
		printf("Processing size %d...\n", size);
		newCandidateSet = generateSizeGreaterThanTwoCandidate(candidateSet, size, intersectSet, &newIntersectSet, aCircleList, noOfDim);
		size++;

		tempCandidateSet[size] = newCandidateSet;

		if (newCandidateSet->noOfElt > config->l)
		{
			mySetSet_free(candidateSet);
			pointListList_free(intersectSet);
		}

		candidateSet = newCandidateSet;
		intersectSet = newIntersectSet;

		//statistics
		statistics_updateCandidateSize(globalStat, candidateSet, size, intersectSet);
	}

	size--;

	count = 0; 
	index = 0;
	while (count < config->l)
	{
		if (index < tempCandidateSet[size]->noOfElt)
		{
			circleSet = tempCandidateSet[size]->elt[index];
			index++;

			mySetSet_insert(returnValue, circleSet);
			count++;
		}
		else
		{
			size--;
			index = 0;
		}
	}

	free(tempCandidateSet);

	return returnValue;
}


void apriori_main(configure *aConfig)
{
	rtree_info *rtreeInfo;

	node_type *rootP;
	pointList *pointListP;

	pointList *pointListO;

	node_type *rootC;

	twoData *aTwoData;

	int noOfDim;

	circleList *aCircleList;

	mySetSet *overlapTable;

	//int *sortedIndex;

	mySetSet *resultSet;

	FILE *debugFilePtr;

	char *debugFilename = "debug.txt";
	

#ifndef WIN32
	char *timeFilename = "time.txt";
	FILE *timeFilePtr;

	float userTime_RPConstruction, sysTime_RPConstruction;
	float userTime_CircleConstruction, sysTime_CircleConstruction;
	float userTime_RCConstruction, sysTime_RCConstruction;
	float userTime_OverlapTableConstruction, sysTime_OverlapTableConstruction;
	float userTime_initialization, sysTime_initialization;
	float userTime_entrySorting, sysTime_entrySorting;
	float userTime_iterativeStep, sysTime_iterativeStep;

	float userTime_allTime, sysTime_allTime;

	struct rusage myTime_start;
	struct rusage myTime_RPConstruction_end;
	struct rusage myTime_CircleConstruction_end;
	struct rusage myTime_RCConstruction_end;
	struct rusage myTime_OverlapTableConstruction_end;
	struct rusage myTime_initialization_end;
	struct rusage myTime_entrySorting_end;
	struct rusage myTime_iterativeStep_end;
#endif

	debugFilePtr = (FILE *) fopen(debugFilename, "w");
	if (debugFilePtr == NULL)
	{
		printf("File %s cannot be opened!\n", debugFilename);
		exit(0);
	}

	rtreeInfo = aConfig->rtreeInfo;
	rtreeInfo->norm = 2;

	pointListP = aConfig->P;
	pointListO = aConfig->O;

	noOfDim = rtreeInfo->dim;

	aTwoData = twoData_new();
	aTwoData->O = pointList_clone(aConfig->O, noOfDim);
	aTwoData->P = pointList_clone(aConfig->P, noOfDim);

	globalStat = statistics_new();

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_start);	
#endif	

	// Step 1: R_P Construction
	printf("Step 1: R_P Construction...\n");
	rootP = maxRNN_buildRtree(pointListP, rtreeInfo);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_RPConstruction_end);	
#endif	

	// Step 2: Circle Construction
	printf("Step 2: Circle Construction...\n");
	aCircleList = constructCircle(rootP, pointListO, rtreeInfo, aConfig->k, aTwoData);

	// print the circleList
	//circleList_print(aCircleList);

	// statistics
	statistics_rtreeNoOfNodeStorageRootP(globalStat, rootP, rtreeInfo);
	// free
	node_free(rootP, rtreeInfo);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_CircleConstruction_end);	
#endif	

	// Step 3: R_C Construction
	printf("Step 3: R_C Construction...\n");
	rootC = maxRNN_buildRtree_circle(aCircleList, rtreeInfo);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_RCConstruction_end);	
#endif	

	// Step 4: Overlapping Table Construction
	printf("Step 4: Overlapping Table Construction...\n");
	overlapTable = constructOverlapTable(aCircleList, rootC, rtreeInfo);

	// printOverlapTable
	//printOverlapTable(overlapTable);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_OverlapTableConstruction_end);	
#endif	

	// Step 5: Initialization
	printf("Step 5: Initialization...\n");
	// done in Step 7

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_initialization_end);	
#endif	

	// Step 6: Entry Sorting
	printf("Step 6: Entry Sorting...\n");
	//sortedIndex = sortOverlapTable(overlapTable);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_entrySorting_end);	
#endif	

	// Step 7: Iterative Step
	printf("Step 7: Iterative Step...\n");
	//resultSet = maxRNN_iterativeStep(aConfig, overlapTable, rootC, sortedIndex, aCircleList, rtreeInfo);
	resultSet = apriori_mainStep(aConfig, overlapTable, aCircleList);

	// print resultSet
	printf("The Result set is:\n");
	printResultSet(resultSet);

	//
	statistics_influenceValueStatistics(globalStat, resultSet);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_iterativeStep_end);	
#endif	

	
	statistics_rtreeNoOfNodeStorageRootC(globalStat, rootC, rtreeInfo);
	statistics_overlapTableStatistics(globalStat, overlapTable);
	statistics_totalStorage(aConfig, globalStat);


	statistics_print(globalStat, debugFilePtr);

	fclose(debugFilePtr);

	

#ifndef WIN32

	timeFilePtr = (FILE *) fopen(timeFilename, "w");
	if (timeFilePtr == NULL)
	{
		printf("File %s cannot be written!\n", timeFilename);
		exit(0);
	}


	calculateExecutionTime(&(myTime_start), &(myTime_RPConstruction_end), &userTime_RPConstruction, &sysTime_RPConstruction);
	calculateExecutionTime(&(myTime_RPConstruction_end), &(myTime_CircleConstruction_end), &userTime_CircleConstruction, &sysTime_CircleConstruction);
	calculateExecutionTime(&(myTime_CircleConstruction_end), &(myTime_RCConstruction_end), &userTime_RCConstruction, &sysTime_RCConstruction);
	calculateExecutionTime(&(myTime_RCConstruction_end), &(myTime_OverlapTableConstruction_end), &userTime_OverlapTableConstruction, &sysTime_OverlapTableConstruction);
	calculateExecutionTime(&(myTime_OverlapTableConstruction_end), &(myTime_initialization_end), &userTime_initialization, &sysTime_initialization);
	calculateExecutionTime(&(myTime_initialization_end), &(myTime_entrySorting_end), &userTime_entrySorting, &sysTime_entrySorting);
	calculateExecutionTime(&(myTime_entrySorting_end), &(myTime_iterativeStep_end), &userTime_iterativeStep, &sysTime_iterativeStep);
	
	calculateExecutionTime(&(myTime_start), &(myTime_iterativeStep_end), &userTime_allTime, &sysTime_allTime);

	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_RPConstruction, sysTime_RPConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_CircleConstruction, sysTime_CircleConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_RCConstruction, sysTime_RCConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_OverlapTableConstruction, sysTime_OverlapTableConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_initialization, sysTime_initialization);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_entrySorting, sysTime_entrySorting);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_iterativeStep, sysTime_iterativeStep);

	fprintf(timeFilePtr, "\n");

	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_allTime, sysTime_allTime);

#endif

}

