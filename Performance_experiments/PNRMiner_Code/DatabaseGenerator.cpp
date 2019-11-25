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

/** DatabaseGenerator.cpp
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
#include <sys/time.h>
#include "DatabaseGenerator.h"
//#include <sys/resource.h>
using namespace std;


void Database::insertEntry(int transaction, int item)
{
	transactions->at(transaction)->insert(item);
}
//void Database::stringToVector(const string& str,vector<int>& tokens)
//{
//	string delimiters = " ";
//    // Skip delimiters at beginning.
//    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
//    // Find first "non-delimiter".
//    string::size_type pos     = str.find_first_of(delimiters, lastPos);
//
//    while (string::npos != pos || string::npos != lastPos)
//    {
//        // Found a token, add it to the vector.
//        int number;
//
//	string substring = str.substr(lastPos, pos - lastPos);
//	char * charstring = new char[ substring.size() + 1 ];
//	strcpy( charstring, substring.c_str() );
//        sscanf(charstring,"%d",&number);
//	delete [] charstring;
//	charstring = 0;
//        tokens.push_back(number);
//
//        // Skip delimiters.  Note the "not_of"
//        lastPos = str.find_first_not_of(delimiters, pos);
//        // Find next "non-delimiter"
//        pos = str.find_first_of(delimiters, lastPos);
//    }
//}



Database::Database()
{
	transactions = new vector<set<int>*>(0);
}


Database::Database(ifstream* myfile) // Reads all lines until EOF or an 'empty' line.
{
	transactions = new vector<set<int>*>(0);
	string line;
	while (!myfile->eof())
	{
		getline (*myfile,line);
		vector<int>* superItemset = new vector<int>(0);
		
		//stringToVector(line, *superItemset);
		if (superItemset->size() < 3) break;
		
		int length = superItemset->at(2);
		set<int>* itemset = new set<int>(superItemset->begin()+3, superItemset->end());
		assert(length==itemset->size());
		
		addTransaction(itemset);
	}
}


Database::~Database()
{
	for (int i=0; i<transactions->size(); i++)
	{
		set<int>* todel = transactions->at(i);
		delete todel;
		transactions->at(i)=0;
	}
	delete transactions;
	transactions=0;
}


void Database::addTransaction(set<int>* itemset)
{
	transactions->push_back(itemset);
}


set<int>* Database::getTransaction(int i) const
{
	return transactions->at(i);
}


set<int>* Database::getSupportingTidset(set<int>* itemset) const
{
	set<int>* out = new set<int>();
	set<int>::iterator start = itemset->begin();
	set<int>::iterator stop = itemset->end();
	vector<set<int>*>::iterator first;
	int ct=0;
	for (first = transactions->begin() ; first != transactions->end(); first++)
	{
		if ( includes((*first)->begin(),(*first)->end(),start,stop) )
		{
			out->insert(ct);
		}
		ct++;
	}
	return out;
}


int Database::getSupport(set<int>* itemset) const
{
	int out = 0;
	set<int>::iterator start = itemset->begin();
	set<int>::iterator stop = itemset->end();
	vector<set<int>*>::iterator first;
	for (first = transactions->begin() ; first != transactions->end(); first++)
	{
		if ( includes((*first)->begin(),(*first)->end(),start,stop) )
		{
			out++;
		}
	}
	return out;
}


int Database::getNumberOfTransactions() const
{
	return transactions->size();
}


int Database::getNumberOfItems() const
{
	int out = 0;
	for (int i=0; i<getNumberOfTransactions(); i++)
	{
		out = max(out,*max_element(transactions->at(i)->begin(),transactions->at(i)->end()));
	}
	return out+1;
}


vector<int>* Database::getTransactionSizes() const
{
	vector<int>* out = new vector<int>();
	vector<set<int>*>::iterator first;
	for (first=transactions->begin() ; first != transactions->end() ; first++)
	{
		out->push_back((*first)->size());
	}
	return out;
}

vector<int>* Database::getItemSizes() const
{
	int nItems = this->getNumberOfItems();
	vector<int>* out = new vector<int>(nItems);
	for (int i=0; i<nItems; i++) out->at(i)=0;
	vector<set<int>*>::iterator first;
	for (first=transactions->begin() ; first != transactions->end() ; first++)
	{
		for (set<int>::iterator f=(*first)->begin(); f != (*first)->end() ; f++)
		{
			out->at(*f) += 1;
		}
	}
	return out;
}


Database* Database::getTransposedDatabase() const
{
	int nItems = this->getNumberOfItems();
	Database* dbOut = new Database();
	for (int i=0 ; i<nItems ; i++)
	{
		set<int>* singletonItemset = new set<int>();
		singletonItemset->insert(i);
		
		set<int>* tidset = getSupportingTidset(singletonItemset);
		dbOut->addTransaction(tidset);
		
		delete singletonItemset;
	}
	return dbOut;
}


void Database::printDatabase() const
{
	cout << "----Begin Database----" << endl;
	for (int i=0; i<transactions->size(); i++)
	{
		printTransaction(i);
		cout << endl;
	}
	cout << "-----End Database-----" << endl;
}

void Database::printTransaction(int tid) const
{
	assert(tid<transactions->size());
	set<int>* toPrint = transactions->at(tid);
	for (set<int>::iterator first = toPrint->begin() ; first != toPrint->end() ; first++)
	{
		cout << (*first) << " ";
	}
}


NullModel::NullModel(vector<int>* rowSums, vector<int>* colSums)
{
	n=rowSums->size();
	m=colSums->size();
	
	rowSumsq = new vector<int>();
	rowIndices = new vector<vector<int>*>();
	makeUnique(rowSums, rowSumsq, rowIndices);
	
	colSumsq = new vector<int>();
	colIndices = new vector<vector<int>*>();
	makeUnique(colSums, colSumsq, colIndices);
	
	nq = rowSumsq->size();
	mq = colSumsq->size();

	muq = new double[nq]; for (int i=0; i<nq; i++) muq[i]=0;
	lambdaq = new double[mq]; for (int i=0; i<mq; i++) lambdaq[i]=0;
	
	int rowMult[nq];
	for (int i=0; i<nq; i++) rowMult[i]=rowIndices->at(i)->size();
	int colMult[mq];
	for (int j=0; j<mq; j++) colMult[j]=colIndices->at(j)->size();
	
	// Start of the iteration
	int counter = 0;
	double infeasibility=1;
	double factors[10] = {0.1, 0.2, 0.3, 0.4, 0.55, 0.7, 0.85, 1, 1.2, 1.5};
	while (counter < 100 and infeasibility>n*m*1e-12)
	{
		//cout << "Iteration: " << counter+1 << endl;
			// Compute gradient and Hessian diagonal
		double gMuq[nq];
		double gLambdaq[mq];
		double hMuq[nq];
		double hLambdaq[mq];
//		// The following vectors are to speed up the computation of exp(muq[i]+lambdaq[j]) below.
//		// However, it could give numerical difficulties if expMuq is small and expLambdaq is large.
//		double expMuq[nq];
//		double expLambdaq[mq];
//		for (int i=0; i<nq; i++) expMuq[i]=exp(muq[i]);
//		for (int j=0; j<mq; j++) expLambdaq[j]=exp(lambdaq[j]);
		
        // Note: each gMuq[i] should actually be multiplied by rowMult[i] to really be the gradient.
        // The way it is computed now, it is the mistake made for the row sum i.
        // Accordingly, each hMuq[i] should be multiplied by rowMult[i].
        // This is not done as it would be divided away anyway when the step direction is computed.
        // A similar thing holds for the lambda's.
		for (int i=0; i<nq; i++) {gMuq[i]=-rowSumsq->at(i); hMuq[i]=0;}
		for (int j=0; j<mq; j++) {gLambdaq[j]=-colSumsq->at(j); hLambdaq[j]=0;}
		for (int i=0; i<nq; i++)
		{
			for (int j=0; j<mq; j++)
			{
				double v=exp(muq[i]+lambdaq[j]); // Alternatively: double v = expMuq[i]*expLambdaq[j];
				double g = v/(1+v);
				double h = g/(1+v);
				gMuq[i] += g*colMult[j];
				gLambdaq[j] += g*rowMult[i];
				hMuq[i] += h*colMult[j];
				hLambdaq[j] += h*rowMult[i];
			}
		}
		infeasibility = 0;
		for (int i=0; i<nq; i++) infeasibility += gMuq[i]*gMuq[i]*rowMult[i];
		for (int j=0; j<mq; j++) infeasibility += gLambdaq[j]*gLambdaq[j]*colMult[j];
		//cout << "Infeasibility (gradient norm squared): " << infeasibility << endl;
		
			// Compute step direction
		double dMuq[nq];
		double dLambdaq[mq];
		for (int i=0; i<nq; i++) dMuq[i]=-gMuq[i]/hMuq[i];
		for (int j=0; j<mq; j++) dLambdaq[j]=-gLambdaq[j]/hLambdaq[j];
		
			// Find optimal step size
		double bestFactor = 0;
		
		double bestError=0;
		if (true) // Use object function as criterion
		{
			for (int i=0; i<nq; i++) bestError -= rowMult[i]*muq[i]*rowSumsq->at(i);
			for (int j=0; j<mq; j++) bestError -= colMult[j]*lambdaq[j]*colSumsq->at(j);
			for (int i=0; i<nq; i++)
			{
				for (int j=0; j<mq; j++)
				{
					bestError += log(1+exp(muq[i]+lambdaq[j]))*rowMult[i]*colMult[j];
				}
			}
		}
		else // Use gradient norm as criterion
		{
			bestError = infeasibility;
		}

		for (int k=0; k<10; k++)
		{
			double factor = factors[k];
			double muqTry[nq];
			double lambdaqTry[mq];
			for (int i=0; i<nq; i++) muqTry[i]=muq[i]+dMuq[i]*factor;
			for (int j=0; j<mq; j++) lambdaqTry[j]=lambdaq[j]+dLambdaq[j]*factor;
			
			double errorTry = 0;
			if (true) // Use object function as criterion
			{
				for (int i=0; i<nq; i++) errorTry -= rowMult[i]*muqTry[i]*rowSumsq->at(i);
				for (int j=0; j<mq; j++) errorTry -= colMult[j]*lambdaqTry[j]*colSumsq->at(j);
				for (int i=0; i<nq; i++)
				{
					for (int j=0; j<mq; j++)
					{
						errorTry += log(1+exp(muqTry[i]+lambdaqTry[j]))*rowMult[i]*colMult[j];
					}
				}
			}
			else // Use gradient norm as criterion
			{
				double gMuqTry[nq];
				double gLambdaqTry[mq];
				for (int i=0; i<nq; i++) gMuqTry[i]=-rowSumsq->at(i);
				for (int j=0; j<mq; j++) gLambdaqTry[j]=-colSumsq->at(j);
				for (int i=0; i<nq; i++)
				{
					for (int j=0; j<mq; j++)
					{
						double v=exp(muqTry[i])*exp(lambdaqTry[j]);
						double g = v/(1+v);
						gMuqTry[i] += g*colMult[j];
						gLambdaqTry[j] += g*rowMult[i];
					}
				}
				double infeasibilityTry = 0;
				for (int i=0; i<nq; i++) infeasibilityTry += gMuqTry[i]*gMuqTry[i]*rowMult[i];
				for (int j=0; j<mq; j++) infeasibilityTry += gLambdaqTry[j]*gLambdaqTry[j]*colMult[j];
				errorTry = infeasibilityTry;
			}
			
			if (errorTry<bestError)
			{
				bestError=errorTry;
				bestFactor = factor;
			}
			else break;
		}
		// cout << "Mu: "; 
		for (int i=0; i<nq; i++) 
		{
			muq[i] += dMuq[i]*bestFactor;
			//cout << muq[i] << " ";
		}
		//cout << endl;
		//cout << "Lambda: ";
		for (int j=0; j<mq; j++) 
		{
			lambdaq[j] += dLambdaq[j]*bestFactor; 
			//cout << lambdaq[j] << " ";
		}
		//cout << endl;
		counter++;
		
		//Compute the entropy -- the primal objective
		double entropy = 0;
		for (int i=0; i<nq; i++)
		{
			for (int j=0; j<mq; j++)
			{
				double p = exp(muq[i]+lambdaq[j])/(1+exp(muq[i]+lambdaq[j]));
				entropy = entropy - (p*log(p) + (1-p)*log(1-p))*rowMult[i]*colMult[j];
			}
		}
		//cout << "Best factor: " << bestFactor << " - Current cost: " << bestError << " - Entropy: " << entropy << endl << endl;
		// The following line is to decrease the smallest possible stepsize if the optimal one was 0.
		if (bestFactor == 0) factors[0] = factors[0]/2;
	}
	
	mu = new double[n];
	lambda = new double[m];

	for (int i=0; i<nq; i++) 
	{
		for (int k=0; k<rowIndices->at(i)->size(); k++) mu[rowIndices->at(i)->at(k)]=muq[i];
	}
	for (int j=0; j<mq; j++) 
	{
		for (int k=0; k<colIndices->at(j)->size(); k++) lambda[colIndices->at(j)->at(k)]=lambdaq[j];
	}
//	delete rowSumsq;
//	delete colSumsq;
//	for (int i=0; i<rowIndices->size(); i++) delete rowIndices->at(i);
//	delete rowIndices;
//	for (int i=0; i<colIndices->size(); i++) delete colIndices->at(i);
//	delete colIndices;
	
	// This is to speed up the probability computation afterwards. Note however that the use of these vectors may be numerically inexact if there are very sparse rows as well as very dense columns, for example (but normally this should be fine).
	expMu = new double[n];
	//cout << "Mu: ";
	for (int i=0; i<n; i++) {expMu[i] = exp(mu[i]); //cout << expMu[i] << " ";
} //cout << endl;
	expLambda = new double[m];
	//cout << "Lambda: ";
	for (int j=0; j<m; j++) {expLambda[j] = exp(lambda[j]); //cout << expLambda[j] << " ";
} //cout << endl;
}


double NullModel::probability(int row, int col) const
{
	assert(row<n and col<m);
//	double e = exp(mu[row]+lambda[col]);
	double e = expMu[row]*expLambda[col]; // This is faster, but may be numerically inexact if there are very sparse rows as well as very dense columns, for example (but normally this should be fine).
	return e/(1+e);
}


double NullModel::logProbability(int row, int col) const
{
	assert(row<n and col<m);
//	double e = exp(mu[row]+lambda[col]);
	double e = expMu[row]*expLambda[col]; // This is faster, but may be numerically inexact if there are very sparse rows as well as very dense columns, for example (but normally this should be fine).
	return log(e)-log(1+e);
}


double NullModel::logProbability(set<int>* rows, set<int>* cols) const
{
	double out=0;
	set<int>::iterator rowStart;
	set<int>::iterator colStart;
	for (rowStart = rows->begin(); rowStart != rows->end(); rowStart++)
	{
		for (colStart = cols->begin(); colStart != cols->end(); colStart++)
		{
			out += logProbability(*rowStart,*colStart);
		}
	}
	return out;
}


double NullModel::logProbabilityTransaction(int row, set<int>* cols) const
{
	double out=0;
	set<int>::iterator colStart;
	for (colStart = cols->begin(); colStart != cols->end(); colStart++)
	{
		out += logProbability(row,*colStart);
	}
	return out;
}


double NullModel::minLogProbabilityTransaction(int row, set<int>* cols) const
{
	double out=0;
	set<int>::iterator colStart;
	for (colStart = cols->begin(); colStart != cols->end(); colStart++)
	{
		out = min(logProbability(row,*colStart),out);
	}
	return out;
}


void NullModel::makeUnique(vector<int>* sums, vector<int>* uniqueSums, vector<vector<int>*>* indices)
{
	uniqueSums->clear();
	for (int i=0; i<indices->size(); i++) indices->at(i)->clear();
	indices->clear();
	
	for (int i=0; i<sums->size(); i++)
	{
		int x=sums->at(i);
		int k;
		for (k=0 ; k<uniqueSums->size(); k++)
		{
			if (x==uniqueSums->at(k))
			{
				indices->at(k)->push_back(i);
				break;
			}
		}
		if (k==uniqueSums->size())
		{
			uniqueSums->push_back(x);
			indices->push_back(new vector<int>(1,i));
		}
	}
	//for (int i=0; i<uniqueSums->size(); i++) cout << uniqueSums->at(i) << " "; cout << endl << endl;
}



/////////////////////////////////////////////

//int main(int argc, char *argv[])
//{
//	int who = RUSAGE_SELF;
//	struct rusage usage;
//
//	if (argc < 3)
//	{
//		cout << "Specify at least 2 parameters: the datafile, and the number of random databases to generate";
//		return -1;
//	}
//	// Read in the data
//	ifstream* myfile = new ifstream(argv[1]);
//	Database* db;
//	if (myfile->is_open())
//	{
//		db = new Database(myfile);
//	}
//	else {cout << "problem" << endl; return -1;}
//
//		//Report time
//	getrusage(who, &usage);
//	cout << "Time to reading the database: " << endl;
//	cout << usage.ru_utime.tv_sec << " seconds and " << usage.ru_utime.tv_usec << " microseconds." << endl;
//
//	// Computing the marginals etc
//	int n=db->getNumberOfTransactions();
//	int m=db->getNumberOfItems();
//	cout << "Number of transactions:" << n << endl;
//	cout << "Number of items:" << m << endl;
//
//	vector<int>* rowSums = db->getTransactionSizes();
//	vector<int>* colSums = db->getItemSizes();
//
//		//Report time
//	getrusage(who, &usage);
//	cout << "Time to computing the marginals: " << endl;
//	cout << usage.ru_utime.tv_sec << " seconds and " << usage.ru_utime.tv_usec << " microseconds." << endl;
//
//	// Make the probabilistic model
//	NullModel* nm = new NullModel(rowSums,colSums);
//
//	// Start generating databases
//
//	int number = atoi(argv[2]);
//	srand(time(0));
//
//	//Report time
//	getrusage(who, &usage);
//	cout << "Time to creation of the probabilistic model: " << endl;
//	cout << usage.ru_utime.tv_sec << " seconds and " << usage.ru_utime.tv_usec << " microseconds." << endl;
//
//	for (int kk=0; kk<number; kk++)
//	{
//		Database* outDB = new Database();
//		for (int i=0; i<n; i++)
//		{
//			outDB->addTransaction(new set<int>());
//		}
//		char str[50];
//		sprintf(str,"%s_%d",argv[1],kk);
//		ofstream* stream = new ofstream(str);
//		for (int i=0; i<nm->nq; i++)
//		{
////			cout << i << endl;
//			for (int j=0; j<nm->mq; j++)
//			{
//				double numerator = exp(nm->muq[i]+nm->lambdaq[j]);
//				double prob = numerator/(1+numerator);
//				int nqi = nm->rowIndices->at(i)->size();
//				int mqj = nm->colIndices->at(j)->size();
//				int nmqij = nqi*mqj;
//				int start = -1;
//				while (start<nqi*mqj-1)
//				{
//					double r = (double)rand()/(double)RAND_MAX;
//					int increment = (int) ceil(min(log(1-r),-r)/min(log(1-prob),-prob));
//					start = start+increment;
//					if (start<nqi*mqj)
//					{
//						outDB->insertEntry(nm->rowIndices->at(i)->at((int) floor(start/mqj)),nm->colIndices->at(j)->at(start % mqj));
//					}
//				}
//			}
//		}
//
//		for (int i=0; i<n;i++)
//		{
//			set<int>* trans = outDB->getTransaction(i);
//			*stream << i << " " << i << " " << trans->size();
//			for (set<int>::iterator it=trans->begin(); it!=trans->end(); it++)
//			{
//				*stream << " " << *it;
//			}
//			*stream << endl;
//		}
//		delete stream;
//		delete outDB;
//	}
//
//	//Report time
//	getrusage(who, &usage);
//	cout << "Time to generating the random databases: " << endl;
//	cout << usage.ru_utime.tv_sec << " seconds and " << usage.ru_utime.tv_usec << " microseconds." << endl;
//}


