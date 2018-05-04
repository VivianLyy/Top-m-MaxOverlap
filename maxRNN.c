
#include "maxRNN.h"
#include "apriori.h"
#include "planeSweep.h"
#include "set.h"
#include <math.h>


#ifndef WIN32
void calculateExecutionTime(struct rusage *myTimeStart, struct rusage *myTimeEnd, float *userTime, float *sysTime)
{
	(*userTime) =
		((float) (myTimeEnd->ru_utime.tv_sec  - myTimeStart->ru_utime.tv_sec)) +
		((float) (myTimeEnd->ru_utime.tv_usec - myTimeStart->ru_utime.tv_usec)) * 1e-6;
	(*sysTime) =
		((float) (myTimeEnd->ru_stime.tv_sec  - myTimeStart->ru_stime.tv_sec)) +
		((float) (myTimeEnd->ru_stime.tv_usec - myTimeStart->ru_stime.tv_usec)) * 1e-6;
	//ru_utime：返回进程在用户模式下的执行时间，以timeval结构的形式返回（该结构体在bits/timeval中声明）。 
    //ru_stime：返回进程在内核模式下的执行时间，以timeval结构的形式返回（该结构体在bits/timeval中声明）。
	
}
#endif

node_type *maxRNN_buildRtree(pointList *aPointList, rtree_info *aInfo)
{ 
	node_type *returnValue;
	int i, j;

	int noOfPoint;
	point **pointList;
	point *aPoint;

	int pointID;

	R_TYPE *content;
	
	/* make tree root node first */
	
	tree_node_allocate(&returnValue, aInfo);
	
	for(i=0; i < aInfo->dim; i++) { 
		returnValue->b[i] = (R_TYPE)(-1 * INT_MAX);
       	returnValue->a[i] = (R_TYPE)(INT_MAX);
	}
	returnValue->id = -3;		//NO_ID
	returnValue->attribute=ROOT;
	returnValue->vacancy=aInfo->M;   
	returnValue->parent=NULL; 
	for(j=0; j < aInfo->M; j++)
		returnValue->ptr[j]=NULL;
	

	noOfPoint = aPointList->noOfPoint;
	pointList = aPointList->list;
	
	/* add data to the tree */
	for(i=0; i< noOfPoint; i++) 
	{ 		
		aPoint = pointList[i];

		pointID = aPoint->id;
		content = aPoint->content;

#ifdef DEBUG
		if (i+1%1000==0)
			printf("    insert data %d\n", pointID);
#endif
		
		insert_node(returnValue, content, pointID, aInfo);  
	}

	return returnValue;
} 

void transformCircleToMBR(circle *aCircle, R_TYPE *a, R_TYPE *b, rtree_info *aInfo)
{
	double x, y, radius;

	x = aCircle->x;
	y = aCircle->y;
	radius = aCircle->radius;

	// assume two dimensionality
	a[0] = (R_TYPE) (x - radius);
	a[1] = (R_TYPE) (y - radius);

	b[0] = (R_TYPE) (x + radius);
	b[1] = (R_TYPE) (y + radius);
}

node_type *maxRNN_buildRtree_circle(circleList *aCircleList, rtree_info *aInfo)
{ 
	node_type *returnValue;
	int i, j;

	int noOfCircle;

	int pointID;

	circle *aCircle;
	circle **circleArray;

	R_TYPE *a;
	R_TYPE *b;
	
	/* make tree root node first */
	
	tree_node_allocate(&returnValue, aInfo);
	
	for(i=0; i < aInfo->dim; i++) { 
		returnValue->b[i] = (R_TYPE)(-1 * INT_MAX);
       	returnValue->a[i] = (R_TYPE)(INT_MAX);
	}
	returnValue->id = -3;		//NO_ID
	returnValue->attribute=ROOT;
	returnValue->vacancy=aInfo->M;   
	returnValue->parent=NULL; 
	for(j=0; j < aInfo->M; j++)
		returnValue->ptr[j]=NULL;
	

	noOfCircle = aCircleList->noOfCircle;
	circleArray = aCircleList->list;
	
	/* add data to the tree */
	for(i=0; i< noOfCircle; i++) 
	{ 		
		aCircle = circleArray[i];

		pointID = i;//aPoint->id;

#ifdef DEBUG
		if (i+1%1000==0)
			printf("    insert data %d\n", pointID);
#endif
		a = (R_TYPE *) malloc(sizeof(R_TYPE)*aInfo->dim);
		b = (R_TYPE *) malloc(sizeof(R_TYPE)*aInfo->dim);
		memset(a, 0, sizeof(R_TYPE)*aInfo->dim);
		memset(b, 0, sizeof(R_TYPE)*aInfo->dim);

		transformCircleToMBR(aCircle, a, b, aInfo);
		
		insert_node_MBR(returnValue, a, b, pointID, aInfo);  

		free(a);
		free(b);
	}

	return returnValue;
} 

double calculateDistance(point *aPoint1, point *aPoint2, rtree_info *rtreeInfo)
{
	double returnValue;

	int i;

	R_TYPE *content1;
	R_TYPE *content2;

	double temp, temp2;
	
	content1 = aPoint1->content;
	content2 = aPoint2->content;

	returnValue = 0;
	for (i = 0; i < rtreeInfo->dim; i++)
	{
		temp = content1[i] - content2[i];

		temp2 = temp*temp;

		returnValue += temp2;
	}

	returnValue = sqrt(returnValue);

	return returnValue;
}

double calculateManhattanDistance(point *aPoint1, point *aPoint2, rtree_info *rtreeInfo)
{
	double returnValue;

	int i;

	R_TYPE *content1;
	R_TYPE *content2;

	double temp;
	
	content1 = aPoint1->content;
	content2 = aPoint2->content;

	returnValue = 0;
	for (i = 0; i < rtreeInfo->dim; i++)
	{
		if (content1[i] > content2[i])
		{
			temp = content1[i] - content2[i];
		}
		else
		{
			temp = content2[i] - content1[i];
		}

		returnValue += temp;
	}

	//returnValue = sqrt(returnValue);

	return returnValue;
}


circleList *constructCircle(node_type *rootP, pointList *pointListO, rtree_info *rtreeInfo, int k, twoData *aTwoData)
{
	circleList *returnValue;

	point **listO;
	int noOfPointO;

	point *oPoint;
	point *pPoint;

	R_TYPE *query;

	NN_type *Presult;

	double distance;

	circle **circleList;
	circle *aCircle;

	int i;

	int pid;

	noOfPointO = pointListO->noOfPoint;

	listO = pointListO->list;

	returnValue = circleList_new(noOfPointO);

	circleList = returnValue->list;

	for (i = 0; i < noOfPointO; i++)
	{
		oPoint = listO[i];

		// set the query point o
		query = oPoint->content;
// printf("  第 %d 个 oPoint，ID is %d, weight is %d, content is %f)\n", i, oPoint->id, oPoint->weight,oPoint->content);
		// find NN of o
		k_NN_search(rootP, query, k, &Presult, rtreeInfo);

		// k-NN
		pid = Presult->oid;

		pPoint = twoData_getPPoint(aTwoData, pid);

		// calculate the distance between pid and oPoint
		if (rtreeInfo->norm == 2)
		{
			distance = calculateDistance(oPoint, pPoint, rtreeInfo);
		}
		else
		{		
			distance = calculateManhattanDistance(oPoint, pPoint, rtreeInfo);
		}


		// for two dimensionality
		aCircle = circle_new();
		aCircle->radius = distance;
		aCircle->x = oPoint->content[0];
		aCircle->y = oPoint->content[1];

		circleList[i] = aCircle;
	}

	return returnValue;
}


mySetSet *constructOverlapTable(circleList *aCircleList, node_type *rootC, rtree_info *aInfo)
{
	mySetSet *returnValue;

	circle **circleArray;
	circle *aCircle;

	int noOfCircle;

	R_TYPE *query;

	int i;

	mySet *overlapSet;

	noOfCircle = aCircleList->noOfCircle;
	circleArray = aCircleList->list;

	query = (R_TYPE *) malloc(sizeof(R_TYPE)*aInfo->dim);

	returnValue = mySetSet_new(noOfCircle);

	for (i = 0; i < noOfCircle; i++)
	{
		aCircle = circleArray[i];

		// assume two dimensionality
		query[0] = (R_TYPE) aCircle->x;
		query[1] = (R_TYPE) aCircle->y;


		overlapSet = mySet_new();
		range_search(overlapSet, rootC, query, (R_TYPE) aCircle->radius, aInfo);

		mySet_removeElt(overlapSet, i);
		returnValue->elt[i] = overlapSet;
	}

	free(query);

	return returnValue;
}

void printOverlapTable(mySetSet *overlapTable)
{
	int noOfElt;
	mySet **elt;
	mySet *aSet;

	int i;

	noOfElt = overlapTable->noOfElt; //row
	elt = overlapTable->elt;  

	printf(" ----------- Overlap Table -------------\n");
	for (i = 0; i < noOfElt; i++)
	{
		aSet = elt[i];

		printf("%d : ", i);
		mySet_print(aSet);
		printf("\n");
	}
	printf(" ---------------------------------------\n");
}

int *sortOverlapTable(mySetSet *overlapTable)
{
	int *returnValue;

	int noOfCircle;
	mySet **entryList;

	mySet *aSet;
	
	int i;

	int *value;

	noOfCircle = overlapTable->noOfElt;
	entryList = overlapTable->elt;

	returnValue = (int *) malloc(sizeof(int)*noOfCircle);
	memset(returnValue, 0, sizeof(int)*noOfCircle);
	for (i = 0; i < noOfCircle; i++)
	{
		returnValue[i] = i;
	}

	value = (int *) malloc(sizeof(int)*noOfCircle);
	memset(value, 0, sizeof(int)*noOfCircle);
	for (i = 0; i < noOfCircle; i++)
	{
		aSet = entryList[i];
		value[i] = aSet->noOfElt;
	}

	quicksort_i(returnValue, value, 0, noOfCircle-1);


	return returnValue;
}

mySetSet *iterativeStep_forOverlapTable(configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo, int *isValid, int noOfValid)
{
	mySetSet *returnValue;
	int *isProcess;

	int noOfCircle;
	int noOfRemain;

	int index;

	int circleID;

	mySet *overlapSet;

	int overlapCircleID;

	double x1, y1, x2, y2;

	circle *overlapCircle;
	circle *aCircle;

	mySet **overlapArray;

	R_TYPE *query;

	mySet *S;

	mySet *S_o;

	int I_o;

	int i;

	int l;

	int N;

	int arr_index = 0;

	int *startRandom;
	int tempIndex;

	l = aConfig->l;

	returnValue = mySetSet_new(l);

	// find the first valid
	tempIndex = 0;
	startRandom = (int *) malloc(sizeof(int)*l);
	memset(startRandom, 0, sizeof(int)*l);
	// assume that there are l valid circles
	for (i = 0; i < overlapTable->noOfElt; i++)
	{
		if (isValid[i] == 1)
		{
			startRandom[tempIndex] = i;

			tempIndex++;

			if (tempIndex >= l)
			{
				break;
			}
		}
	}

	// initialization
	// assume that no. of circles >= l
	for (i = 0; i < l; i++)
	{
		S_o = mySet_new();
		// put point id = i to S_o for initialization
		//mySet_insert(S_o, i);
		// put point id = startRandom[i] to S_o for initialization
		mySet_insert(S_o, startRandom[i]);

		returnValue->elt[i] = S_o;
	}
	I_o = returnValue->elt[l-1]->noOfElt;

	free(startRandom);

	//R_TYPE *query;
	query = (R_TYPE *) malloc(sizeof(R_TYPE)*rtreeInfo->dim);

	noOfCircle = overlapTable->noOfElt;
	N = noOfCircle * (noOfCircle-1)+100;
	printf(" N= %d \n", N);
	//declare arr
	// interPoint_query arr [N];
	interPoint_query *arr;
	arr = (interPoint_query *)malloc(sizeof(interPoint_query)*N);
	interPoint_query IQ;
// for(int i = 0; i < N; i++)
// {
//   arr[i] = (interPoint_query*)malloc(sizeof(interPoint_query));
// }

	isProcess = (int *) malloc(sizeof(int)*noOfCircle);
	memset(isProcess, 0, sizeof(int)*noOfCircle);

	overlapArray = overlapTable->elt;

	noOfRemain = noOfCircle;
	index = 0;
	while (noOfRemain > 0)
	{

		circleID = sortedIndex[index]; //remove an entry (c, L) with the largest value of W(L) from T
		noOfRemain--;

		if (isValid[circleID] == 1)
		{

			aCircle = circle_getCircle(aCircleList, circleID);

			overlapSet = overlapArray[circleID];//L  overlapSet 拿到与circleID相交的circlelist

			// printf(" overlapSet->noOfElt = %d, I_o = %d\n", overlapSet->noOfElt, I_o);

			if (aConfig->ALG == 1)
			{
				if (overlapSet->noOfElt < I_o - 1)
				{
					break;
				}
			}

			// printf(" Processing circle %d...\n", circleID);

			// statistics
			globalStat->noOfCircleProcessed++;

			if (index%100 == 0)
			{
				printf("  Processing %d-th element (size %d, I_o %d)\n", index, overlapSet->noOfElt, I_o);
			}

			for (i = 0; i < overlapSet->noOfElt; i++)
			{
							

				overlapCircleID = overlapSet->elt[i];

				overlapCircle = circle_getCircle(aCircleList, overlapCircleID);

				if (isProcess[overlapCircleID] == 0)
				{
					// compute the interesection pt between overlapCircleID and circleID
					inter_computeIntersectionTwoCirlce(overlapCircle, aCircle, &x1, &y1, &x2, &y2);

					// statistics
					globalStat->noOfIntersection++;

					// for each intersection pt q  一共两个
					//   perform a point query from q over R_C to find all circles covering q and obtain S

					// interesection pt 1
					// assume two dimensionality
					query[0] = (R_TYPE) x1;
					query[1] = (R_TYPE) y1;

					// point query
					S = mySet_new();
					range_search(S, rootC, query, 0, rtreeInfo);

					mySet_sort(S);

					// printf(" mySet_sort(S)\n");

					if(S->noOfElt){
						// arr[arr_index++] = interPoint_query_new(S->noOfElt, S);
						IQ = interPoint_query_new(S->noOfElt, S);
						
						arr[arr_index++] = IQ;
						
					}
					// printf("  Processing %d-th element in arr(the element size is %d,compare %d,)\n", arr_index, arr[arr_index-1].key, S->noOfElt);
					// mySet_print(arr[arr_index-1].S);
					// printf("\n");

					if (l==1){
						if (S->noOfElt > I_o)
						{
						// update
							mySetSet_updateSortDescendingOrder(returnValue, S);

							I_o = returnValue->elt[l-1]->noOfElt;
						}
						else
						{
							mySet_free(&S);
						}
					}

					// interesection pt 2
					// assume two dimensionality
					query[0] = (R_TYPE) x2;
					query[1] = (R_TYPE) y2;

					// point query
					S = mySet_new();
					range_search(S, rootC, query, 0, rtreeInfo);

					mySet_sort(S);

					if(S->noOfElt){
						// arr[arr_index++] = interPoint_query_new(S->noOfElt, S);
						IQ.key = S->noOfElt;
						IQ.S = S;
						arr[arr_index++] = IQ;
					}
					// printf("  Processing %d-th element in arr(the element size is %d,compare %d,)\n", arr_index, arr[arr_index-1].key, S->noOfElt);
					// mySet_print(arr[arr_index-1].S);
					// printf("\n");

				//printf("  S->noOfElt = %d, I_o = %d\n", S->noOfElt, I_o);
					if (l==1){
						if (S->noOfElt > I_o)
						{   // update
							mySetSet_updateSortDescendingOrder(returnValue, S);

							I_o = returnValue->elt[l-1]->noOfElt;
						}
						else
						{
							mySet_free(&S);
						}
					}
					
				}
			}
		}

		index++;
	}//T遍历完,arr 初始化完成
	int k;
	if (l>1){

		for (k =0; k<l; k++){
			int idx = findMax(arr,arr_index);
			// returnValue->elt[k] = arr[idx]->S;
			// printf("idx: %d \n", idx );
			mySetSet_updateSortDescendingOrder(returnValue, arr[idx].S);
			arr[idx].key = 0;
			int i;
			for (i = 0; i < arr_index; i++) {
				if (i == idx || arr[i].key == 0) continue;
				arr[i] = interPoint_query_update(arr[i], arr[idx].S);
				// printf("更新后arr的第%d 个对象，对象包含%d个元素,分别是 \n", i ,arr[i].key);
				// mySet_print(arr[i].S);
				// printf("\n");
			}
		}

	}

	free(query);
	free(arr); 
	return returnValue;
}

int findMax(interPoint_query arr[], int n){
	int Max = 0, idx = -1,i = 0;
	for (i = 0; i < n; i++){
		if (arr[i].key > Max){
			Max = arr[i].key;
			idx = i;
		}
	}
	return idx;
}


mySetSet *maxRNN_iterativeStep(configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo)
{
	mySetSet *returnValue;

	int *isValid;
	int i;

	isValid = (int *) malloc(sizeof(int)*overlapTable->noOfElt);
	for (i = 0; i < overlapTable->noOfElt; i++)
	{
		isValid[i] = 1;
	}

	returnValue = iterativeStep_forOverlapTable(aConfig, overlapTable, rootC, sortedIndex, aCircleList, rtreeInfo, isValid, overlapTable->noOfElt);

	free(isValid);

	return returnValue;
}

mySetSet *filter_calculateGreatestNoOfIntersection(mySet *unionBoxSet, configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo)
{
	mySetSet *returnValue;

	int *isValid;
	int i;

	int index;

	isValid = (int *) malloc(sizeof(int)*overlapTable->noOfElt);
	memset(isValid, 0, sizeof(int)*overlapTable->noOfElt);
	for (i = 0; i < unionBoxSet->noOfElt; i++)
	{
		index = unionBoxSet->elt[i];
		isValid[index] = 1;
	}

	//**** RAY
	//for (i = 0; i < overlapTable->noOfElt; i++)
	//{
	//	isValid[i] = 1;
	//}

	returnValue = iterativeStep_forOverlapTable(aConfig, overlapTable, rootC, sortedIndex, aCircleList, rtreeInfo, isValid, overlapTable->noOfElt);

	free(isValid);

	return returnValue;
}

void printResultSet(mySetSet *resultSet)
{
	int noOfElt;
	mySet **elt;

	mySet *aSet;

	int i;

	noOfElt = resultSet->noOfElt;
	elt = resultSet->elt;

	for (i = 0; i < noOfElt; i++)
	{
		aSet = elt[i];
		printf("  Rank %d (value %d): ", i+1, aSet->noOfElt);
		mySet_print(aSet);
		printf("\n");
	}
}

mySetSet *selectQueryPoint(pointList *pointListQ, mySetSet *resultSet, circleList *aCircleList, rtree_info *rtreeInfo, configure *aConfig)
{
	printf(" 进入函数selectQueryPoint ");
	mySetSet *returnValue;
	int noOfElt;
	mySet **elt;

	mySet *aSet;
	mySet *S_o;

	int i;
	int k;

	noOfElt = resultSet->noOfElt;
	elt = resultSet->elt;

	point *pointItem;
	int noOfPoint;
	R_TYPE *content;

	noOfPoint = pointListQ->noOfPoint;

	int l = aConfig->l;

	returnValue = mySetSet_new(l);

	for (i = 0; i < l; i++)
	{
		S_o = mySet_new();
		// put point id = i to S_o for initialization
		// mySet_insert(S_o, i);
		returnValue->elt[i] = S_o;
	}

	for (k = 0; k < noOfPoint; k++){
		pointItem = pointListQ->list[k];
		// content = pointItem->content;
		for (i = 0; i < noOfElt; i++)
		{
			aSet = elt[i];
			// printf("  Rank %d (value %d): ", i+1, aSet->noOfElt);
			 // printf("第%d 个query点，属于第%d 个集合对应的区域\n",k,i);
			if(traverseAset(pointItem, aSet, aCircleList, rtreeInfo))//返回1 表示pointItem->id在Rank(i+1)区域内部
			{
				printf("第%d 个query点，属于第%d 个集合对应的区域\n",k,i);
				mySet_insert(returnValue->elt[i], pointItem->id);
			}
			// printf("\n");
		}
	}
	return returnValue;

}


int traverseAset(point *aPoint, mySet *aSet, circleList *aCircleList, rtree_info *rtreeInfo)
{
	// printf(" 进入函数traverseAset ");
	int returnvalue;
	int noOfElt;

	int *elt;
	int i = 0;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;

	circle **list;
	circle *aCircle;
	int circleNum;
	int noOfCircle;
	int pointID = aPoint->id;
	R_TYPE pointX = aPoint->content[0];
	R_TYPE pointY = aPoint->content[1];
// printf("pointItem id is %d \n",pointID);
// printf("point X is %f, Y is %f \n",pointX, pointY);
	noOfCircle = aCircleList->noOfCircle;
	list = aCircleList->list;
	
	do{
		circleNum = elt[i];
		// printf("circleNum is %d \n",circleNum);
		aCircle = list[circleNum];
		returnvalue = compare(aPoint, aCircle, rtreeInfo); //返回1 表示在内部
		i++;
	}while (returnvalue && i < noOfElt);

	if(returnvalue){
			printf("pointItem id is %d \n",pointID);
			printf("point X is %f, Y is %f \n",pointX, pointY);
		}
	return returnvalue;
}


int compare(point *aPoint, circle *aCircle, rtree_info *rtreeInfo)
{
	int returnvalue = 0;

	double x, y, radius;
	x = aCircle->x;
	y = aCircle->y;
	radius = aCircle->radius;

	double tempValue = 0;
	double temp1 = fabs(aPoint->content[0] - x);
	double temp2 = fabs(aPoint->content[1] - y);
	tempValue = temp1*temp1 + temp2*temp2;
	tempValue = sqrt(tempValue);	
	// printf("circle的 x :%f y:%f radius:%f \n",x,y,radius);
	// printf("point的 x :%f y:%f temp1: %f temp2: %f tempValue:%f \n",aPoint->content[0],aPoint->content[1],temp1,temp2,tempValue);
	if(tempValue <= radius){
		returnvalue = 1;
	}
	return returnvalue;
	// printf("(%.5f, %.5f) with radius = %.5f\n", x, y, radius);
}

void maxRNN_main(configure *aConfig)
{
	rtree_info *rtreeInfo;

	node_type *rootP;

	pointList *pointListP;
	pointList *pointListO;
	pointList *pointListQ;

	node_type *rootC;

	twoData *aTwoData;

	int noOfDim;

	circleList *aCircleList;

	mySetSet *overlapTable;

	int *sortedIndex;

	mySetSet *resultSet;
	mySetSet *QueryResult;

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
	float userTime_querySelection, sysTime_querySelection;

	float userTime_allTime, sysTime_allTime;

	struct rusage myTime_start;
	struct rusage myTime_RPConstruction_end;
	struct rusage myTime_CircleConstruction_end;
	struct rusage myTime_RCConstruction_end;
	struct rusage myTime_OverlapTableConstruction_end;
	struct rusage myTime_initialization_end;
	struct rusage myTime_entrySorting_end;
	struct rusage myTime_iterativeStep_end;
	struct rusage myTime_querySelection_end;
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
	pointListQ = aConfig->Q;

	noOfDim = rtreeInfo->dim;

	aTwoData = twoData_new();
	aTwoData->O = pointList_clone(aConfig->O, noOfDim);
	aTwoData->P = pointList_clone(aConfig->P, noOfDim);

	globalStat = statistics_new();

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_start);	
#endif	

	// Step 1: R_P Construction  为P点集合构建rtree
	printf("Step 1: R_P Construction...\n");
	rootP = maxRNN_buildRtree(pointListP, rtreeInfo);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_RPConstruction_end);	
#endif	

	// Step 2: Circle Construction
	printf("Step 2: Circle Construction...\n");
	aCircleList = constructCircle(rootP, pointListO, rtreeInfo, aConfig->k, aTwoData);

	// print the circleList‘
	printf("print the circleList...\n");
	circleList_print(aCircleList);

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
	printf("printOverlapTable...\n");
	printOverlapTable(overlapTable);

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
	sortedIndex = sortOverlapTable(overlapTable);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_entrySorting_end);	
#endif	

	// Step 7: Iterative Step
	printf("Step 7: Iterative Step...\n");
	resultSet = maxRNN_iterativeStep(aConfig, overlapTable, rootC, sortedIndex, aCircleList, rtreeInfo);

	// print resultSet
	printf("The Result set is:\n");
	printResultSet(resultSet);

	//
	statistics_influenceValueStatistics(globalStat, resultSet);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_iterativeStep_end);	
#endif	

//Query point Selection
	printf("Step 8: Query point Selection...\n");
	QueryResult = selectQueryPoint(pointListQ, resultSet, aCircleList, rtreeInfo, aConfig);
	printf("The Query point Selection Result set is:\n");
	printResultSet(QueryResult);


#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_querySelection_end);	
#endif	
//Query point Selection

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
	calculateExecutionTime(&(myTime_iterativeStep_end), &(myTime_querySelection_end), &userTime_querySelection, &sysTime_querySelection);

	calculateExecutionTime(&(myTime_start), &(myTime_querySelection_end), &userTime_allTime, &sysTime_allTime);

	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_RPConstruction, sysTime_RPConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_CircleConstruction, sysTime_CircleConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_RCConstruction, sysTime_RCConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_OverlapTableConstruction, sysTime_OverlapTableConstruction);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_initialization, sysTime_initialization);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_entrySorting, sysTime_entrySorting);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_iterativeStep, sysTime_iterativeStep);
	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_querySelection, sysTime_querySelection);

	fprintf(timeFilePtr, "\n");

	fprintf(timeFilePtr, "%.10f\t%.10f\n", userTime_allTime, sysTime_allTime);

#endif

}

float uniform(float _min, float _max)
{
	int int_r = rand();
	long base = RAND_MAX-1;
	float f_r  = ((float) int_r) / base;
	return (_max - _min) * f_r + _min;
}

float new_uniform_one(int _d_num)
{
	int i;
	float base=1;
	float sum=0; 
	int digit;

	for (i=0; i<_d_num; i++)
	{
		digit= (int) uniform(0, 10);
		if (digit==10) digit=9;
		sum+=base*digit;
		base*=10;
	}
	return sum;
}

float new_uniform(float _min, float _max)
{
	float ran_base=9999999;
	float ran=new_uniform_one(7);
	return ran/ran_base*(_max-_min)+_min;
}

double rangeRandom(double min, double max)
{
	double returnValue;

	returnValue = (min*1000+(max-min)*1000/rand())/1000.0;

	return returnValue;
}

double planeSweep_calculateAvgInfluenceValue(fourPointList *aFourPointList, node_type *rootC, rtree_info *rtreeInfo)
{
	double returnValue;

	double influenceValue;

	fourPoint *aFourPoint;

	mySet *S;

	int i;
	int sampleSize;

	R_TYPE *query;

	query = (R_TYPE *) malloc(sizeof(R_TYPE)*2);

	// assume only L = 1
	aFourPoint = aFourPointList->elt[0];

	influenceValue = 0;

	// Point 1
	query[0] = (R_TYPE) aFourPoint->x1;
	query[1] = (R_TYPE) aFourPoint->y1;

	// point query
	S = mySet_new();
	range_search(S, rootC, query, 0, rtreeInfo);

	influenceValue += S->noOfElt;

	printf("  Point 1 (%.2f, %.2f): %d\n", query[0], query[1], S->noOfElt);

	mySet_free(&S);

	

	// Point 2
	query[0] = (R_TYPE) aFourPoint->x2;
	query[1] = (R_TYPE) aFourPoint->y2;

	// point query
	S = mySet_new();
	range_search(S, rootC, query, 0, rtreeInfo);

	influenceValue += S->noOfElt;

	printf("  Point 2 (%.2f, %.2f): %d\n", query[0], query[1], S->noOfElt);


	mySet_free(&S);



	// Point 3
	query[0] = (R_TYPE) aFourPoint->x3;
	query[1] = (R_TYPE) aFourPoint->y3;

	// point query
	S = mySet_new();
	range_search(S, rootC, query, 0, rtreeInfo);

	influenceValue += S->noOfElt;

	printf("  Point 3 (%.2f, %.2f): %d\n", query[0], query[1], S->noOfElt);


	mySet_free(&S);


	// Point 4
	query[0] = (R_TYPE) aFourPoint->x4;
	query[1] = (R_TYPE) aFourPoint->y4;

	// point query
	S = mySet_new();
	range_search(S, rootC, query, 0, rtreeInfo);

	influenceValue += S->noOfElt;

	printf("  Point 4 (%.2f, %.2f): %d\n", query[0], query[1], S->noOfElt);


	mySet_free(&S);

	// Point 5
	query[0] = (R_TYPE) ((aFourPoint->x1 + aFourPoint->x2 + aFourPoint->x3 + aFourPoint->x4)/4.0);
	query[1] = (R_TYPE) ((aFourPoint->y1 + aFourPoint->y2 + aFourPoint->y3 + aFourPoint->y4)/4.0);

	// point query
	S = mySet_new();
	range_search(S, rootC, query, 0, rtreeInfo);

	influenceValue += S->noOfElt;

	printf("  Point 5 (%.2f, %.2f): %d\n", query[0], query[1], S->noOfElt);


	mySet_free(&S);


	// 
	sampleSize = 1000;

	for (i = 0; i < sampleSize - 5; i++)
	{
		// Point i

		query[0] = (R_TYPE) rangeRandom(aFourPoint->x1, aFourPoint->x4);
		query[1] = (R_TYPE) rangeRandom(aFourPoint->y1, aFourPoint->y4);

		// point query
		S = mySet_new();
		range_search(S, rootC, query, 0, rtreeInfo);

		influenceValue += S->noOfElt;

		printf("  Point %d (%.2f, %.2f): %d\n", i+6, query[0], query[1], S->noOfElt);


		mySet_free(&S);
	}

	returnValue = influenceValue/((double)sampleSize);

	free(query);

	
	return returnValue;
}



void planeSweep_main(configure *aConfig)
{
	rtree_info *rtreeInfo;

	node_type *rootP;
	pointList *pointListP;

	pointList *pointListO;

	node_type *rootC;

	twoData *aTwoData;

	int noOfDim;

	circleList *aCircleList;
	circleList *aRhombusList;

	FILE *debugFilePtr;

	char *debugFilename = "debug.txt";

	fourPointList *aFourPointList;

	double avgValue;
	

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
	//node_free(rootP, rtreeInfo);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_CircleConstruction_end);	
#endif	

	// Step 3: R_C Construction
	printf("Step 3: R_C Construction...\n");
	rootC = maxRNN_buildRtree_circle(aCircleList, rtreeInfo);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_RCConstruction_end);	
#endif	


	//*** START here

	rtreeInfo->norm = 1;
	printf("Step 4: Rhombus Construction...\n");
	aRhombusList = constructCircle(rootP, pointListO, rtreeInfo, aConfig->k, aTwoData);

	// free
	node_free(rootP, rtreeInfo);

	aFourPointList = findOptimalByPlaneSweep(aConfig->l, aRhombusList);

	avgValue = planeSweep_calculateAvgInfluenceValue(aFourPointList, rootC, rtreeInfo);


	printf("Average influence value = %.10f\n", avgValue);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_OverlapTableConstruction_end);	
#endif	





#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_initialization_end);	
#endif	

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_entrySorting_end);	
#endif	

	// print resultSet
	//printf("The Result set is:\n");
	//printResultSet(resultSet);

	//
	//statistics_influenceValueStatistics(globalStat, resultSet);

	globalStat->max_InfluenceValue = avgValue;
	globalStat->min_InfluenceValue = avgValue;
	globalStat->avg_InfluenceValue = avgValue;


#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_iterativeStep_end);	
#endif	

	
	statistics_rtreeNoOfNodeStorageRootC(globalStat, rootC, rtreeInfo);
	//statistics_overlapTableStatistics(globalStat, overlapTable);
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


void filter_main(configure *aConfig)
{
	rtree_info *rtreeInfo;

	node_type *rootP;
	pointList *pointListP;

	pointList *pointListO;

	node_type *rootC;

	twoData *aTwoData;

	int noOfDim;

	circleList *aCircleList;
	circleList *aRhombusList;

	mySetSet *overlapTable;
	int *sortedIndex;

	FILE *debugFilePtr;

	char *debugFilename = "debug.txt";

	//fourPointList *aFourPointList;

	double avgValue;

	mySetSet *intersectResult;
	

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
	//node_free(rootP, rtreeInfo);

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
	sortedIndex = sortOverlapTable(overlapTable);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_entrySorting_end);	
#endif	

	//*** START here

	rtreeInfo->norm = 1;
	printf("Step 4: Rhombus Construction...\n");
	aRhombusList = constructCircle(rootP, pointListO, rtreeInfo, aConfig->k, aTwoData);

	// free
	node_free(rootP, rtreeInfo);

	intersectResult = filter_findOptimalInNewDim(aConfig->l, aRhombusList, aConfig, overlapTable, rootC, sortedIndex, aCircleList, rtreeInfo);

	//avgValue = planeSweep_calculateAvgInfluenceValue(aFourPointList, rootC, rtreeInfo);


	//printf("Average influence value = %.10f\n", avgValue);
	printf("Influence value = %d\n", intersectResult->elt[0]->noOfElt);

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_OverlapTableConstruction_end);	
#endif	





#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_initialization_end);	
#endif	

#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_entrySorting_end);	
#endif	

	// print resultSet
	//printf("The Result set is:\n");
	//printResultSet(resultSet);

	//
	//statistics_influenceValueStatistics(globalStat, resultSet);

	avgValue = intersectResult->elt[0]->noOfElt;

	globalStat->max_InfluenceValue = avgValue;
	globalStat->min_InfluenceValue = avgValue;
	globalStat->avg_InfluenceValue = avgValue;


#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime_iterativeStep_end);	
#endif	

	
	statistics_rtreeNoOfNodeStorageRootC(globalStat, rootC, rtreeInfo);
	//statistics_overlapTableStatistics(globalStat, overlapTable);
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


int main()
{
	char *configFilename = "conf.txt";

	configure *aConfig;

	aConfig = readConfig(configFilename);

	if ((aConfig->ALG == 1) || (aConfig->ALG == 2))
	{
		maxRNN_main(aConfig);
	}
	else if (aConfig->ALG == 3)
	{
		apriori_main(aConfig);
	}
	else if (aConfig->ALG == 4)
	{
		planeSweep_main(aConfig);
	}
	else if (aConfig->ALG == 5)
	{
		filter_main(aConfig);
	}


	return 0;
}


