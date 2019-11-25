/*=================================================================================================
 * N-RMiner version 1.0 - Software to mine interesting Maximal Complete Connected Subsets (N-MCCSs)
 * from multi-relational data containing N-ary relationships.
 *
 * Copyright (C) 2011 Eirini Spyropoulou, Tijl De Bie
 * Department of Engineering Mathematics, University of Bristol, Bristol, UK
 * ------------------------------------------------------------------------------------------------
 *
 * N-RMiner-1.0 licensed under the Non-Profit Open Software License 3.0 (NPOSL-3.0)
 *================================================================================================*/

/** NullModel3Ary.cpp
 *
 *  Author: Tijl De Bie
 *  email: tijl.debie@gmail.com
 **/

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <set>
#include <vector>
#include <time.h>
#include <ctime>
#include <algorithm>
#include <iterator>
#include <cstdlib>
//#include <sys/time.h>
//#include <sys/resource.h>
using namespace std;


///////////////////////////////////////////////
// CLASS NULLMODEL
///////////////////////////////////////////////


class NullModel3Ary
{
public:
	NullModel3Ary(vector<int>* rowSums, vector<int>* colSums, vector<int>* tubSums);
	double probability(int row, int col, int tub) const;
	double logProbability(int row, int col, int tub) const;
	double logProbability(set<int>* rows, set<int>* cols, set<int>* tubs) const;
	double logProbabilityRowsCols(int row, int col) const;
	double logProbabilityRowsTubs(int row, int tub) const;
	double logProbabilityColsTubs(int col, int tub) const;
	double logProbabilityRowsCols(set<int>* rows, set<int>* cols) const;
	double logProbabilityRowsTubs(set<int>* rows, set<int>* tubs) const;
	double logProbabilityColsTubs(set<int>* cols, set<int>* tubs) const;

	double* muq;
	double* lambdaq;
	double* kappaq;
	vector<int>* rowSumsq;
	vector<int>* colSumsq;
	vector<int>* tubSumsq;
	vector<vector<int>*>* rowIndices;
	vector<vector<int>*>* colIndices;
	vector<vector<int>*>* tubIndices;
	int nq;
	int mq;
	int pq;
private:
	void makeUnique(vector<int>* sums, vector<int>* uniqueSums, vector<vector<int>*>* indices);
	double* mu;
	double* lambda;
	double* kappa;
	double* expMu;
	double* expLambda;
	double* expKappa;
	int n;
	int m;
	int p;
};
