Readme (MaxBRNN)
=========================
This package contains all source codes for 
1. MaxOverlap
2. Arrangement
3. Buffer
4. Buffer-Adapt

Usage Step
==========
a. Compilation
	make

b. Execution
	./run

c. Input Files (Format of those files can be found in Appendix)
   1. By default, we have file "conf.txt" for configuration
   2. Data set for O (filename is specified in "conf.txt")
   3. Data set for P (filename is specified in "conf.txt")
d. Output Files (Format of those files can be found in Appendix)
   1. "time.txt" which captures the time statistics of the program
   2. "debug.txt" which captures all important information of the program


Appendix A. Format of Configure file (By Default, the configure file is named as "conf.txt")
------------------------------------

1. no. of o points
2. no. of p points
3. no. of q points
4. data file for O
5. data file for P
6. data file for Q
7. k
8. l
9. alg no.
10. ~~ blank line ~~
11. m
12. M
13. no. of Dim
14. reinsert_p
15. no_histogram (dummy)
16.data file (dummy)
17.query file (dummy)


Appendix B. Format of data file for O or P
------------------------------------------
Each line corresponds to a point.
The format of each line is 
   <weight> <dim 1> <dim 2> ... <dim n>  <--- data point 1
   <weight> <dim 1> <dim 2> ... <dim n>  <--- data point 2
   ...

Appendix C. Format of "time.txt"
------------------------------------------

Each line corresponds to a user time and system time
1. time for building Rtree for O 
2. time for building the NLCs
3. time for building Rtree for P
4. time for building an overlp table
5. time for initialization
6. time for sorting the entires in the overlap table
7. time for the iterative process of the algorithm
8. time for the query point selection process of the algorithm
9. ~~ blank line ~~
10. time for all parts

Appendix D. Format of "debug.txt"
------------------------------------------
Each line corresponds the following information.
1. // overlapTable
2. averageOverlapLength (the average length of the entries in the overlap table)
3. overlapTableSize (the size of the overlap table)
4. // pruning
5. noOfCircleProcessed (the total number of NLCs processed in our algorithm)
6. noOfIntersection (the total number of intersections)
7. // Rtree
8. noOfNodeRtreeP (no. of nodes in R-tree from P)
9. rtreeP_storage (the storage size of R-tree from P)
10.noOfNodeRtreeC (no. of nodes in R-tree from O)
11.rtreeC_storage (the storage size of R-tree from O)
12.// totalStorage
13.int totalStorage (the total storage)
14.// influence value
15.double max_InfluenceValue (the maximum influence value in the output)
16.double avg_InfluenceValue (the average influence value in the output)
17.double min_InfluenceValue (the minimum influence value in the output)
18.// arrangement
19.int totalNoOfCandidate (the total number of candidates for algorithm arrangement)
20.int totalCandidateSize (the total candidate size for algorithm arrangement)
21.int totalNoOfIntersection (the total number of intersection for algorithm arrangement)




