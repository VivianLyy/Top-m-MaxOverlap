
#include "rtree.h"

config_type *rtreeConf_new()
{
	config_type *returnValue;

	returnValue = (config_type *) malloc(sizeof(config_type));
	memset(returnValue, 0, sizeof(config_type));

	return returnValue;
}

rtree_info *rtreeInfo_new()
{
	rtree_info *returnValue;

	returnValue = (rtree_info *) malloc(sizeof(rtree_info));
	memset(returnValue, 0, sizeof(rtree_info));

	return returnValue;
}




setNode *setNode_new()
{
	setNode *returnValue;
	
	returnValue = (setNode *) malloc(sizeof(setNode));
	memset(returnValue, 0, sizeof(setNode));
	
	return returnValue;
}


void setNode_free(setNode *aSetNode)
{
	if (aSetNode->noOfNode > 0)
	{
		free(aSetNode->elt);
		free(aSetNode->level);
	}
}

void setNode_insert(setNode *aSetNode, node_type **aNode, int aLevel)
{
	int noOfNode;
	node_type **elt;
	int *level;
	
	noOfNode = aSetNode->noOfNode;
	elt = aSetNode->elt;
	level = aSetNode->level;
	
	if (noOfNode == 0)
	{
		elt = (node_type **) malloc(sizeof(node_type *));
		level = (int *) malloc(sizeof(int));
	}
	else
	{
		elt = (node_type **) realloc(elt, sizeof(node_type *)*(noOfNode+1));
		level = (int *) realloc(level, sizeof(int)*(noOfNode+1));
	}
	
	elt[noOfNode] = (*aNode);
	level[noOfNode] = aLevel;
	
	aSetNode->noOfNode++;
	
	aSetNode->elt  = elt;
	aSetNode->level = level;
}

// 1: remove successfully
// 0: unsuccessfully
int setNode_removeLast(setNode *aSetNode, node_type **aNode, int *aLevel)
{
	int returnValue;
	
	int noOfNode;
	node_type **elt;
	int *level;

	node_type **newElt;
	int *newLevel;
	
	noOfNode = aSetNode->noOfNode;
	elt = aSetNode->elt;
	level = aSetNode->level;
	
	if (noOfNode == 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = 1;
		
		(*aNode) = elt[noOfNode-1];
		(*aLevel) = level[noOfNode-1];
		
		// remove
		newElt = (node_type **) malloc(sizeof(node_type)*(noOfNode-1));
		newLevel = (int *) malloc(sizeof(int)*(noOfNode-1));
		
		memcpy(newElt, elt, sizeof(node_type)*(noOfNode-1));
		memcpy(newLevel, level, sizeof(int)*(noOfNode-1));
		//for (i = 0; i < noOfNode-1; i++)
		//{
		//	newElt[i] = elt[i];
		//	newLevel[i] = level[i];
		//}
		
		free(elt);
		free(level);
		
		aSetNode->noOfNode = noOfNode-1;

		aSetNode->elt = newElt;
		aSetNode->level = newLevel;
	}
	
	return returnValue;
}


void noOfNodeRtree(node_type *root, int *noOfNode, rtree_info *aInfo)
{
	int i;

	int stop;

	if (root->attribute == LEAF)
	{
		(*noOfNode)++;
	}
	else
	{
		(*noOfNode)++;
		stop = aInfo->M - root->vacancy;
		for (i = 0; i < stop ; i++)
		{
			noOfNodeRtree(root->ptr[i], noOfNode, aInfo);
		}
	}
}

int rtree_nodeSize(rtree_info *aInfo)
{
	int returnValue;

	returnValue = 16 + 4*aInfo->M + aInfo->dim*8;

	return returnValue;
}

// 1: consistent
// 0: inconsistent
int rtree_checkConsistent(node_type *root, rtree_info *aInfo)
{
	int i;

	int returnValue;
	int childConsistent;
	node_type *aChildNode;

	int stop;

	if (root->attribute == LEAF)
	{
		returnValue = 1;
	}
	else
	{

		stop = aInfo->M - root->vacancy;

		returnValue = 1;
		for (i = 0; i < stop; i++)
		{
			aChildNode = root->ptr[i];

			if (aChildNode->parent != root)
			{
				returnValue = 0;
				break;
			}

			childConsistent = rtree_checkConsistent(aChildNode, aInfo);
			if (childConsistent == 0)
			{
				returnValue = 0;
				break;
			}
			else
			{
				//returnValue = 1;
			}
		}
	}

	return returnValue;
}

/****************************/
/* Start Utility procedures */
/****************************/

void initialize(config_type *config, char *configFile, rtree_info *aInfo)
{ 
	FILE *fp;
	int string_len;
	
	//fp = fopen(CONFIG_FILE, "r");
	fp = fopen(configFile, "r");
	
	fscanf(fp, "m=%d\n", &(aInfo->m));
	fscanf(fp, "M=%d\n", &(aInfo->M));
	fscanf(fp, "dim=%d\n", &(aInfo->dim));
	fscanf(fp, "reinsert_p=%d\n", &(aInfo->reinsert_p));
	fscanf(fp, "no_histogram=%d\n",&(aInfo->no_histogram));
	
	/*
	fgets(config->nodefile, FILENAME_MAX, fp);
	string_len = strlen(config->nodefile);
	config->nodefile[string_len-1] = '\0';
	
	 fgets(config->rootfile, FILENAME_MAX, fp);
	 string_len = strlen(config->rootfile);
	 config->rootfile[string_len-1] = '\0';
	*/
	
	fgets(config->positionfile, FILENAME_MAX, fp);
	string_len = strlen(config->positionfile);
	config->positionfile[string_len-1] = '\0';
	
	fgets(config->queryfile, FILENAME_MAX, fp);
	string_len = strlen(config->queryfile);
	config->queryfile[string_len-1] = '\0';
	
	sprintf(config->save_tree_file, "%s.rstree", config->positionfile);
	
} 
/* initialize */


void tree_node_allocate(node_type **node, rtree_info *aInfo)
{
	
	(*node) = (node_type *)malloc(sizeof(node_type));
	(*node)->a = (R_TYPE *)malloc(sizeof(R_TYPE) * aInfo->dim);
	(*node)->b = (R_TYPE *)malloc(sizeof(R_TYPE) * aInfo->dim);
	(*node)->ptr = (node_type **)malloc(sizeof(node_type *) * aInfo->M);
	
}


void tree_node_deallocate(node_type *free_node)
{
	
	free(free_node->a);
	free(free_node->b);
	free(free_node->ptr);
	
	free(free_node);
	
}

void node_free(node_type *aNode, rtree_info *aInfo)
{
	int stop;
	int i;

	if (aNode->attribute != LEAF)
	{
		// free children
		stop = aInfo->M - aNode->vacancy;
		for (i = 0; i < stop ; i++)
		{
			//tree_node_deallocate(root->ptr[i]);
			node_free(aNode->ptr[i], aInfo);
		}
	}

	tree_node_deallocate(aNode);
}


void cal_MBR_node_node(R_TYPE *new_a, R_TYPE *new_b, node_type *node1, node_type *node2, rtree_info *aInfo)
{
	int i;
	
	for (i=0; i<aInfo->dim; i++) {
		
		if (node1->a[i] < node2->a[i])
			new_a[i] = node1->a[i];
		else
			new_a[i] = node2->a[i];
	}
	
	for (i=0; i<aInfo->dim; i++) {
		
        if (node1->b[i] > node2->b[i])
			new_b[i] = node1->b[i];
        else
			new_b[i] = node2->b[i];
	}
	
	return;
	
}


double cal_vol(R_TYPE *a, R_TYPE *b, rtree_info *aInfo)
{
	int i;
	double volume = 1.0;
	
	for (i=0; i<aInfo->dim; i++)
		volume = volume * (double)(b[i] - a[i]);
	
	
	return(volume);
	
}


double cal_overlap(node_type *node1, node_type *node2, rtree_info *aInfo)
{
	double overlap;
	
	int i;
	
	overlap = 1.0;
	for (i=0; i<aInfo->dim; i++) {
		
		/* 6 possible cases */
		
		if (node2->a[i] > node1->b[i] || node1->a[i] > node2->b[i]) {
			
			overlap = 0.0;
			break;
			
		}
		else if (node2->a[i] <= node1->a[i]) {
			
			if (node2->b[i] <= node1->b[i]) {
				
				// a2, a1, b2, b1
				
				overlap = overlap * (node2->b[i] - node1->a[i]);
				
			}
			else {
				
				// a2, a1, b1, b2
				
				overlap = overlap * (node1->b[i] - node1->a[i]);
				
			}
		}
		else if (node1->a[i] < node2->a[i]) {
			
			if (node2->b[i] <= node1->b[i]) {
				
				// a1, a2, b2, b1
				
				overlap = overlap * (node2->b[i] - node2->a[i]);
				
			}
			else {
				
				// a1, a2, b1, b2
				
				overlap = overlap * (node1->b[i] - node2->a[i]);
				
			}
			
		}
		
	}
	
	
	return(overlap);
	
}


double cal_overlap_sum(node_type *node, int index_skip,  node_type *parent_node, rtree_info *aInfo)
{
	double overlap;
	
	int i, stop;
	
	
	overlap = 0.0;
	stop = aInfo->M - parent_node->vacancy;
	for (i=0; i<stop; i++) {
		
		if (i == index_skip) continue;
		
		overlap = overlap + cal_overlap(parent_node->ptr[i], node, aInfo);
		
	}
	
	
	return(overlap);
	
}


double Dist2(node_type *node1, node_type *node2, rtree_info *aInfo)
{
	double distance;
	double diff;
	float *point1, *point2;
	
	int i;
	
	point1 = (float *)malloc(sizeof(float) * aInfo->dim);
	point2 = (float *)malloc(sizeof(float) * aInfo->dim);
	
	for (i=0; i<aInfo->dim; i++) {
		point1[i] = (float) ((node1->b[i] - node1->a[i])/2.0);
		point2[i] = (float) ((node2->b[i] - node2->a[i])/2.0);
	}
	
	distance = 0.0;
	for (i=0; i<aInfo->dim; i++) {
		//distance = distance + pow((double)(point1[i] - point2[i]), 2.0);
		diff = point1[i] - point2[i];
		distance = distance + diff*diff;
	}
	
	//return(pow(distance, 0.5));
	return distance;
	
}


/**************************/
/* End Utility procedures */
/**************************/


int least_overlap_enlarge(node_type *parent_node, node_type  *data_node, rtree_info *aInfo)
{
	double new_overlap_diff, old_overlap, new_overlap; 
	double vol_at_index, new_vol, min_overlap_diff;
	int index;
	node_type *temp_node;
	
	int i, stop;
	
	
	tree_node_allocate(&temp_node, aInfo);
	
	stop = aInfo->M - parent_node->vacancy;
	for(i=0; i<stop; i++) {
		
        /* original overlap */
		
        old_overlap = cal_overlap_sum(parent_node->ptr[i], i, parent_node, aInfo);
		
        /* overlap after enlargement */
		
        cal_MBR_node_node(temp_node->a, temp_node->b, parent_node->ptr[i], data_node, aInfo);
		
        new_overlap = cal_overlap_sum(temp_node, i, parent_node, aInfo);
		
        /* check if index is needed to updated */
        new_overlap_diff = new_overlap - old_overlap;
		
		if (i == 0) {
			
			index = i;
			min_overlap_diff = new_overlap_diff;   
			
		}
		else {
			if (new_overlap_diff < min_overlap_diff) {
				index = i;
				min_overlap_diff = new_overlap_diff;
			}
			else if (new_overlap_diff == min_overlap_diff) {
				
				vol_at_index = cal_vol(parent_node->ptr[index]->a, parent_node->ptr[index]->b, aInfo);
				new_vol = cal_vol(temp_node->a, temp_node->b, aInfo);
				if(new_vol < vol_at_index) {
					
					index=i;
					
				}
			}
       	}
        
	}  /* end i */
	
	tree_node_deallocate(temp_node);
	
	
	return (index);
	
}



/********************************************************/
/* least_area_enlarge():                                */
/* Select the node which will cause least bounding      */
/* box enlargement when insert an entry to it           */
/********************************************************/

int least_area_enlarge(node_type *parent_node, node_type *data_node, rtree_info *aInfo)
{ 
	double new_vol_diff, old_vol, new_vol;
	double vol_at_index, min_vol_diff;
	int index;
	R_TYPE *temp_a, *temp_b;
	int i, stop;
	
	temp_a = (R_TYPE *)malloc(sizeof(R_TYPE) * aInfo->dim);
	temp_b = (R_TYPE *)malloc(sizeof(R_TYPE) * aInfo->dim);
	
	
	stop = aInfo->M - parent_node->vacancy;
	for(i=0; i<stop; i++) { 
		
		/* original volume */
		
		old_vol = cal_vol(parent_node->ptr[i]->a, parent_node->ptr[i]->b, aInfo);            	
		
		/* volume after enlargement */
		
		cal_MBR_node_node(temp_a, temp_b, parent_node->ptr[i], data_node, aInfo);
		
		new_vol = cal_vol(temp_a, temp_b, aInfo);
		
		/* check if index is needed to updated */
        new_vol_diff = new_vol - old_vol;
		
		if (i == 0) {
			
			index = i;
			min_vol_diff = new_vol_diff;
			vol_at_index = new_vol;
			
		}
		else {
			if (new_vol_diff < min_vol_diff) { 
				index = i;
				min_vol_diff = new_vol_diff;
				vol_at_index = new_vol;
			}
			else if (new_vol_diff == min_vol_diff) { 
				if(new_vol < vol_at_index) {
					index=i;
					vol_at_index = new_vol;
				}
			}
		}
		
	}  /* end i */
	
	free(temp_a);
	free(temp_b);
	
	return (index);
	
} 
/* least_enlarge */




/**********************************************************/
/* choose_leaf():                                         */
/* Select a leaf node in which to place a new index entry */
/* current_level is the level of the current_node	  */
/**********************************************************/

int choose_leaf(node_type **node_found, node_type *current_node, int 
				current_level, node_type *data_node, rtree_info *aInfo) 
{
	
	int child_chosen, level_found;
	
	/*******/
	/* CL1 */
	/*******/
	
	/**********************************************************/
	/* Initialise: 			     		    */  
	/* Set N to be the root node(already done in insert_node) */
	/**********************************************************/
	
	/*  It has been already done in insert()  */
	
	/*******/
	/* CL2 */
	/*******/
	
	/**************/
	/* Leaf check */
	/**************/
	
	if(current_node->attribute == ROOT && (current_node->ptr[0]==NULL || current_node->ptr[0]->attribute==LEAF)) { 
		
	/************************************************************
	*node_found is the root of the tree because the root is 
	the only internal node of the tree at that time.
		*************************************************************/
		
		return(current_level);		// root is at level 0				
		
	}
	
	if (current_node->ptr[0]->attribute == LEAF) {
        *node_found = current_node;
        return(current_level);
	}
	
	/*******/
	/* CL3 */
	/*******/
	
	/******************/
	/* Choose subtree */
	/******************/
	
	if (current_node->ptr[0]->ptr[0]->attribute != LEAF) {
		
		child_chosen = least_area_enlarge(current_node, data_node, aInfo);
		
	}
	else {
		
		child_chosen = least_overlap_enlarge(current_node, data_node, aInfo);
		
	}
	
	/*******/
	/* CL4 */
	/*******/
    
	/***********************/
	/* Descend recursively */
	/***********************/
	
	current_node = current_node->ptr[child_chosen];
	level_found = choose_leaf(node_found, current_node, current_level+1, data_node, aInfo);
	
	
	return(level_found);
	
} /* choose_leaf */



void adjust_MBR(node_type *node_inserted, rtree_info *aInfo)
{
	node_type *node = node_inserted;
	int i, flag;
	
	while(node->attribute != ROOT) {
		
		flag = FALSE;
		for(i=0; i < aInfo->dim; i++) { 
			if(node->parent->a[i] > node->a[i]) {
				node->parent->a[i] = node->a[i];
				flag = TRUE;
			}
			if(node->parent->b[i] < node->b[i]) {
				node->parent->b[i] = node->b[i];
				flag = TRUE;
			}
		}
		
		if (flag == FALSE) break;
		
		node = node->parent;
		
	}
	
	return;
	
} /* adjust_MBR */



void adjust_MBR_delete(node_type *node_inserted, rtree_info *aInfo)
{
	node_type *node = node_inserted, *parent;
	int j, stop;
	
	while(node->attribute != ROOT) {
		
		parent = node->parent;
		stop = aInfo->M - parent->vacancy;
		
		for (j=0; j<aInfo->dim; j++) {
			parent->a[j] = parent->ptr[0]->a[j];
			parent->b[j] = parent->ptr[0]->b[j];
		}
		//printf("stop %d\n", stop);
		for (j=1; j<stop; j++) {
			cal_MBR_node_node(parent->a, parent->b, parent, parent->ptr[j], aInfo);
		}
		
		//printf("3\n");
		//   if (flag == FALSE) break;
		
        node = parent;
        
	}
	
	return;
	
} /* adjust_MBR_delete */     



void swap(int *sorted_index, double *value, int i, int j)
{ 
	int temp_index;
	double temp_value; 
	
	temp_value = value[i];  
	value[i] = value[j];
	value[j] = temp_value;
	
	temp_index = sorted_index[i];
	sorted_index[i] = sorted_index[j];
	sorted_index[j] = temp_index;
	
	return;
	
}




void sort_entries(int *sorted_index, node_type **overnode, int axis_sort, rtree_info *aInfo)
{
	int i, start, end;
	double *value;
	
	value = (double *)malloc(sizeof(double) * (aInfo->M+1));
	
	
	for (i=0; i<aInfo->M+1; i++) {
		sorted_index[i] = i;
		value[i] = (double)overnode[i]->a[axis_sort];
	}
	
	
	quicksort(sorted_index, value, 0, aInfo->M);
	
	i = 0;
	while (i<aInfo->M) {
		
		if (value[i] == value[i+1]) {
			
			start = i;
			
			while (i < aInfo->M && value[i] == value[i+1]) {
				
				value[i] = (double)overnode[sorted_index[i]]->b[axis_sort];
				i ++;
				
			}
			
			value[i] = (double)overnode[sorted_index[i]]->b[axis_sort];
			end = i;	
			
			
			
			if ((end - start) > 1) {
				quicksort(sorted_index, value, start, end);
			}
			else {
				if (value[end] < value[start]) {
					swap(sorted_index, value, start, end);
				}
			}
		}
		
		
		i++;
		
	}
	
	free(value);
	
	
	return;
	
}


int ChooseSplitAxis(node_type **overnode, rtree_info *aInfo)
{
	int axis_chosen, *sorted_index;
	node_type *group1, *group2;
	double new_margin_value, min_margin_value;
	
	
	int i, j, k, l, stop, cut;
	
	
	sorted_index = (int *)malloc(sizeof(int) * (aInfo->M+1));
	tree_node_allocate(&group1, aInfo);
	tree_node_allocate(&group2, aInfo);
	
	
	for (i=0; i<aInfo->dim; i++) {
		
		sort_entries(sorted_index, overnode, i, aInfo);   // sort the entries by axis i
		
		
		new_margin_value = 0.0;
		stop = aInfo->M - 2*aInfo->m + 1;
		for (k=0; k<stop; k++) {
			
			for (l=0; l<aInfo->dim; l++) {
				group1->a[l] = overnode[sorted_index[0]]->a[l];
				group1->b[l] = overnode[sorted_index[0]]->b[l];
				group2->a[l] = overnode[sorted_index[aInfo->M]]->a[l];
				group2->b[l] = overnode[sorted_index[aInfo->M]]->b[l];
			}        
			
			
			j = 0;
			cut = aInfo->m + k;
			while (j < aInfo->M+1) {
				
				if (j < cut) {
					
					cal_MBR_node_node(group1->a, group1->b, group1, overnode[sorted_index[j]], aInfo);
					
				}
				else {
					
					cal_MBR_node_node(group2->a, group2->b, group2, overnode[sorted_index[j]], aInfo);
					
				}
				
				j++;
				
			}
			
			for (l=0; l<aInfo->dim; l++) {
				
				new_margin_value = new_margin_value + group1->b[l] - group1->a[l];
				new_margin_value = new_margin_value + group2->b[l] - group2->a[l];
				
			}
		}
		
		if (i == 0) {
			
			axis_chosen = i;
			min_margin_value = new_margin_value;
		}
		else {
			
			if (new_margin_value < min_margin_value) {
				
				axis_chosen = i;
				min_margin_value = new_margin_value;
			}
			
		}	
	}
	
	tree_node_deallocate(group1);
	tree_node_deallocate(group2);
	free(sorted_index);
	
	return(axis_chosen);
	
} /* ChooseSplitAxis */



void ChooseSplitIndex(node_type **overnode, int axis_chosen, node_type 
					  *group1_chosen, node_type *group2_chosen, rtree_info *aInfo)
{
	int split_index, *sorted_index;
	node_type *group1, *group2;  
	double new_overlap_value, min_overlap_value;
	double vol_at_index, new_vol;
	
	int i, j, k, stop, cut;
	
	
	sorted_index = (int *)malloc(sizeof(int) * (aInfo->M+1));
	
	tree_node_allocate(&group1, aInfo);
	tree_node_allocate(&group2, aInfo);
	
	
	sort_entries(sorted_index, overnode, axis_chosen, aInfo);   // sort the entries by the axis, axis_chosen
	
	
	new_overlap_value = 0.0;  
	stop = aInfo->M - 2*aInfo->m + 1;
	for (k=0; k<=stop; k++) {
		
		for (i=0; i<aInfo->dim; i++) {
			group1->a[i] = overnode[sorted_index[0]]->a[i];
			group1->b[i] = overnode[sorted_index[0]]->b[i];
			group2->a[i] = overnode[sorted_index[aInfo->M]]->a[i];
			group2->b[i] = overnode[sorted_index[aInfo->M]]->b[i];
		}
		
		
		cut = aInfo->m + k; 
		for (j=0; j<aInfo->M+1; j++) {
			
			if (j < cut) {
				
				cal_MBR_node_node(group1->a, group1->b, group1, overnode[sorted_index[j]], aInfo);
				
			}
			else {
				
				cal_MBR_node_node(group2->a, group2->b, group2, overnode[sorted_index[j]], aInfo);
				
			}
			
		}
		
		new_overlap_value = cal_overlap(group1, group2, aInfo);
		
        
		if (k == 0) {   
			
			split_index = k;
			min_overlap_value = new_overlap_value;
			vol_at_index = cal_vol(group1->a, group1->b, aInfo) + cal_vol(group2->a, group2->b, aInfo);
			
		}
		else {   
			
			if (new_overlap_value < min_overlap_value) {
				
				split_index = k;
				min_overlap_value = new_overlap_value;
				vol_at_index = cal_vol(group1->a, group1->b, aInfo) + cal_vol(group2->a, group2->b, aInfo);
				
			}
			else {
				
				new_vol = cal_vol(group1->a, group1->b, aInfo) + cal_vol(group2->a, group2->b, aInfo);
				if (new_vol < vol_at_index) {
					split_index = k;
				}
				
			}
		}
		
	}
	
	
	for (i=0; i<aInfo->dim; i++) {
		group1_chosen->a[i] = overnode[sorted_index[0]]->a[i];
		group1_chosen->b[i] = overnode[sorted_index[0]]->b[i];
		group2_chosen->a[i] = overnode[sorted_index[aInfo->M]]->a[i];
		group2_chosen->b[i] = overnode[sorted_index[aInfo->M]]->b[i];
	}
	
	
	cut = aInfo->m + split_index;
	
	for (j=0; j<aInfo->M+1; j++) {
		
		
		if (j < cut) {
			
			group1_chosen->ptr[j] = overnode[sorted_index[j]];
			
			overnode[sorted_index[j]]->parent = group1_chosen;
			cal_MBR_node_node(group1_chosen->a, group1_chosen->b, group1_chosen, overnode[sorted_index[j]], aInfo);
		}
		
		
		else {
			
			group2_chosen->ptr[j-cut] = overnode[sorted_index[j]];
			overnode[sorted_index[j]]->parent = group2_chosen;
			cal_MBR_node_node(group2_chosen->a, group2_chosen->b, group2_chosen, overnode[sorted_index[j]], aInfo);
		}
		
	}       
	
	
	group1_chosen->vacancy = aInfo->M - cut;
	group2_chosen->vacancy = cut - 1;	// M - (M+1 - cut);
	
	tree_node_deallocate(group1);
	tree_node_deallocate(group2);
	free(sorted_index);
	
	
	return;
	
} /* ChooseSplitIndex() */




void split(node_type *splitting_node, node_type *extra_node, node_type *node1, node_type *node2, rtree_info *aInfo) 
{
	node_type **overnode;
	int axis_chosen;
	
	int i;
	
	overnode = (node_type **)malloc((aInfo->M+1) * sizeof(node_type *));
	for (i=0; i<aInfo->M; i++) {
        overnode[i] = splitting_node->ptr[i];
	}
	
	overnode[aInfo->M] = extra_node;
	
	for(i=0; i < aInfo->M; i++) {
        node1->ptr[i]=NULL;
        node2->ptr[i]=NULL;
	}
	
	axis_chosen = ChooseSplitAxis(overnode, aInfo);
	
	ChooseSplitIndex(overnode, axis_chosen, node1, node2, aInfo);
	
	node1->attribute = NODE;
	node1->parent = splitting_node->parent;
	node1->id = NO_ID;
	
	node2->attribute = NODE;
	node2->parent = splitting_node->parent;
	node2->id = NO_ID;
	
	free(overnode);
	
	return;
	
} /* split() */



void adjust_tree(node_type *splitting_node, int over_level, int old_level, node_type 
				 *node1, node_type *node2, node_type *root, rtree_info *aInfo) 
{
	int split_child_no = 0;
	node_type *split_parent;
	
	int i, j;
	
	if(splitting_node->attribute == ROOT) { 
		
		/* The splitting node is the root */
		
		node1->parent = root;
		node2->parent = root;
		root->ptr[0] = node1;
		root->ptr[1] = node2;
		for (j=2; j<aInfo->M; j++) 
			root->ptr[j] = NULL;
		root->parent = NULL;
		root->vacancy=aInfo->M-2;
		root->attribute=ROOT;
		
		cal_MBR_node_node(root->a, root->b, node1, node2, aInfo);
		
		aInfo->extra_level++;
	}
	else { 
		
		/* The splitted is an intermediate node */
		
		split_parent = splitting_node->parent;
		
        for(i=0; i < aInfo->M; i++) {
			if(split_parent->ptr[i] == splitting_node) { 
				split_child_no = i;
				break;
			}
        }
		
		tree_node_deallocate(splitting_node);	
		
		/* insert first node */
		split_parent->ptr[split_child_no] = node1;
		node1->parent = split_parent;
		
		adjust_MBR_delete(node1, aInfo);
		
		/* insert second node */
		
		if(split_parent->vacancy != 0) { 
			
			/* no need to split again */
           	split_parent->ptr[aInfo->M - split_parent->vacancy] = node2;
			
			node2->parent = split_parent;
			split_parent->vacancy--;
			
			//		cal_MBR_node_node(split_parent->a, split_parent->b, node1, split_parent);
			cal_MBR_node_node(split_parent->a, split_parent->b, node2, split_parent, aInfo);
			
			adjust_MBR(split_parent, aInfo);
			
		}
       	else { 
			
			/* need to split again */
			overflow(split_parent, over_level-1, old_level, node2, root, aInfo);
			
		}
		
	}
	
	
	
	return;
	
} /* adjust_tree */



void choose_leaf_level(node_type **node_found, node_type *current_node, int 
					   current_level, node_type *inserted_node, int desired_level, rtree_info *aInfo) 
{
	
	int child_chosen;
	
	if (current_level == desired_level) {
        *node_found = current_node;
		//printf("7\n");
		
        return;
	}
	
	//printf("current_level %d desired_level %d\n", current_level, desired_level);
	
	//printf("current_node->ptr[0]->ptr[0] %d\n", current_node->ptr[0]->ptr[0]);
	//if (current_node->ptr[0]->ptr[0]->attribute != LEAF) {
	if (current_node->ptr[0]->attribute != LEAF) {
		
		//printf("8\n");
        child_chosen = least_area_enlarge(current_node, inserted_node, aInfo);
		
	}
	else {
        
		//printf("9\n");
        child_chosen = least_overlap_enlarge(current_node, inserted_node, aInfo);
		
	}
	
	//printf("5\n");
	
	current_node = current_node->ptr[child_chosen];
	//printf("current_level %d desired_level %d\n", current_level, desired_level);
	choose_leaf_level(node_found, current_node, current_level+1, inserted_node, desired_level, aInfo);
	
	
	return;
	
} /* choose_leaf_level */

void reinsert_level(node_type *root, node_type *insertNode, int insertLevel, rtree_info *aInfo)
{
	node_type *node_found;

	int realLevel;
	
	node_found = root;

	realLevel = insertLevel+aInfo->extra_level;
	if (realLevel < 0)
	{
		realLevel = 0;
	}
	
	if (insertNode->attribute == LEAF)
	{
		// ignore the level
		choose_leaf(&node_found, root, 0, insertNode, aInfo);
	}
	else
	{
		//choose_leaf_level(&node_found, root, 0, overnode[sorted_index[i]], over_level);
		choose_leaf_level(&node_found, root, 0, insertNode, realLevel, aInfo);
		//printf("2\n");
	}
	
	/* Test whether the node has room or not */
	if(node_found->vacancy!=0) {
		
		/* have room to insert the entry */
		//printf("3\n");
		
		insertNode->parent = node_found;
		
		node_found->ptr[aInfo->M - node_found->vacancy] = insertNode;
		
		node_found->vacancy--;
		
		adjust_MBR(insertNode, aInfo);
		//printf("4\n");
	}
	else {
		
		overflow(node_found, insertLevel, insertLevel, insertNode, root, aInfo);
		
	}
}

void reinsert(node_type *over_node, int over_level, node_type *extra_node, 
			  node_type *root, rtree_info *aInfo) 
{
	node_type **overnode;
	double *value; 
	int *sorted_index;
	
	int i, start, stop;
	
	value = (double *)malloc(sizeof(double) * (aInfo->M+1));
	sorted_index = (int *)malloc(sizeof(int) * (aInfo->M+1));
	
	overnode = (node_type **)malloc((aInfo->M+1) * sizeof(node_type *));
	for (i=0; i<aInfo->M; i++) {
        overnode[i] = over_node->ptr[i];
	}
	
	overnode[aInfo->M] = extra_node;
	overnode[aInfo->M]->parent = over_node;
	
	for (i=0; i<aInfo->M+1; i++) {
		value[i] = Dist2(overnode[i], over_node, aInfo);
		sorted_index[i] = i;
	}
	
	quicksort(sorted_index, value, 0, aInfo->M);
	
	for (i=0; i<aInfo->dim; i++) {
		over_node->a[i] = overnode[sorted_index[0]]->a[i];
		over_node->b[i] = overnode[sorted_index[0]]->b[i];
	}
	over_node->ptr[0] = overnode[sorted_index[0]];
	
	//printf("1\n");
	
	stop = aInfo->M+1 - aInfo->reinsert_p;
	
	for (i=1; i<stop; i++) {
		over_node->ptr[i] = overnode[sorted_index[i]];
		cal_MBR_node_node(over_node->a, over_node->b, over_node, overnode[sorted_index[i]], aInfo);
	}
	for (i=stop; i<aInfo->M; i++)
		over_node->ptr[i] = NULL;
	
	over_node->vacancy = aInfo->reinsert_p - 1;
	
	adjust_MBR_delete(over_node, aInfo);
	
	start = aInfo->M + 1 - aInfo->reinsert_p;
	
	for (i=start; i<aInfo->M+1; i++) {
		
		//
		reinsert_level(root, overnode[sorted_index[i]], over_level, aInfo);
		
	}        
	
	
	return;
	
} /* reinsert */





void overflow(node_type *over_node, int over_level, int old_level, node_type
			  *extra_node, node_type *root, rtree_info *aInfo)
{
	node_type *node1, *node2;
	
	if (over_level < old_level && over_level != 0) {
		reinsert(over_node, over_level, extra_node, root, aInfo);
		
	}
	else {
		
		tree_node_allocate(&node1, aInfo);
		tree_node_allocate(&node2, aInfo);
		
        split(over_node, extra_node, node1, node2, aInfo);
		
        adjust_tree(over_node, over_level, old_level, node1, node2, root, aInfo);
		
		//printf("overflow, go out adjust, extra_node->id %d\n", extra_node->id);
		
		
	}
	
	
	return;
	
}




void insert_node_MBR(node_type *root, R_TYPE *a, R_TYPE *b, int seq_id, rtree_info *aInfo)
{
	node_type *node_found, *new_node, *data_node;
	int level_found;
	
	
	int i;
	
	/******/
	/* I1 */
	/******/
	
	
	node_found = root;
	aInfo->extra_level=0;
	
	tree_node_allocate(&data_node, aInfo);
	for (i=0; i<aInfo->dim; i++) {
		data_node->a[i] = a[i];
		data_node->b[i] = b[i];
	}
	level_found = choose_leaf(&node_found, root, 0, data_node, aInfo);
	/* Now, node_found is the pointer to the leaf chosen */
	
	
	/******/
	/* I2 */
	/******/
	
	/********************************************************/
	/* Add record to leaf node:                            */
	/* If L has room for another entry, install the entry   */
	/* Otherwise invoke split() to split the node */
	/********************************************************/  
	
	/* Make a leaf node */
	
	tree_node_allocate(&new_node, aInfo);
	
	
	for(i=0; i < aInfo->dim; i++) { 
       	new_node->a[i]=a[i];
       	new_node->b[i]=b[i];
	}
	new_node->id = seq_id;		//data[dim] is the seq_id   
	new_node->attribute=LEAF;
	new_node->vacancy=aInfo->M;
	new_node->parent=node_found;
	for(i=0; i < aInfo->M; i++)
		new_node->ptr[i]=NULL;
	
	/* Test whether the node has room or not */
	if(node_found->vacancy!=0) { 
		
		//printf("insert new->id %d\n", seq_id);
		
		/* have room to insert the entry */
		
		node_found->ptr[aInfo->M - node_found->vacancy] = new_node;
		
		node_found->vacancy--;
		
		adjust_MBR(new_node, aInfo);
		
	}
	else {
		
		overflow(node_found, level_found, level_found+1, new_node, root, aInfo);
		
	}
	
	
	return;
	
} /* insert_node */



void insert_node(node_type *root, R_TYPE *data, int seq_id, rtree_info *aInfo)
{
	insert_node_MBR(root, data, data, seq_id, aInfo);
} /* insert_node */




void remove_childIndex(node_type **childList, int childIndex, rtree_info *aInfo)
{
	node_type **tempList;
	
	if (childIndex != aInfo->M-1)
	{
		tempList = (node_type **) malloc(sizeof(node_type *)*aInfo->M);
		memcpy(tempList, childList, sizeof(node_type *)*aInfo->M);
		
		memcpy(&(childList[childIndex]), &(tempList[childIndex+1]), sizeof(node_type *)*(aInfo->M-childIndex-1));
		free(tempList);
	}
}

int findLevel(node_type *aNode)
{
	int returnValue;

	node_type *tempNode;

	tempNode = aNode;
	tempNode = tempNode->parent;
	returnValue = 0;
	while (tempNode->attribute != ROOT)
	{
		tempNode = tempNode->parent;
		returnValue++;
	}

	return returnValue;
}

// inputLevel: a level (exactly equal to the level of leaf)
void condenseTree(node_type *root, node_type *aLeafNode, int inputLevel, rtree_info *aInfo)
{
	setNode *aSetNode;
	node_type *aNode;
	node_type *childNode;
	node_type *aParentNode;
	int stop, stopParent;
	
	int i, j;
	//int isSuccess;
	int childIndex;
	int aLevel;
	node_type *aTempNode;
	
	// initialize
	aSetNode = setNode_new();
	aNode = aLeafNode;

	aInfo->extra_level = 0;
	
	aLevel = inputLevel;
	while (aNode->attribute != ROOT)
	{
		// Find Parent entry	
		aParentNode = aNode->parent;
		
		childIndex = -1;
		stop = aInfo->M - aParentNode->vacancy;
		for (i = 0; i < stop; i++)
		{
			if (aParentNode->ptr[i] == aNode)
			{
				childIndex = i;
				break;
			}
		}
		
		if (childIndex == -1)
		{
			printf(" Problem (child->parent != parent->child)!\n");
		}
		
		// Eliminate Underfull node
		if (aNode->attribute != LEAF)
		{
			// non-leaf node
			
			stop = aInfo->M - aNode->vacancy;
			// if aNode underflow
			if (stop < aInfo->m)
			{
				// delete EN from P and add N to aSetNode
				remove_childIndex(aParentNode->ptr, childIndex, aInfo);
				aParentNode->vacancy++;
				
				// insert
				for (i = 0; i < stop; i++)
				{
					aTempNode = aNode->ptr[i];
					setNode_insert(aSetNode, &aTempNode, aLevel);
				}	
			}
		}
		else
		{
			// leaf node
			
			// delete EN from P and add N to aSetNode
			remove_childIndex(aParentNode->ptr, childIndex, aInfo);
			aParentNode->vacancy++;
			
			// free
			tree_node_deallocate(aNode);
		}
		
		
		// adjust covering rectangle
		if (aParentNode->vacancy != aInfo->M)
		{
			stopParent = aInfo->M - aParentNode->vacancy;
			for (j = 0; j < aInfo->dim; j++)
			{
				aParentNode->a[j] = aParentNode->ptr[0]->a[j];
				aParentNode->b[j] = aParentNode->ptr[0]->b[j];
			}
			for (i = 1; i < stopParent; i++)
			{
				cal_MBR_node_node(aParentNode->a, aParentNode->b, aParentNode, aParentNode->ptr[i], aInfo);
			}
			
			adjust_MBR_delete(aParentNode, aInfo);
		}
		
		
		// move up one level in the tree
		aNode = aParentNode;
		
		aLevel--;
	}

	if (aNode->attribute == ROOT)
	{
		// if root

		stop = aInfo->M - aNode->vacancy;

		if (stop == 1)
		{
			// only one child
			childNode = aNode->ptr[0];

			if (childNode->attribute != LEAF)
			{
				// non-leaf node

				// copy childNOde to the root
				memcpy(root->a, childNode->a, sizeof(R_TYPE)*aInfo->dim);
				memcpy(root->b, childNode->b, sizeof(R_TYPE)*aInfo->dim);
				root->id = childNode->id;
				root->vacancy = childNode->vacancy;
				memcpy(root->ptr, childNode->ptr, sizeof(node_type *)*aInfo->M) ;
				//root->parent = ;

				// update the parent of all the children
				stop = aInfo->M - aNode->vacancy;
				for (i = 0; i < stop; i++)
				{
					root->ptr[i]->parent = root;
				}

				// free childNode
				free(childNode->a);
				free(childNode->b);
				free(childNode);

				// level -1
				aInfo->extra_level--;
				
				//printf("Decrement a tree depth...\n");
			}
		}
	}
	
if (aSetNode->noOfNode > 0)
{
	printf("");
}
	// re-insert orphaned entries in aSetNode
/*	isSuccess = setNode_removeLast(aSetNode, &aNode, &aLevel);
	while (isSuccess == 1)
	{
		aLevel = findLevel(aNode);
		reinsert_level(root, aNode, aLevel, aInfo);
		isSuccess = setNode_removeLast(aSetNode, &aNode, &aLevel);
	}
*/
	
	
	for (i = 0; i < aSetNode->noOfNode; i++)
	{
		aNode = aSetNode->elt[i];

		aLevel = aSetNode->level[i];

//if (aNode->id == 3280)
//{
//	int isConsistent;
//	printf("   Checking consistent...\n");
//	isConsistent = rtree_checkConsistent(root, aInfo);
//	printf("      consistent : %d\n", isConsistent);
//	if (isConsistent == 0)
//	{
//		printf("");
//	}
//}

		//aLevel = findLevel(aNode);
		reinsert_level(root, aNode, aLevel, aInfo);
	}

	
	setNode_free(aSetNode);
}



/********************/
/* build_tree():    */
/* build the R-tree */
/********************/

void build_tree(node_type **root, R_TYPE **data, int no_data, rtree_info *aInfo)
{ 
	int i, j;
	
	
	/* make tree root node first */
	
	tree_node_allocate(root, aInfo);
	
	for(i=0; i < aInfo->dim; i++) { 
		(*root)->b[i] = (R_TYPE)(-1 * INT_MAX);
       	(*root)->a[i] = (R_TYPE)(INT_MAX);
	}
	(*root)->id = -3;		//NO_ID
	(*root)->attribute=ROOT;
	(*root)->vacancy=aInfo->M;   
	(*root)->parent=NULL; 
	for(j=0; j < aInfo->M; j++)
		(*root)->ptr[j]=NULL;
	
	
	/* add data to the tree */
	for(i=0; i<no_data; i++) { 
		
#ifdef DEBUG
		if (i+1%1000==0)
			printf("insert data %d\n", i+1);
#endif
		
		if (i == 3)
		{
			printf("");
		}
		
		insert_node(*root, data[i], i, aInfo);  // i is the seq id.
		
		
		free(data[i]);
	}
	
	free(data);
	
} /*build_tree */


int make_data(char *positionfile, R_TYPE ***data, rtree_info *aInfo)
{ 
	int no_data;
	int i,j;
	FILE *fp_position;
	
	fp_position=fopen(positionfile,"r");  
	
	no_data = aInfo->no_histogram;
	
	(*data) = (R_TYPE **)malloc(sizeof(R_TYPE *) * no_data);
	for(i=0; i<no_data; i++)
        (*data)[i] = (R_TYPE *)malloc(sizeof(R_TYPE) * aInfo->dim);
	
	for(i=0; i<no_data; i++)
		for (j=0; j<aInfo->dim; j++)
		{
#ifdef R_FLOAT
			fscanf(fp_position,"%f",&((*data)[i][j]));
#else
			fscanf(fp_position,"%d",&((*data)[i][j]));
#endif
		}
		
		fclose(fp_position);
		
		return(no_data);
		
} /* make_data */



void write_leaf_node(node_type *node, FILE *fp, rtree_info *aInfo)
{
	int i;
	
	for (i = 0; i<aInfo->dim; i++)
#ifdef R_FLOAT
        fprintf(fp, "%f\n", (node->a)[i]);
#else
		fprintf(fp, "%d.0\n", (node->a)[i]);
#endif
	
	for (i = 0; i<aInfo->dim; i++)
#ifdef R_FLOAT
        fprintf(fp, "%f\n", (node->b)[i]);
#else
		fprintf(fp, "%d.0\n", (node->b)[i]);
#endif
	
	fprintf(fp, "%d\n", node->attribute);
	fprintf(fp, "%d\n", node->id);
	fprintf(fp, "%d\n", node->vacancy);
	
	return;
	
}


void write_inter_node(node_type *node, FILE *fp, rtree_info *aInfo)
{
	int i, count;
	
	for (i = 0; i<aInfo->dim; i++) 
#ifdef R_FLOAT
		fprintf(fp, "%f\n", (node->a)[i]);
#else
		fprintf(fp, "%d.0\n", (node->a)[i]);
#endif
	
	for (i = 0; i<aInfo->dim; i++)  
#ifdef R_FLOAT
        fprintf(fp, "%f\n", (node->b)[i]);
#else
		fprintf(fp, "%d.0\n", (node->b)[i]);
#endif
	
	fprintf(fp, "%d\n", node->attribute);
	fprintf(fp, "%d\n", node->vacancy);
	
	count = aInfo->M - node->vacancy;
	for (i=0; i<count; i++) {
		if (node->ptr[i]->attribute != LEAF)
			write_inter_node(node->ptr[i], fp, aInfo);
		else
			write_leaf_node(node->ptr[i], fp, aInfo);
	}
	
	return;
	
}

void save_rtree(node_type *root, char save_tree_file[], rtree_info *aInfo)
{
	FILE *fp;
	
	//fp = fopen(SAVE_RTREE_FILE, "w");
	fp = fopen(save_tree_file, "w");
	if (!fp) {
		printf("Can't write to %s\n", save_tree_file);
		exit(EXIT_FAILURE);
	}
	
	write_inter_node(root, fp, aInfo);
	
	fclose(fp);
	
	return;
	
}



int main3(int argc, char *argv[])
{ 
	rtree_info aInfo;
	int no_data;
	R_TYPE **data;
	config_type config;
	node_type *root;
#ifndef WIN32
	float  userTime, sysTime;
	struct rusage myTime1, myTime2;
#endif
	
	//////////////
	if (argc != 2) {
        printf("Usage: %s <config>\n", argv[0]);
        exit(1);
	}
	//////////////
	
	printf("initialize\n");
	initialize(&config, argv[1], &aInfo);   
	
	printf("make_data\n");
	no_data=make_data(config.positionfile, &data, &aInfo);
	
#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime1);
#endif
	
	printf("build_tree\n");
	build_tree(&root, data, no_data, &aInfo);
	
#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime2);
#endif
	
	printf("save_rtree\n");
	save_rtree(root, config.save_tree_file, &aInfo);
	
#ifndef WIN32
	userTime =
		((float) (myTime2.ru_utime.tv_sec  - myTime1.ru_utime.tv_sec)) +
		((float) (myTime2.ru_utime.tv_usec - myTime1.ru_utime.tv_usec)) * 1e-6;
	sysTime =
		((float) (myTime2.ru_stime.tv_sec  - myTime1.ru_stime.tv_sec)) +
		((float) (myTime2.ru_stime.tv_usec - myTime1.ru_stime.tv_usec)) * 1e-6;
	fprintf(stdout, "User time : %f seconds\n",userTime);
	fprintf(stdout, "System time : %f seconds\n",sysTime);
	fprintf(stdout, "Total time : %f seconds\n",userTime+sysTime);
#endif
	
	return(0);  
} /* main */  


int main4(int argc, char *argv[])
{ 
	rtree_info aInfo;

	int inputLevel;
	int i, j;
	
	int no_data;
	R_TYPE **data;
	config_type config;
	node_type *root;

	NN_type *NNresult;
	int k = 1;
	R_TYPE *query;
	node_type *aLeafNode;
	int count;
#ifndef WIN32
	float  userTime, sysTime;
	struct rusage myTime1, myTime2;
#endif

	
	//////////////
	if (argc != 2) {
		printf("Usage: %s <config>\n", argv[0]);
		exit(1);
	}
	//////////////
	
	// initialize
	printf("initialize\n");
	initialize(&config, argv[1], &aInfo);   
	
	// make data
	printf("make_data\n");
	no_data = make_data(config.positionfile, &data, &aInfo);
	
#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime1);
#endif
	
	// build tree
	printf("build_tree\n");
	build_tree(&root, data, no_data, &aInfo);
	
#ifndef WIN32
	getrusage(RUSAGE_SELF,&myTime2);
#endif
	
	// save tree
	printf("save_rtree\n");
	save_rtree(root, config.save_tree_file, &aInfo);
	
#ifndef WIN32
	userTime =
		((float) (myTime2.ru_utime.tv_sec  - myTime1.ru_utime.tv_sec)) +
		((float) (myTime2.ru_utime.tv_usec - myTime1.ru_utime.tv_usec)) * 1e-6;
	sysTime =
		((float) (myTime2.ru_stime.tv_sec  - myTime1.ru_stime.tv_sec)) +
		((float) (myTime2.ru_stime.tv_usec - myTime1.ru_stime.tv_usec)) * 1e-6;
	fprintf(stdout, "User time : %f seconds\n",userTime);
	fprintf(stdout, "System time : %f seconds\n",sysTime);
	fprintf(stdout, "Total time : %f seconds\n",userTime+sysTime);
#endif
	
	count = 0;
	for (i = 0; i < 100; i++)
	{
		// query
		printf("----- query %d --------\n", i);
		query = (R_TYPE *) malloc(sizeof(R_TYPE)*aInfo.dim);
		memset(query, 0, sizeof(R_TYPE)*aInfo.dim);
		for (j = 0; j < aInfo.dim; j++)
		{
			query[j] = 1;//0.1;
		}
		k_NN_search(root, query, k, &NNresult, &aInfo);

		//
		while (NNresult != NULL)
		{
			if (NNresult->oid != UNDEFINED)
			{
				count++;
				printf("Removing ID=%d (%d pts removed)...\n", NNresult->oid, count);

				aLeafNode = NNresult->pointer;
				inputLevel = NNresult->level;
				condenseTree(root, aLeafNode, inputLevel, &aInfo);
			}

			NNresult = NNresult->next;
		}
	}

	return(0);  
} /* main */  
