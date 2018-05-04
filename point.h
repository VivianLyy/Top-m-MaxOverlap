
#ifndef POINT_H
#define POINT_H

#include "rtree.h"

struct point_s
{
	int id;
	int weight;
	R_TYPE *content;
};

typedef struct point_s point;

struct pointList_s
{
	int noOfPoint;
	int noOfRemain;

	point **list;
};

typedef struct pointList_s pointList;

struct pointListList_s
{
	int noOfElt;

	pointList **elt;
};

typedef struct pointListList_s pointListList;

struct twoData_s
{
	pointList *O;
	pointList *P;
};

typedef struct twoData_s twoData;

pointList *pointList_new();
point *point_new(int noOfDim);
point *point_clone(point *aPoint, int noOfDim);

twoData *twoData_new();



pointList *pointList_clone(pointList *aPointList, int noOfDim);
void pointList_insert(pointList *aPointList, point *aPoint);
void pointList_free(pointList **aPointList);

point *twoData_getOPoint(twoData *aTwoData, int oid);
point *twoData_getPPoint(twoData *aTwoData, int pid);

void twoData_decrementO(twoData *aTwoData);
void twoData_decrementP(twoData *aTwoData);

int twoData_oIsEmpty(twoData *aTwoData);
int twoData_pIsEmpty(twoData *aTwoData);

twoData *twoData_clone(twoData *aTwoData, int noOfDim);

pointListList *pointListList_new();
void pointListList_insert(pointListList *aPointListList, pointList *aPointList);
void pointListList_free(pointListList *aPointListList);

#endif

