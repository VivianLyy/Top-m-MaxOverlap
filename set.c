
#include "set.h"

mySet *mySet_new()
{
	mySet *returnValue;

	returnValue = (mySet *) malloc(sizeof(mySet));
	memset(returnValue, 0, sizeof(mySet));

	return returnValue;
}

void mySet_free(mySet **aSet)
{
	int noOfElt;

	noOfElt = (*aSet)->noOfElt;
	if (noOfElt > 0)
	{
		free((*aSet)->elt);
	}
	free((*aSet));
}

void mySet_insert(mySet *aSet, int aElt)
{
	int noOfElt;
	int *elt;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;
//先为指针重新分配内存
	if (noOfElt == 0)
	{
		elt = (int *) malloc(sizeof(int));
	}
	else
	{
		elt = (int *) realloc(elt, sizeof(int)*(noOfElt+1));
	}
	elt[noOfElt] = aElt;

	aSet->noOfElt++;
	aSet->elt = elt;
}

mySetSet *mySetSet_new(int noOfElt)
{
	mySetSet *returnValue;

	returnValue = (mySetSet *) malloc(sizeof(mySetSet));
	memset(returnValue, 0, sizeof(mySetSet));

	if (noOfElt != 0)
	{
		returnValue->noOfElt = noOfElt;
		returnValue->elt = (mySet **) malloc(sizeof(mySet *)*noOfElt);
		memset(returnValue->elt, 0, sizeof(mySet *)*noOfElt);
	}

	return returnValue;
}

void mySetSet_insert(mySetSet *aSetSet, mySet *aSet)
{
	mySet **elt;
	int noOfElt;

	noOfElt = aSetSet->noOfElt;
	elt = aSetSet->elt;

	if (noOfElt == 0)
	{
		elt = (mySet **) malloc(sizeof(mySet *));
	}
	else
	{
		elt = (mySet **) realloc(elt, sizeof(mySet *)*(noOfElt+1));
	}
	elt[noOfElt] = aSet;

	aSetSet->elt = elt;
	aSetSet->noOfElt++;
}

// Add mySet_isAllIncluded()
int mySet_isAllIncluded(mySet *set1, mySet *set2)
{
	int returnValue;

	int noOfElt1;
	int *elt1;

	int noOfElt2;
	int *elt2;

	int i;

	noOfElt1 = set1->noOfElt;
	elt1 = set1->elt;

	noOfElt2 = set2->noOfElt;
	elt2 = set2->elt;

	int num = noOfElt1 < noOfElt2 ? noOfElt1:noOfElt2; //选择较小的
	
	returnValue = 1;
	for (i = 0; i < num; i++)
	{
		if (elt1[i] != elt2[i])
		{
			returnValue = 0;
			break;
		}
	}
	
	return returnValue;
}

// modify mySet_isSame()
int mySet_isSame(mySet *set1, mySet *set2)
{
	int returnValue;

	int noOfElt1;
	int *elt1;

	int noOfElt2;
	int *elt2;

	int i;
	int isAllInclude;

	noOfElt1 = set1->noOfElt;
	elt1 = set1->elt;

	noOfElt2 = set2->noOfElt;
	elt2 = set2->elt;

	if (noOfElt1 != noOfElt2)
	{
		isAllInclude = mySet_isAllIncluded(set1, set2);

		if(isAllInclude)
		{
			returnValue = 1;
		}
		else 
		{
			returnValue = 0;
		}
	}
	else
	{
		returnValue = 1;
		for (i = 0; i < noOfElt1; i++)
		{
			if (elt1[i] != elt2[i])
			{
				returnValue = 0;
				break;
			}
		}
	}

	return returnValue;
}

/*
int mySet_isSame(mySet *set1, mySet *set2)
{
	int returnValue;

	int noOfElt1;
	int *elt1;

	int noOfElt2;
	int *elt2;

	int i;

	noOfElt1 = set1->noOfElt;
	elt1 = set1->elt;

	noOfElt2 = set2->noOfElt;
	elt2 = set2->elt;

	if (noOfElt1 != noOfElt2)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = 1;
		for (i = 0; i < noOfElt1; i++)
		{
			if (elt1[i] != elt2[i])
			{
				returnValue = 0;
				break;
			}
		}
	}

	return returnValue;
}
*/

int mySetSet_isExist(mySetSet *aSetSet, mySet *S)
{
	int returnValue;

	int noOfElt;
	mySet **elt;

	int i;
	mySet *aSet;

	int isSame;

	noOfElt = aSetSet->noOfElt;
	elt = aSetSet->elt;

	returnValue = 0;
	for (i = 0; i < noOfElt; i++)
	{
		aSet = elt[i];
		isSame = mySet_isSame(aSet, S);//返回0不存在的时候 才可以更新
		if (isSame == 1)
		{
			returnValue = 1;
			break;
		}
	}

	return returnValue;
}

void mySetSet_updateSortDescendingOrder(mySetSet *aSetSet, mySet *S)
{
	int noOfElt;
	mySet **elt;

	int i;

	int index;

	int isExist;

	isExist = mySetSet_isExist(aSetSet, S);

	if (isExist == 0)
	{

		noOfElt = aSetSet->noOfElt;
		elt = aSetSet->elt;
		
		for (i = 0; i < noOfElt; i++)
		{
			if (S->noOfElt >= elt[i]->noOfElt)
			{
				index = i;
				break;
			}
		}

		// free the last elt
		mySet_free(&(elt[noOfElt-1]));

		// move the element one step from index to noOfElt-1
		for (i = noOfElt-1 ; i > index; i--)
		{
			elt[i] = elt[i-1];
		}

		// assign this set to index
		elt[index] = S;
	}
}


void mySet_print(mySet *aSet)
{
	int noOfElt;

	int *elt;

	int i;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;

	for (i = 0; i < noOfElt; i++)
	{
		printf("%d ", elt[i]);
	}
}

void mySet_removeElt(mySet *aSet, int no)
{
	int noOfElt;
	int *elt;
	int *newElt;

	int i;
	int index;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;

	if (noOfElt == 1)
	{
		newElt = NULL;
	}
	else if (noOfElt > 1)
	{
		newElt = (int *) malloc(sizeof(int)*(noOfElt-1));

		index = 0;
		for (i = 0; i < noOfElt ; i++)
		{
			if (elt[i] != no)
			{
				newElt[index] = elt[i];
				index++;
			}
		}
	}

	if (noOfElt > 0)
	{
		free(elt);
	}

	noOfElt--;

	aSet->noOfElt = noOfElt;
	aSet->elt = newElt;

}

void mySet_sort(mySet *aSet)
{
	int noOfElt;
	int *elt;

	int *index;

	int i;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;

	if (noOfElt > 0)
	{
		index = (int *) malloc(sizeof(int)*noOfElt);
		memset(index, 0, sizeof(int)*noOfElt);

		for (i = 0; i < noOfElt; i++)
		{
			index[i] = i;
		}

		quicksort_i(index, elt, 0, noOfElt-1);

		free(index);
	}
	
}

// assume sorted in ascending order
void mySet_insert_sorted(mySet *aSet, int eltNo)
{
	int noOfElt;
	int *elt;

	int index;

	int *newElt;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;

	index = 0;
	while (index < noOfElt)
	{
		if (elt[index] > eltNo)
		{
			break;
		}
		index++;
	}

	newElt = (int *) malloc(sizeof(int)*(noOfElt+1));

	if (index != 0)
	{
		memcpy(newElt, elt, sizeof(int)*index);
	}

	if (index != noOfElt)
	{
		memcpy(&(newElt[index+1]), &(elt[index]), sizeof(int)*(noOfElt-index));
	}
	newElt[index] = eltNo;

	free(aSet->elt);
	aSet->elt = newElt;
	
	aSet->noOfElt = noOfElt+1;
}

int mySet_isExist(mySet *aSet, int eltNo)
{
	int returnValue;

	int noOfElt;
	int *elt;

	int i;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;

	returnValue = 0;
	for (i = 0; i < noOfElt; i++)
	{
		if (elt[i] == eltNo)
		{
			returnValue = 1;
			break;
		}
	}

	return returnValue;
}

void mySetSet_free(mySetSet *aSetSet)
{
	int noOfElt;
	mySet **elt;

	int i;
	
	noOfElt = aSetSet->noOfElt;
	elt = aSetSet->elt;

	for (i = 0; i < noOfElt; i++)
	{
		mySet_free(&(elt[i]));
	}
	free(elt);

	free(aSetSet);
}

// -1: cannot find
int mySet_findIndex(mySet *aSet, int eltNo)
{
	int returnValue;

	int i;

	int noOfElt;
	int *elt;

	noOfElt = aSet->noOfElt;
	elt = aSet->elt;

	returnValue = -1;
	for (i = 0; i < noOfElt; i++)
	{
		if (elt[i] == eltNo)
		{
			returnValue = i;
			break;
		}
	}


	return returnValue;
}

int mySet_intersection(int*a, int*b, int*c,int lenA,int lenB)
{
	int i = 0, j = 0, k = 0;
	while (i < lenA && j < lenB)
	{
	if (a[i] == b[j])
	{
	c[k++] = a[i];
	// printf("a[i] is %d b[i] is %d k is %d \n", a[i],b[j],k );
	i++;
	j++;
	}
	else if (a[i] < b[j]) j++;
	else i++;
	}
	return k;
}

int mySet_difference(int*a, int*b, int*c,int lenA,int lenB)
{
int i = 0, j = 0, k = 0;
while (i < lenA && j < lenB)
{
	if (a[i] == b[j])
	{
		i++;
		j++;
	}
	else {
		// c = (int *) realloc(c, sizeof(int)*(k+1));
		c[k++] = a[i];
		i++;
	}
}
return k;
}


interPoint_query interPoint_query_update(interPoint_query Q, mySet* del)
{
	interPoint_query returnValue;
	mySet* target = (mySet *) malloc(sizeof(mySet));
	target->noOfElt = Q.S->noOfElt;
	target->elt = Q.S->elt;

// printf(" target 前: " );
// mySet_print(target);
// printf("\n");

	int PointNum = target->noOfElt > del->noOfElt ? target->noOfElt : del->noOfElt;
	int* intersect;
	int* difference;
	int difference_Num = 0;
	intersect = (int *) malloc(sizeof(int)*(PointNum));
	difference = (int *) malloc(sizeof(int)*(PointNum));
	// target->elt = (int *) malloc(sizeof(int)*(PointNum));

	int intersection_Num = mySet_intersection(target->elt, del->elt, intersect, target->noOfElt, del->noOfElt);

// printf(" target after intersection: " );
// mySet_print(target);
// printf("\n");

	// printf("intersection_Num is %d \n", intersection_Num);
	if(intersection_Num == 0){
		target->noOfElt = Q.S->noOfElt;
		target->elt = Q.S->elt;
	}
	else{
 		 difference_Num = mySet_difference(target->elt, intersect, difference, target->noOfElt, intersection_Num);
		 target->elt = difference;
    	 target->noOfElt = difference_Num;
    	 // printf("difference_Num is %d \n", difference_Num);
	}
   

// printf(" del: " );
// mySet_print(del);
// printf("\n");

// printf(" target 后: " );
// mySet_print(target);
// printf("\n");

 	// returnValue.key = intersection_Num ;
    returnValue.S = target;

    returnValue.key = target->noOfElt;

 //    printf(" returnValue S: " );
	// mySet_print(returnValue.S);
	// printf("\n");

    return returnValue;
}


interPoint_query interPoint_query_new(int k , mySet* aSet)
{
	interPoint_query returnValue;

	returnValue.key = k;
	
	mySet *temp_myset = (mySet *) malloc(sizeof(mySet));
	temp_myset->noOfElt = aSet->noOfElt;
	temp_myset->elt = aSet->elt;

	returnValue.S = temp_myset;

	return returnValue;
}

