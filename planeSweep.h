
#ifndef PLANES_H
#define PLANE_H

#include <math.h>
#include "circle.h"
#include "qsort.h"
#include "config.h"

struct interval_s
{
	double left;
	double right;

	int weight;
};

typedef struct interval_s interval;


struct intervalList_s
{
	int noOfElt;

	interval **elt;
};

typedef struct intervalList_s intervalList;

struct fourPoint_s
{
	double x1;
	double x2;
	double x3;
	double x4;

	double y1;
	double y2;
	double y3;
	double y4;
};

typedef struct fourPoint_s fourPoint;

struct fourPointList_s
{
	int noOfElt;
	fourPoint **elt;
};

typedef struct fourPointList_s fourPointList;

fourPointList *findOptimalByPlaneSweep(int L, circleList *aCircleList);
extern mySetSet *filter_calculateGreatestNoOfIntersection(mySet *unionBoxSet, configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo);

//mySetSet *filter_findOptimalInNewDim(int L, circleList *aCircleList);
//mySetSet *filter_findOptimalInNewDim(int L,  configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo);
mySetSet *filter_findOptimalInNewDim(int L, circleList *rhombusList, configure *aConfig, mySetSet *overlapTable, node_type *rootC, int *sortedIndex, circleList *aCircleList, rtree_info *rtreeInfo);

#endif
