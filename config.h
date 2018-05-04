
/*若struct node {}这样来定义结构体的话。在申请node 的变量时，需要这样写，struct node n;
若用typedef，可以这样写，typedef struct node{}NODE; 。在申请变量时就可以这样写，NODE n;
区别就在于使用时，是否可以省去struct这个关键字。*/

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtree.h"
#include "point.h"

struct configure_s
{
	pointList *O;
	pointList *P;
	pointList *Q;

	char *oFilename;
	char *pFilename;
	char *qFilename;

	int k;
	int l;

	config_type *rtreeConf;
	rtree_info *rtreeInfo;

	int ALG;
};

typedef struct configure_s configure;

configure *readConfig(char *filename);

#endif
