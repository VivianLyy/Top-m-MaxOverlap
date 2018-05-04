
#include "config.h"

#define MAXSTR 10000


void readDataSet(pointList *aPointList, int noOfPoint, int noOfDim, int offset, char *filename)
{
	FILE *filePtr;
	int i, j;

	point **list;
	point *aPoint;
	R_TYPE *content;

	filePtr = (FILE *) fopen(filename, "r");
	if (filePtr == NULL)
	{
		printf("File %s does not exist!\n", filename);
		exit(0);
	}

	aPointList->noOfPoint = noOfPoint;
	aPointList->noOfRemain = noOfPoint;

	list = (point **) malloc(sizeof(point *)*noOfPoint);
	aPointList->list = list;
	for (i = 0; i < noOfPoint; i++)
	{
		aPoint = point_new(noOfDim);
		list[i] = aPoint;

		content = aPoint->content;

		// id
		aPoint->id = offset + i;

		// weight
		fscanf(filePtr, "%d\n", &(aPoint->weight));

		// content
		for (j = 0; j < noOfDim; j++)
		{
#ifdef R_FLOAT
			fscanf(filePtr, "%f ", &(content[j]));
#else
			fscanf(filePtr, "%d ", &(content[j]));
#endif
		}

	}

	fclose(filePtr);
}

configure *config_new()
{
	configure *returnValue;

	returnValue = (configure *) malloc(sizeof(configure));
	memset(returnValue, 0, sizeof(configure));

	returnValue->O = pointList_new();
	returnValue->P = pointList_new();
	returnValue->Q = pointList_new();

	returnValue->oFilename = (char *) malloc(sizeof(char)*MAXSTR);
	returnValue->pFilename = (char *) malloc(sizeof(char)*MAXSTR);
	returnValue->qFilename = (char *) malloc(sizeof(char)*MAXSTR);

	returnValue->rtreeConf = rtreeConf_new();
	returnValue->rtreeInfo = rtreeInfo_new();

	return returnValue;
}

configure *readConfig(char *filename)
{
	configure *returnValue;
	FILE *filePtr;

	int noOfOPoint;
	int noOfPPoint;
	int noOfQPoint;

	config_type *rtreeConf;
	rtree_info *rtreeInfo;


	int m, M, noOfDim, reinsert_p, noOfData;
	char *dataFilename;
	char *queryFilename;

	int offset;

	returnValue = config_new();

	rtreeConf = returnValue->rtreeConf;
	rtreeInfo = returnValue->rtreeInfo;

	dataFilename = (char *) malloc(sizeof(char)*MAXSTR);
	queryFilename = (char *) malloc(sizeof(char)*MAXSTR);

	filePtr = (FILE *) fopen(filename, "r");
	if (filePtr == NULL)
	{
		printf("File %s does not exist!\n", filename);
		exit(0);
	}

	fscanf(filePtr, "%d\n", &noOfOPoint);		// no. of o points
	fscanf(filePtr, "%d\n", &noOfPPoint);		// no. of p points
	fscanf(filePtr, "%d\n", &noOfQPoint);		// no. of q points

	fscanf(filePtr, "%s\n", returnValue->oFilename);	// data file for O
	fscanf(filePtr, "%s\n", returnValue->pFilename);	// data file for P
	fscanf(filePtr, "%s\n", returnValue->qFilename);	// data file for Q

	fscanf(filePtr, "%d\n", &(returnValue->k));	// k
	fscanf(filePtr, "%d\n", &(returnValue->l));	// l
	fscanf(filePtr, "%d\n", &(returnValue->ALG));		// alg no.

	fscanf(filePtr, "\n"); // ~~ blank line ~~

	fscanf(filePtr, "%d\n", &m);			// m
	fscanf(filePtr, "%d\n", &M);			// M
	fscanf(filePtr, "%d\n", &noOfDim);		// no. of Dim
	fscanf(filePtr, "%d\n", &reinsert_p);	// reinsert_p
	fscanf(filePtr, "%d\n", &noOfData);		// no_histogram (dummy)
	fscanf(filePtr, "%s\n", dataFilename);	// data file (dummy)
	fscanf(filePtr, "%s\n", queryFilename);	// query file (dummy)

	// put to rtreeConfig
	rtreeConf->m = m;
	rtreeConf->M = M;
	rtreeConf->dim = noOfDim;
	rtreeConf->reinsert_p = reinsert_p;
	rtreeConf->no_histogram = noOfData;
	strcpy(rtreeConf->positionfile, dataFilename);
	strcpy(rtreeConf->queryfile, queryFilename);

	// put to rtreeInfo
	rtreeInfo->m = m;
	rtreeInfo->M = M;
	rtreeInfo->dim = noOfDim;
	rtreeInfo->reinsert_p = reinsert_p;
	rtreeInfo->no_histogram = noOfData;
	rtreeInfo->extra_level = 0;

	// put to O
	offset = 0;
	readDataSet(returnValue->O, noOfOPoint, noOfDim, offset, returnValue->oFilename);

	// put to P
	offset = noOfOPoint;
	readDataSet(returnValue->P, noOfPPoint, noOfDim, offset, returnValue->pFilename);

	// put to Q
	// offset = noOfOPoint + noOfPPoint;
	offset = 0;
	readDataSet(returnValue->Q, noOfQPoint, noOfDim, offset, returnValue->qFilename);
	

	fclose(filePtr);

	free(dataFilename);
	free(queryFilename);

	return returnValue;
}
