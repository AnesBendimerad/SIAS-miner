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

/** DatabaseGenerator.h
 *
 *  Author: Tijl De Bie
 *  email: tijl.debie@gmail.com
 **/

#ifndef DATABASEGENERATOR_H_
#define DATABASEGENERATOR_H_

#endif /* DATABASEGENERATOR_H_ */

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
#include <sys/time.h>
//#include <sys/resource.h>
using namespace std;



///////////////////////////////////////////////
// CLASS DATABASE
///////////////////////////////////////////////


class Database
{
public:
	Database();
	Database(ifstream* myfile);
	~Database();
	void addTransaction(set<int>* itemset);
	set<int>* getTransaction(int i) const;
	set<int>* getSupportingTidset(set<int>* itemset) const;
	int getSupport(set<int>* itemset) const;
	int getNumberOfTransactions() const;
	int getNumberOfItems() const;
	vector<int>* getTransactionSizes() const;
	vector<int>* getItemSizes() const;
	Database* getTransposedDatabase() const;
	void printDatabase() const;
	void printTransaction(int tid) const;
	void insertEntry(int transaction, int item);
private:
	void stringToVector(const string& str,vector<int>& tokens);
	vector<set<int>*>* transactions;
};

///////////////////////////////////////////////
// CLASS NULLMODEL
///////////////////////////////////////////////


class NullModel
{
public:
	NullModel(vector<int>* rowSums, vector<int>* colSums);
	double probability(int row, int col) const;
	double logProbability(int row, int col) const;
	double logProbability(set<int>* rows, set<int>* cols) const;
	double logProbabilityTransaction(int row, set<int>* cols) const;
	double minLogProbabilityTransaction(int row, set<int>* cols) const;

	double* muq;
	double* lambdaq;
	vector<int>* rowSumsq;
	vector<int>* colSumsq;
	vector<vector<int>*>* rowIndices;
	vector<vector<int>*>* colIndices;
	int nq;
	int mq;
private:
	void makeUnique(vector<int>* sums, vector<int>* uniqueSums, vector<vector<int>*>* indices);
	double* mu;
	double* lambda;
	double* expMu;
	double* expLambda;
	int n;
	int m;
};
