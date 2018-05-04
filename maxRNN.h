#ifndef MAXRNN_H
#define MAXRNN_H

#include "config.h"
#include "circle.h"
#include "intersection.h"
#include "statistics.h"

statistics *globalStat;


node_type *maxRNN_buildRtree(pointList *aPointList, rtree_info *aInfo);
node_type *maxRNN_buildRtree_circle(circleList *aCircleList, rtree_info *aInfo);
void printResultSet(mySetSet *resultSet);
circleList *constructCircle(node_type *rootP, pointList *pointListO, rtree_info *rtreeInfo, int k, twoData *aTwoData);
mySetSet *constructOverlapTable(circleList *aCircleList, node_type *rootC, rtree_info *aInfo);
mySetSet *selectQueryPoint(pointList *pointListQ, mySetSet *resultSet, circleList *aCircleList, rtree_info *rtreeInfo, configure *aConfig);

#endif
