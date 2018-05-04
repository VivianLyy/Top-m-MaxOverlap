
#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qsort.h"

struct mySet_s
{
	int noOfElt;
	int *elt;
};

typedef struct mySet_s mySet;

struct mySetSet_s
{
	int noOfElt;
	mySet **elt;
};

typedef struct mySetSet_s mySetSet;

mySet *mySet_new();

void mySet_insert(mySet *aSet, int aElt);
void mySet_free(mySet **aSet);
void mySet_print(mySet *aSet);
void mySet_removeElt(mySet *aSet, int no);
void mySet_sort(mySet *aSet);
void mySet_insert_sorted(mySet *aSet, int eltNo);
int mySet_isExist(mySet *aSet, int eltNo);
int mySet_isAllIncluded(mySet *set1, mySet *set2);
int mySet_findIndex(mySet *aSet, int eltNo);

int mySet_intersection(int*a, int*b, int*c, int lenA, int lenB);
int mySet_difference(int*a, int*b, int*c, int lenA, int lenB);

mySetSet *mySetSet_new(int noOfElt);

void mySetSet_insert(mySetSet *aSetSet, mySet *aSet);
void mySetSet_updateSortDescendingOrder(mySetSet *aSetSet, mySet *S);
void mySetSet_free(mySetSet *aSetSet);
int mySetSet_isExist(mySetSet *aSetSet, mySet *S);

struct interPoint_query_q{ // influ
	int key;
	mySet* S;
	// bool operator < (const interPoint_query &b) const {
	// 	return key < b.key;
	// }
};

typedef struct interPoint_query_q interPoint_query;

interPoint_query interPoint_query_new(int k , mySet* aSet);

interPoint_query interPoint_query_update(interPoint_query Q,  mySet* del);

#endif
