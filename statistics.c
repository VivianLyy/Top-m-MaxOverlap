

#include "statistics.h"

statistics *statistics_new()
{
	statistics *returnValue;

	returnValue = (statistics *) malloc(sizeof(statistics));
	memset(returnValue, 0, sizeof(statistics));

	//returnValue->chainLen_min = 1000000;
	//returnValue->noOfCoupleForO_min = 1000000;
	//returnValue->noOfCoupleForP_max = 1000000;

	return returnValue;
}

void statistics_free(statistics **aStat)
{
	free((*aStat));
}


void statistics_rtreeNoOfNodeStorageRootP(statistics *aStat, node_type *rootP, rtree_info *aInfo)
{
	int noOfNode;

	int nodeSize;

	nodeSize = rtree_nodeSize(aInfo);

	noOfNode = 0;
	noOfNodeRtree(rootP, &noOfNode, aInfo);
	aStat->noOfNodeRtreeP = noOfNode;
	aStat->rtreeP_storage = noOfNode*nodeSize;


}


void statistics_rtreeNoOfNodeStorageRootC(statistics *aStat, node_type *rootC, rtree_info *aInfo)
{
	int noOfNode;

	int nodeSize;

	nodeSize = rtree_nodeSize(aInfo);

	noOfNode = 0;
	noOfNodeRtree(rootC, &noOfNode, aInfo);
	aStat->noOfNodeRtreeC = noOfNode;
	aStat->rtreeC_storage = noOfNode*nodeSize;

}

void statistics_overlapTableStatistics(statistics *aStat, mySetSet *overlapTable)
{
	int noOfElt;
	mySet **elt;

	mySet *aSet;

	int count;

	int i;

	noOfElt = overlapTable->noOfElt;
	elt = overlapTable->elt;

	count = 0;
	for (i = 0; i < noOfElt; i++)
	{
		aSet = elt[i];

		count += aSet->noOfElt;
	}

	aStat->averageOverlapLength = count/((double)noOfElt);

	aStat->overlapTableSize = count*4 + noOfElt*4;
}

void statistics_totalStorage(configure *aConfigure, statistics *aStat)
{
	int ALG;
	
	ALG = aConfigure->ALG;

	if ((ALG == 1) || (ALG == 2))
	{
		aStat->totalStorage = aStat->overlapTableSize + aStat->rtreeC_storage + aStat->rtreeP_storage;
	}
	else if (ALG == 3)
	{
		aStat->totalStorage = aStat->overlapTableSize + aStat->rtreeC_storage + aStat->rtreeP_storage;
		aStat->totalStorage += aStat->totalCandidateSize;
	}
	else if (ALG == 4)
	{
		aStat->totalStorage = aStat->rtreeC_storage + aStat->rtreeP_storage;
	}

}



void statistics_influenceValueStatistics(statistics *aStat, mySetSet *resultSet)
{
	int noOfElt;
	mySet **elt;

	mySet *aSet;

	int influenceValue;

	int i;

	noOfElt = resultSet->noOfElt;
	elt = resultSet->elt;

	aStat->max_InfluenceValue = -1;
	aStat->min_InfluenceValue = -1;
	aStat->avg_InfluenceValue = -1;

	for (i = 0; i < noOfElt; i++)
	{
		aSet = elt[i];
		influenceValue = aSet->noOfElt;

		if (aStat->max_InfluenceValue == -1)
		{
			aStat->max_InfluenceValue = influenceValue;
			aStat->min_InfluenceValue = influenceValue;
			aStat->avg_InfluenceValue = influenceValue;
		}
		else
		{
			if (influenceValue > aStat->max_InfluenceValue)
			{
				aStat->max_InfluenceValue = influenceValue;
			}

			if (influenceValue < aStat->min_InfluenceValue)
			{
				aStat->min_InfluenceValue = influenceValue;
			}

			aStat->avg_InfluenceValue += influenceValue;

		}
	}

	aStat->avg_InfluenceValue /= noOfElt;
}

void statistics_updateCandidateSize(statistics *aStat, mySetSet *candidateSet, int size, pointListList *intersectSet)
{
	int i;

	aStat->totalNoOfCandidate += candidateSet->noOfElt;
	aStat->totalCandidateSize += candidateSet->noOfElt*size;

	for (i = 0; i < intersectSet->noOfElt; i++)
	{
		aStat->totalNoOfIntersection += intersectSet->elt[i]->noOfPoint;
	}
}

void statistics_print(statistics *aStat, FILE *filePtr)
{
	fprintf(filePtr, "// overlapTable\n");
	fprintf(filePtr, "%.10f\n", aStat->averageOverlapLength);
	fprintf(filePtr, "%d\n", aStat->overlapTableSize);
	fprintf(filePtr, "// pruning\n");
	fprintf(filePtr, "%d\n", aStat->noOfCircleProcessed);
	fprintf(filePtr, "%d\n", aStat->noOfIntersection);
	fprintf(filePtr, "// Rtree\n");
	fprintf(filePtr, "%d\n", aStat->noOfNodeRtreeP);
	fprintf(filePtr, "%d\n", aStat->rtreeP_storage);
	fprintf(filePtr, "%d\n", aStat->noOfNodeRtreeC);
	fprintf(filePtr, "%d\n", aStat->rtreeC_storage);
	fprintf(filePtr, "// totalStorage\n");
	fprintf(filePtr, "%d\n", aStat->totalStorage);
	fprintf(filePtr, "// influence value\n");
	fprintf(filePtr, "%.10f\n", aStat->max_InfluenceValue);
	fprintf(filePtr, "%.10f\n", aStat->avg_InfluenceValue);
	fprintf(filePtr, "%.10f\n", aStat->min_InfluenceValue);
	fprintf(filePtr, "// apriori\n");
	fprintf(filePtr, "%d\n", aStat->totalNoOfCandidate);
	fprintf(filePtr, "%d\n", aStat->totalCandidateSize);
	fprintf(filePtr, "%d\n", aStat->totalNoOfIntersection);
}

