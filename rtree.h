#ifndef RTREE_H
#define RTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#ifndef WIN32
#include <sys/resource.h>
#include <sys/times.h>
#include <unistd.h>
#endif

#include "qsort.h"
#include "set.h"

#define DEBUG 1


#define CONFIG_FILE	"rtree.config"
#define SAVE_RTREE_FILE "save_rtree_file"

#define FALSE    	0
#define TRUE     	1

#define RANGE_SEARCH 0
#define kNN_SEARCH 1
#define CHOICE kNN_SEARCH

#define ASC_NUM  	48
#define NO_ID	 	-1
#define FOUND		1
#define NOT_FOUND 	0

#define ROOT  0   
#define LEAF  1
#define NODE  2

//#define INFINITY  FLT_MAX
#define INFINITY  1E+37 //FLT_MAX
#define UNDEFINED -3  // for id of entries in PR

//#define R_FLOAT 0
//#define R_TYPE int
#define R_FLOAT 1
#define R_TYPE float

/* Global variable ******************
m: min. number entries of each node;
M: max. number entries of each node;
dim: dimension of the rtree data.
*************************************/

typedef struct rtree_info_s
{
	int m, M, dim, reinsert_p, no_histogram;
	int extra_level;
	
	int norm;
} rtree_info;

typedef struct node { 
	R_TYPE *a;
	R_TYPE *b;
	int id;
	int attribute;//属性
	int vacancy;//
	struct node *parent;
	struct node **ptr; //子节点
}   node_type;
	
typedef struct NN {	
		double dist;
		int oid;
		struct node *pointer; 
		int level;
		struct NN *next; 
} NN_type;
		
typedef struct BranchArray {	
	double min;
	node_type *node;
} ABL;


typedef struct config { 
	int dim;
	int m;
	int M;
	int reinsert_p;
	int no_histogram;
	//char nodefile[FILENAME_MAX];
	//char rootfile[FILENAME_MAX];
	char queryfile[FILENAME_MAX];
	char positionfile[FILENAME_MAX]; 
	char save_tree_file[FILENAME_MAX]; 
}   config_type;



struct setNode_s
{
	int noOfNode;
	node_type **elt;
	int *level;
};

typedef struct setNode_s setNode;

config_type *rtreeConf_new();
rtree_info *rtreeInfo_new();

void node_free(node_type *aNode, rtree_info *aInfo);

int rtree_checkConsistent(node_type *root, rtree_info *aInfo);

void noOfNodeRtree(node_type *root, int *noOfNode, rtree_info *aInfo);
int rtree_nodeSize(rtree_info *aInfo);

void insert_node(node_type *root, R_TYPE *data, int seq_id, rtree_info *aInfo);

void insert_node_MBR(node_type *root, R_TYPE *a, R_TYPE *b, int seq_id, rtree_info *aInfo);

void overflow(node_type *over_node, int over_level, int old_level, node_type
			  *extra_node, node_type *root, rtree_info *aInfo);

void initialize(config_type *config, char *configFile, rtree_info *aInfo);
void tree_node_allocate(node_type **node, rtree_info *aInfo);
void NN_freeChain(NN_type *aNN);

void k_NN_search(node_type *root, R_TYPE *query, int k, NN_type **returnResult, rtree_info *aInfo);

void condenseTree(node_type *root, node_type *aLeafNode, int inputLevel, rtree_info *aInfo);
NN_type *findRandomPoint(node_type *root, rtree_info *aInfo);

int range_search(mySet *resultSet, node_type *curr_node, R_TYPE *query, double error, rtree_info *aInfo);

#endif
