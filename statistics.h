
#ifndef STAT_H
#define STAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "set.h"


struct statistics_s
{
	// overlapTable
	double averageOverlapLength;
	int overlapTableSize;

	// pruning
	int noOfCircleProcessed;
	int noOfIntersection;

	// Rtree
	int noOfNodeRtreeP;
	int rtreeP_storage;
	int noOfNodeRtreeC;
	int rtreeC_storage;

	// totalStorage
	int totalStorage;

	// influence value
	double max_InfluenceValue;
	double avg_InfluenceValue;
	double min_InfluenceValue;

	// apriori
	int totalNoOfCandidate;
	int totalCandidateSize;
	int totalNoOfIntersection;
};

typedef struct statistics_s statistics;

extern statistics *globalStat;

statistics *statistics_new();
void statistics_print(statistics *aStat, FILE *filePtr);


//void statistics_rtreeNoOfNodeStorage(statistics *aStat, node_type *rootP, node_type *rootC, rtree_info *aInfo);
void statistics_overlapTableStatistics(statistics *aStat, mySetSet *overlapTable);
void statistics_totalStorage(configure *aConfigure, statistics *aStat);
void statistics_print(statistics *aStat, FILE *filePtr);
void statistics_influenceValueStatistics(statistics *aStat, mySetSet *resultSet);

void statistics_rtreeNoOfNodeStorageRootP(statistics *aStat, node_type *rootP, rtree_info *aInfo);
void statistics_rtreeNoOfNodeStorageRootC(statistics *aStat, node_type *rootC, rtree_info *aInfo);

void statistics_updateCandidateSize(statistics *aStat, mySetSet *candidateSet, int size, pointListList *intersectSet);

#endif
