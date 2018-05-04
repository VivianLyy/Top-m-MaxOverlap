
#include "rtree.h"

int E_dist_comp_count = 0;
int E_page_access_count = 0;
int E_represent_accuracy = 0;
int E_prune_ans_no = 0;
int E_overlap_yes = 0;
int E_no_answer = 0;



/*
void initialize(config_type *config, char *configFile)
{
FILE *fp;
int string_len;

 //fp = fopen(CONFIG_FILE, "r");
 fp = fopen(configFile, "r");
 
  
   fscanf(fp, "m=%d\n", &m);
   fscanf(fp, "M=%d\n", &M);
   fscanf(fp, "dim=%d\n", &dim);
   fscanf(fp, "reinsert_p=%d\n", &reinsert_p); 
   fscanf(fp, "no_histogram=%d\n",&no_histogram);
   
	
	 
	  fgets(config->positionfile, FILENAME_MAX, fp);
	  string_len = strlen(config->positionfile);
	  config->positionfile[string_len-1] = '\0';
	  
	   fgets(config->queryfile, FILENAME_MAX, fp);
	   string_len = strlen(config->queryfile);
	   config->queryfile[string_len-1] = '\0';
	   
		sprintf(config->save_tree_file, "%s.rstree", config->positionfile);
		}
		
		 
		  
		   void tree_node_allocate(node_type **node)
		   {
		   
			(*node) = (node_type *)malloc(sizeof(node_type));
			(*node)->a = (float *)malloc(sizeof(float) * dim);
			(*node)->b = (float *)malloc(sizeof(float) * dim);
			(*node)->ptr = (node_type **)malloc(sizeof(node_type *) * M);
			}
*/

int read_query(char *queryfile, R_TYPE ***query, int no_query, rtree_info *aInfo)
{
	FILE *fp_pos;  
	//int no_query;
	
	int i,j;
	
	//no_query=100;
	
	fp_pos = fopen(queryfile, "r");
	(*query) = (R_TYPE **)malloc(no_query * sizeof(R_TYPE*));
	for (i=0; i<no_query; i++)
		(*query)[i] = (R_TYPE *)malloc(aInfo->dim * sizeof(R_TYPE));
	
	for (i=0; i<no_query; i++)
		for (j=0; j<aInfo->dim; j++)
			fscanf(fp_pos, "%f", &((*query)[i][j]));
		fclose(fp_pos);
		
		return(no_query);
		
} /* read_query */

/* Distance Computation */
double MINDIST(R_TYPE *P, R_TYPE *a, R_TYPE *b, rtree_info *aInfo)
{
	int i;
	double diff;
	double sum = 0.0;
	
	
	for(i=0 ;i<aInfo->dim ;i++)
	{
		if (P[i] > b[i])
		{
			if (aInfo->norm == 2)
			{
				//sum += pow(P[i] - b[i], 2.0);
				diff = P[i] - b[i];
				sum += diff*diff;
			}
			else
			{
				if (P[i] > b[i])
				{
					diff = P[i] - b[i];
				}
				else
				{
					diff = b[i] - P[i];
				}
				sum += diff;
			}
		}
		else if (P[i] < a[i])
		{
			if (aInfo->norm == 2)
			{
				//sum += pow(a[i] - P[i], 2.0);
				diff = a[i] - P[i];
				sum += diff*diff;
			}
			else
			{
				if (a[i] > P[i])
				{
					diff = a[i] - P[i];
				}
				else
				{
					diff = P[i] - a[i];
				}
				sum += diff;
			}
		}
	}
	
	return sum;     
}

double cal_Euclidean(node_type *node, R_TYPE *query, rtree_info *aInfo)
{
	int i;
	double distance;
	double diff;
	distance = 0.0;
	
	for(i=0; i< aInfo->dim ;i++)
	{
		//distance += pow((node->a[i] - query[i]),(double)2.0);
		diff = node->a[i] - query[i];
		distance += diff*diff;
	}
	
	//return (sqrt(distance));
	return distance;
}

double cal_ManhattanDistance(R_TYPE *point, R_TYPE *query, rtree_info *aInfo)
{
	int i;
	double distance;
	double diff;
	distance = 0.0;
	
	for(i=0; i< aInfo->dim ;i++)
	{
		//distance += pow((node->a[i] - query[i]),(double)2.0);
		if (point[i] > query[i])
		{
			diff = point[i] - query[i];
		}
		else
		{
			diff = query[i] - point[i];
		}
		distance += diff;
	}
	
	//return (sqrt(distance));
	return distance;
}

double cal_Manhattan(node_type *node, R_TYPE *query, rtree_info *aInfo)
{
	double distance;

	distance = 0.0;
	
	distance = cal_ManhattanDistance(node->a, query, aInfo);
	
	//return (sqrt(distance));
	return distance;
}

/***********************************/
/* rectangle_search():             */
/* search query points on the tree */
/*************************** *******/

int rectangle_search(mySet *resultSet, node_type *curr_node, R_TYPE *query, double error, rtree_info *aInfo)
{
	int find_flag;
	int i, j, stop, flag;
	int query_dim;              
	
	query_dim = aInfo->dim;
	
	/* Search leaf node */
	if(curr_node->attribute == LEAF) { 
		
		//for(j=0; j<query_dim; j++)   
		//	printf("%f ", curr_node->a[j]);
        
		//printf("  at %d\n", curr_node->id);

		mySet_insert(resultSet, curr_node->id);
		
        return(FOUND);
		
	}
	
	stop = aInfo->M - curr_node->vacancy;
	for(i=0; i < stop; i++) {
		
        flag = TRUE;
		
        /* search subtree */
        for(j=0; j < query_dim; j++) {
			
			if(curr_node->ptr[i]->a[j] > (query[j] + error) ||
				curr_node->ptr[i]->b[j] < (query[j] - error))
			{
				flag = FALSE;
				break;
			}
        }
		
        /* search the node which contains the query */
        if(flag==TRUE) {
			find_flag = rectangle_search(resultSet, curr_node->ptr[i], query, error, aInfo);
        }
	}
	
	return(find_flag);
	
}
/* rectangle_search */


// return center
// for circle and rhombus
R_TYPE *findCenterAndRadius(node_type *aNode, rtree_info *aInfo, double *radius)
{
	R_TYPE *returnValue;

	int i;

	returnValue = (R_TYPE *) malloc(sizeof(R_TYPE)*aInfo->dim);
	memset(returnValue, 0, sizeof(R_TYPE)*aInfo->dim);

	for (i = 0; i < aInfo->dim; i++)
	{
		returnValue[i] = (R_TYPE) (0.5*(aNode->a[i] + aNode->b[i]));
	}

	(*radius) = 0.5*(aNode->b[0] - aNode->a[0]);

	return returnValue;
}

double calculateDistance_sqrt(R_TYPE *point1, R_TYPE *point2, rtree_info *aInfo)
{
	double returnValue;
	int i;

	double temp;

	returnValue = 0;
	for (i = 0; i < aInfo->dim; i++)
	{
		temp = point1[i] - point2[i];
		returnValue += temp*temp;
	}
	returnValue = sqrt(returnValue);

	return returnValue;
}

// two circles
// two rhombus
int twoCircle_isOverlap(R_TYPE *center1, double radius1, R_TYPE *center2, double radius2, rtree_info *aInfo)
{
	int returnValue;

	double dist;

	double threshold = 0.00001;

	if (aInfo->norm == 2)
	{
		dist = calculateDistance_sqrt(center1, center2, aInfo);
	}
	else
	{
		dist = cal_ManhattanDistance(center1, center2, aInfo);
	}

	if (dist <= (radius1 + radius2) + threshold)
	{
		returnValue = 1;
	}
	else
	{
		returnValue = 0;
	}

	return returnValue;
}



int range_search(mySet *resultSet, node_type *curr_node, R_TYPE *query, double error, rtree_info *aInfo)
{
	int find_flag;
	int i, j, stop, flag;
	int query_dim;              

	double radius;
	R_TYPE *center;

	int isOverlap;
	
	query_dim = aInfo->dim;
	
	/* Search leaf node  递归结束点*/ 
	if(curr_node->attribute == LEAF) { 
		
		//for(j=0; j<query_dim; j++)   
		//	printf("%f ", curr_node->a[j]);
        
		//printf("  at %d\n", curr_node->id);

		center = findCenterAndRadius(curr_node, aInfo, &radius);

		isOverlap = twoCircle_isOverlap(center, radius, query, error, aInfo);

		free(center);

		if (isOverlap == 1)
		{
			mySet_insert(resultSet, curr_node->id);
			
			return(FOUND);
		}
		else
		{
			return NOT_FOUND;
		}
	}
	
	stop = aInfo->M - curr_node->vacancy;
	for(i=0; i < stop; i++) {
		
        flag = TRUE;
		
        /* search subtree */
        for(j=0; j < query_dim; j++) {
			
			if(curr_node->ptr[i]->a[j] > (query[j] + error) ||
				curr_node->ptr[i]->b[j] < (query[j] - error))
			{
				flag = FALSE;
				break;
			}
        }
		
        /* search the node which contains the query */
        if(flag==TRUE) {
			find_flag = range_search(resultSet, curr_node->ptr[i], query, error, aInfo);
        }
	}
	
	return(find_flag);
	
}


/**********************************/
/* rectangle_search_tree():       */
/* prepare to search query points */
/**********************************/

void rectangle_search_tree(node_type *root, int no_query, R_TYPE **query, float error, rtree_info *aInfo) 
{
	int query_index;
	int j, find_flag = NOT_FOUND;
	int query_dim;

	mySet *resultSet;
	
	query_dim = aInfo->dim;  
	
	/* start search data points by invoking rectangle_search() */
	for(query_index=0; query_index < no_query; query_index++) {
        
		
        printf("Query ");
        for(j=0; j < query_dim; j++)
			printf("%f ", query[query_index][j]);
        printf("is found satisfied with\n");
		
		
        find_flag = NOT_FOUND;
		resultSet = mySet_new();
        find_flag = rectangle_search(resultSet, root, query[query_index], error, aInfo);
		mySet_free(&resultSet);
		
	}
	
	
} /* rectangle_search_tree */




void read_inter_node(node_type *node, FILE *fp, rtree_info *aInfo)
{
	int i, count;
	
	for (i = 0; i<aInfo->dim; i++)
        fscanf(fp, "%f\n", &((node->a)[i]));
	
	for (i = 0; i<aInfo->dim; i++)
        fscanf(fp, "%f\n", &((node->b)[i]));
	
	fscanf(fp, "%d\n", &(node->attribute));
	if (node->attribute == LEAF) {
		fscanf(fp, "%d\n", &(node->id));
	}
	fscanf(fp, "%d\n", &(node->vacancy));
	
	/****************************************
	for (i = 0; i<dim; i++)
	printf("%f\n", node->a[i]);
	
	 for (i = 0; i<dim; i++)
	 printf("%f\n", node->b[i]);
	 
	  printf("%d\n", node->attribute);
	  if (node->attribute == LEAF) {
	  printf("%d\n", node->id);
	  }
	  printf("%d\n", node->vacancy);
	****************************************/
	
	if (node->attribute != LEAF) {
		count = aInfo->M - node->vacancy;
		for (i=0; i<count; i++) {
			tree_node_allocate(&(node->ptr[i]), aInfo);
			read_inter_node(node->ptr[i], fp, aInfo);
		}
	}        
	
	return;
	
} 



void read_rtree(node_type **root, char save_tree_file[], rtree_info *aInfo)
{ 
	FILE *fp;
	
	
	tree_node_allocate(root, aInfo);
	
	//fp = fopen(SAVE_RTREE_FILE, "r");
	fp = fopen(save_tree_file, "r");
	if (!fp) {
		printf("Can't open file %s\n", save_tree_file);
		exit(EXIT_FAILURE);
	}
	
	read_inter_node(*root, fp, aInfo);
	
	fclose(fp);
	
	return;
	
}

void NN_freeChain(NN_type *NN)
{
	if (NN->next != NULL)
	{
		NN_freeChain(NN->next);
	}
	free(NN);
}

void NN_update(NN_type **NN, double dist, node_type *node, int level, int k)
{
	int i=0;
	int l;
	int count;
	NN_type *nextNN;
	NN_type *oldNN;
	int noOfNode;
	
	if ((*NN)->dist == dist)
	{
//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		nextNN = (NN_type *) malloc(sizeof(NN_type));
		nextNN->oid = node->id;
		nextNN->dist = dist;
		nextNN->pointer = node;
		nextNN->level = level;
		nextNN->next = (*NN)->next;
		
		(*NN)->next = nextNN;
		
		return;
	}
	
	nextNN = (*NN);
	//while (i<k-1 && NN->next->dist >= dist)
	while ((nextNN->next != NULL) && (nextNN->next->dist > dist))
	{
		nextNN->dist = nextNN->next->dist;
		nextNN->level = nextNN->next->level;
		nextNN->oid = nextNN->next->oid;
		nextNN->pointer = nextNN->next->pointer;
		nextNN = nextNN->next;
		i++;
	}
	
	nextNN->dist = dist;
	nextNN->level = level;
	nextNN->oid = node->id;
	nextNN->pointer = node;
	
	
	// shrink
	noOfNode = 1;
	nextNN = (*NN);
	while (nextNN->next != NULL)
	{
		noOfNode++;
		nextNN = nextNN->next;
	}
	
	if (noOfNode != k)
	{
//printf("----------------------------------------\n");
		//check
		l = noOfNode-k;
		
		// find l-th
		count = 1;
		nextNN = (*NN);
		while (count < l)
		{
			count++;
			nextNN = nextNN->next;
		}
		
		if (nextNN->next != NULL)
		{
			if (nextNN->dist == nextNN->next->dist)
			{
				// do nothing
			}
			else
			{
				oldNN = (*NN);
				(*NN) = nextNN->next;
				
				nextNN->next = NULL;
				
				
				// free
				NN_freeChain(oldNN);
			}
		}
		else
		{
			// impossible
		}
	}
	
	
	return;
}



//static int compare(ABL *i, ABL *j) 
//static int compare(const ABL *i, const ABL *j) 
static int compare(const void *a, const void *b)
{
	ABL *i;
	ABL *j;
	i = (ABL *) a;
	j = (ABL *) b;
	if (i->min > j->min)
		return (1);
	if (i->min < j->min)
		return (-1);
	return (0);
}

/*
static int compare(const ABL *i, const ABL *j) 
{
	if (i->min > j->min)
		return (1);
	if (i->min < j->min)
		return (-1);
	return (0);
}
*/

void gen_ABL(node_type *node, ABL *branch, R_TYPE *query, int total, rtree_info *aInfo)
{
	int i;
	for (i=0 ; i<total ; i++)
	{
		branch[i].node = node->ptr[i];
		branch[i].min = MINDIST(query, node->ptr[i]->a, node->ptr[i]->b, aInfo);
	}
	//qsort(branch, total, (int) sizeof(struct BranchArray),  compare);
	return;
}

void k_NN_NodeSearch(node_type *curr_node, R_TYPE *query, NN_type **NN, int k, int level, rtree_info *aInfo)
{
	
	int i, total;
	double dist;
	ABL       *branch;
	
	E_page_access_count++;

	if (curr_node->vacancy == aInfo->M)
	{
		return;
	}
	
	/* Please refer NN Queries paper */
	if (curr_node->ptr[0]->attribute == LEAF) 
	{
		total = aInfo->M - curr_node->vacancy;
		for (i=0;i<total;i++)
		{
			if (curr_node->ptr[i]->id != -1)
			{
				if (aInfo->norm == 2)
				{
					dist = cal_Euclidean(curr_node->ptr[i], query, aInfo);
				}
				else
				{
					dist = cal_Manhattan(curr_node->ptr[i], query, aInfo);
				}
				if (dist <= (*NN)->dist)
					NN_update(NN, dist, curr_node->ptr[i], level+1, k);
				}
		}
	}
	else
	{
		/* Please refer SIGMOD record Sep. 1998 Vol. 27 No. 3 P.18 */
		total = aInfo->M - curr_node->vacancy;
		//branch = (struct BranchArray *) malloc(total*sizeof(struct BranchArray)*total);
		branch = (ABL *) malloc(sizeof(ABL)*total*total);
		if (branch == NULL)
		{
			printf("Cannot return branch!\n");
		}
		memset(branch, 0, sizeof(ABL)*total*total);
		gen_ABL(curr_node, branch, query, total, aInfo);
		for (i=0;i<total;i++)
		{
			if (branch[i].min > (*NN)->dist)
			{
				//break;
			}
			else
			{
				k_NN_NodeSearch(branch[i].node, query, NN, k, level+1, aInfo);
			}
		}
		free(branch);
	}
	return;
}


void k_NN_trim(NN_type *head, int k)
{
	NN_type *aNN;
	NN_type *prevNN;
	NN_type *nextNN;
	NN_type *diffNN;
	double minDist;
	
	int i;
	aNN = head;
	
	for (i = 0; i < k-1; i++)
	{
		aNN = aNN->next;
	}
	
	minDist = aNN->dist;
	
	prevNN = aNN;
	nextNN = prevNN->next;
	diffNN = NULL;
	while (nextNN != NULL)
	{
		if (nextNN->dist != minDist)
		{
			diffNN = nextNN;
			
			prevNN->next = NULL;
			break;
		}
		
		prevNN = nextNN;
		nextNN = prevNN->next;
	}
	
	if (diffNN != NULL)
	{
		// free the remaining
		NN_freeChain(diffNN);
	}
}

void k_NN_search(node_type *root, R_TYPE *query, int k, NN_type **returnResult, rtree_info *aInfo)
{
	int i;
	
	NN_type *NN, *head;
	NN_type *newNN;
	
	NN = (NN_type *) malloc(sizeof(NN_type));
	if (NN == NULL)
        fprintf(stderr, "malloc error at k-NN_search 1\n");
	NN->oid = UNDEFINED;
	NN->level = -1;
	NN->dist = INFINITY;
	NN->pointer = NULL;
	head = NN;
	
	for (i=0; i< k-1; i++) {
		
		newNN = (NN_type *) malloc(sizeof(NN_type));
        if (newNN == NULL)
			fprintf(stderr, "malloc error at k-NN_search 1\n");
		
        newNN->oid = UNDEFINED; 
		newNN->level = -1;
        newNN->dist = INFINITY;
		newNN->pointer = NULL;
		newNN->next = NULL;
		
		NN->next = newNN;
        NN = newNN;
	}
	NN->next = NULL;
	
	k_NN_NodeSearch(root, query, &head, k, 0, aInfo);
	//k_NN_trim(head, k);
	
	NN = head;
/*
	printf("\n");
	
	while (NN != NULL) {
		
        printf("ID: %d    Dist: %f\n", NN->oid, NN->dist);
		
        NN = NN->next;
		
	}
	printf("\n");
*/
	
	(*returnResult) = head;
	
	return;
	
}

void free_tree(node_type *node, rtree_info *aInfo)
{
	int i;
	
	if (!node) return;
	
	free(node->a);
	free(node->b);
	
	for (i=0; i < aInfo->M-node->vacancy; i++)
        free_tree(node->ptr[i], aInfo);
	
	free(node);
	
	return;
}

// NULL: we cannot find a child
NN_type *findRandomLeafNode(node_type *root, rtree_info *aInfo, int level)
{
	NN_type *returnValue;

	int noOfChild, childNo;

	if (root->attribute == LEAF)
	{
		//returnValue = root;
		returnValue = (NN_type *) malloc(sizeof(NN_type));
		memset(returnValue, 0, sizeof(NN_type));

		returnValue->dist = UNDEFINED;
		returnValue->level = level;
		returnValue->next = NULL;
		returnValue->oid = root->id;
		returnValue->pointer = root;
	}
	else
	{
		noOfChild = aInfo->M - root->vacancy;
		if (noOfChild == 0)
		{
			returnValue = NULL;
		}
		else
		{
			returnValue = NULL;
			while (returnValue == NULL)
			{
				childNo = rand()%noOfChild;
				returnValue = findRandomLeafNode(root->ptr[childNo], aInfo, level+1);
			}
		}
	}

	return returnValue;
}

NN_type *findRandomPoint(node_type *root, rtree_info *aInfo)
{
	NN_type *returnValue;

	int level;

	level = 0;
	
	returnValue = findRandomLeafNode(root, aInfo, level);

	return returnValue;
}


int main2(int argc, char *argv[])
{
	rtree_info aInfo;
	node_type *root;
	config_type config;
	int no_query;
	R_TYPE **query;
	float error;
	
	int i, nn;
	NN_type *result;  
	//char E_result_filename[FILENAME_MAX], dummy[80];
	
#ifndef WIN32
	// for experiment
	float  userTime, sysTime;
	struct rusage myTime1, myTime2;
#endif
	FILE *E_result_fp;            
	double E_represent_accuracy_sum, E_prune_accuracy_sum;
	int E_page_access_count_sum;
	
	//////////////
	if (argc != 4) {
		printf("Usage: %s <config> <k> <#query> <result file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	//strcpy(E_result_filename, argv[4]);
	
	//////////////
	initialize(&config, argv[1], &aInfo);
	
	printf("read_rtree\n");
	read_rtree(&root, config.save_tree_file, &aInfo);
	
	printf("read_query\n");
	no_query = read_query(config.queryfile, &query, atoi(argv[3]), &aInfo);
	
	printf("start searching\n");
	
	if (CHOICE==RANGE_SEARCH) {
		
		while (TRUE) {
			
			printf("Please input the error bound:");
			scanf("%f", &error);
			rectangle_search_tree(root, no_query, query, error, &aInfo);
			
		}
	}
	
	else if (CHOICE == kNN_SEARCH) {
		
		nn = atoi(argv[2]);
		
		E_page_access_count_sum = 0;
		E_represent_accuracy_sum = 0.0; 
		E_prune_accuracy_sum = 0.0;
		E_dist_comp_count = 0;
#ifndef WIN32                         
		getrusage(RUSAGE_SELF,&myTime1);
#endif
		for (i=0; i<no_query; i++) {
			
			printf("Query %d\n", i+1);
			E_page_access_count = 0;
			E_represent_accuracy = 0;
			E_prune_ans_no = 0;
			E_overlap_yes = 0;
			
			k_NN_search(root, query[i], nn, &result, &aInfo);
			
			E_page_access_count_sum += E_page_access_count;
			E_represent_accuracy_sum += ((double)E_represent_accuracy)/((double)E_page_access_count);
			E_prune_accuracy_sum += ((double)E_overlap_yes)/((double)E_prune_ans_no);
			
			printf("---------------------------\n");
		}
#ifndef WIN32                                
		getrusage(RUSAGE_SELF,&myTime2);
		
        userTime =
			((float) (myTime2.ru_utime.tv_sec  - myTime1.ru_utime.tv_sec)) +
			((float) (myTime2.ru_utime.tv_usec - myTime1.ru_utime.tv_usec)) * 1e-6;
        sysTime =
			((float) (myTime2.ru_stime.tv_sec  - myTime1.ru_stime.tv_sec)) +
			((float) (myTime2.ru_stime.tv_usec - myTime1.ru_stime.tv_usec)) * 1e-6;
#endif
		
        //E_result_fp = fopen(E_result_filename, "w");
        E_result_fp = stdout;
		
		fprintf(E_result_fp, "Number of query is %d\n", no_query);
		fprintf(E_result_fp, "average no. of page accesses = %f\n", (double)E_page_access_count_sum/(double)no_query);
		
#ifndef WIN32
        fprintf(E_result_fp, "User time : %f seconds\n",userTime);
        fprintf(E_result_fp, "System time : %f seconds\n",sysTime);
        fprintf(E_result_fp, "Total time : %f seconds\n",userTime+sysTime);
#endif
		
		fprintf(E_result_fp, "\n");
		fclose(E_result_fp);
		
	}
	
	printf("free_tree\n");
	free_tree(root, &aInfo);
	
	return(0);
}
