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

#include "NullModel3Ary.h"
//#include <sys/resource.h>
using namespace std;


NullModel3Ary::NullModel3Ary(vector<int>* rowSums, vector<int>* colSums, vector<int>* tubSums)
{
	n=rowSums->size();
	m=colSums->size();
	p=tubSums->size();
	
	rowSumsq = new vector<int>();
	rowIndices = new vector<vector<int>*>();
	makeUnique(rowSums, rowSumsq, rowIndices);
	
	colSumsq = new vector<int>();
	colIndices = new vector<vector<int>*>();
	makeUnique(colSums, colSumsq, colIndices);
	
	tubSumsq = new vector<int>();
	tubIndices = new vector<vector<int>*>();
	makeUnique(tubSums, tubSumsq, tubIndices);
	
	nq = rowSumsq->size();
	mq = colSumsq->size();
	pq = tubSumsq->size();

	muq = new double[nq]; for (int i=0; i<nq; i++) muq[i]=0;
	lambdaq = new double[mq]; for (int i=0; i<mq; i++) lambdaq[i]=0;
	kappaq = new double[pq]; for (int i=0; i<pq; i++) kappaq[i]=0;
	
	int* rowMult;
	rowMult = new int[nq];
	for (int i=0; i<nq; i++) rowMult[i]=rowIndices->at(i)->size();
	int* colMult;
	colMult = new int[mq];
	for (int j=0; j<mq; j++) colMult[j]=colIndices->at(j)->size();
	int* tubMult;
	tubMult = new int[pq];
	for (int k=0; k<pq; k++) tubMult[k]=tubIndices->at(k)->size();
	
	// Start of the iteration
	int counter = 0;
	double infeasibility=1;
	double factors[10] = {0.01, 0.021544, 0.046416, 0.1, 0.21544, 0.46416, 1, 2.1544, 4.6416, 10};
	while ((counter < 100) & (infeasibility>n*m*1e-12))
	{
		//cout << "Iteration: " << counter+1 << endl;
		// Compute gradient and Hessian diagonal
		double* gMuq;
		gMuq = new double[nq];
		double* gLambdaq;
		gLambdaq = new double[mq];
		double* gKappaq;
		gKappaq = new double[pq];
		double* hMuq;
		hMuq = new double[nq];
		double* hLambdaq;
		hLambdaq = new double[mq];
		double* hKappaq;
		hKappaq = new double[pq];

		// Note: each gMuq[i] should actually be multiplied by rowMult[i] to really be the gradient.
        // The way it is computed now, it is the mistake made for the row sum i.
        // Accordingly, each hMuq[i] should be multiplied by rowMult[i].
        // This is not done as it would be divided away anyway when the step direction is computed.
        // A similar thing holds for the lambda's and kappa's.
		for (int i=0; i<nq; i++) {gMuq[i]=-rowSumsq->at(i); hMuq[i]=0;}
		for (int j=0; j<mq; j++) {gLambdaq[j]=-colSumsq->at(j); hLambdaq[j]=0;}
		for (int k=0; k<pq; k++) {gKappaq[k]=-tubSumsq->at(k); hKappaq[k]=0;}
		for (int i=0; i<nq; i++)
		{
			for (int j=0; j<mq; j++)
			{
				for (int k=0; k<pq; k++)
				{
					double v=exp(muq[i]+lambdaq[j]+kappaq[k]);
					double g = v/(1+v);
					double h = g/(1+v);
					gMuq[i] += g*colMult[j]*tubMult[k];
					gLambdaq[j] += g*rowMult[i]*tubMult[k];
					gKappaq[k] += g*rowMult[i]*colMult[j];
					hMuq[i] += h*colMult[j]*tubMult[k];
					hLambdaq[j] += h*rowMult[i]*tubMult[k];
					hKappaq[k] += h*rowMult[i]*colMult[j];
				}
			}
		}
		infeasibility = 0;
		for (int i=0; i<nq; i++) infeasibility += gMuq[i]*gMuq[i]*rowMult[i];
		for (int j=0; j<mq; j++) infeasibility += gLambdaq[j]*gLambdaq[j]*colMult[j];
		for (int k=0; k<pq; k++) infeasibility += gKappaq[k]*gKappaq[k]*tubMult[k];
		//cout << "Infeasibility (gradient norm squared): " << infeasibility << endl;
		
		// Compute step direction
		double* dMuq;
		dMuq = new double[nq];
		double* dLambdaq;
		dLambdaq = new double[mq];
		double* dKappaq;
		dKappaq = new double[pq];
		for (int i=0; i<nq; i++) dMuq[i]=-gMuq[i]/hMuq[i];
		for (int j=0; j<mq; j++) dLambdaq[j]=-gLambdaq[j]/hLambdaq[j];
		for (int k=0; k<pq; k++) dKappaq[k]=-gKappaq[k]/hKappaq[k];
		
		// Find optimal step size
		double bestFactor;
		bestFactor = 0;
		
		double bestError=0;
		for (int i=0; i<nq; i++) bestError -= rowMult[i]*muq[i]*rowSumsq->at(i);
		for (int j=0; j<mq; j++) bestError -= colMult[j]*lambdaq[j]*colSumsq->at(j);
		for (int k=0; k<pq; k++) bestError -= tubMult[k]*kappaq[k]*tubSumsq->at(k);
		for (int i=0; i<nq; i++)
		{
			for (int j=0; j<mq; j++)
			{
				for (int k=0; k<pq; k++)
				{
					bestError += log(1+exp(muq[i]+lambdaq[j]+kappaq[k]))*rowMult[i]*colMult[j]*tubMult[k];
				}
			}
		}
//		cout << "bestError: " << bestError << "\n";

		for (int ct=0; ct<10; ct++)
		{
//			cout << "ct: " << ct << "\n";
			double factor = factors[ct];
			double* muqTry;
			muqTry = new double[nq];
			double* lambdaqTry;
			lambdaqTry = new double[mq];
			double* kappaqTry;
			kappaqTry = new double[pq];
			for (int i=0; i<nq; i++) muqTry[i]=muq[i]+dMuq[i]*factor;
			for (int j=0; j<mq; j++) lambdaqTry[j]=lambdaq[j]+dLambdaq[j]*factor;
			for (int k=0; k<pq; k++) kappaqTry[k]=kappaq[k]+dKappaq[k]*factor;
			
			double errorTry;
			errorTry = 0;
			for (int i=0; i<nq; i++) errorTry -= rowMult[i]*muqTry[i]*rowSumsq->at(i);
			for (int j=0; j<mq; j++) errorTry -= colMult[j]*lambdaqTry[j]*colSumsq->at(j);
			for (int k=0; k<pq; k++) errorTry -= tubMult[k]*kappaqTry[k]*tubSumsq->at(k);
			for (int i=0; i<nq; i++)
			{
				for (int j=0; j<mq; j++)
				{
					for (int k=0; k<pq; k++)
					{
						errorTry += log(1+exp(muqTry[i]+lambdaqTry[j]+kappaqTry[k]))*rowMult[i]*colMult[j]*tubMult[k];
					}
				}
			}
			
			if (errorTry<bestError)
			{
				bestError=errorTry;
				bestFactor = factor;
			}
			else
			{
				break;
			}
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
		//cout << "Kappa: ";
		for (int k=0; k<pq; k++) 
		{
			kappaq[k] += dKappaq[k]*bestFactor; 
			//cout << kappaq[k] << " ";
		}
		//cout << endl;
		counter++;
		
		//Compute the entropy -- the primal objective
		double entropy = 0;
		for (int i=0; i<nq; i++)
		{
			for (int j=0; j<mq; j++)
			{
				for (int k=0; k<pq; k++)
				{
					double p = exp(muq[i]+lambdaq[j]+kappaq[k])/(1+exp(muq[i]+lambdaq[j]+kappaq[k]));
					entropy = entropy - (p*log(p) + (1-p)*log(1-p))*rowMult[i]*colMult[j]*tubMult[k];
				}
			}
		}
		//cout << "Best factor: " << bestFactor << " - Current cost: " << bestError << " - Entropy: " << entropy << endl << endl;
		// The following line is to decrease the smallest possible stepsize if the optimal one was 0.
		if (bestFactor == 0) factors[0] = factors[0]/2;
	}
	
	mu = new double[n];
	lambda = new double[m];
	kappa = new double[p];

	for (int i=0; i<nq; i++) 
	{
		for (int ct=0; ct<rowIndices->at(i)->size(); ct++) mu[rowIndices->at(i)->at(ct)]=muq[i];
	}
	for (int j=0; j<mq; j++) 
	{
		for (int ct=0; ct<colIndices->at(j)->size(); ct++) lambda[colIndices->at(j)->at(ct)]=lambdaq[j];
	}
	for (int k=0; k<pq; k++) 
	{
		for (int ct=0; ct<tubIndices->at(k)->size(); ct++) kappa[tubIndices->at(k)->at(ct)]=kappaq[k];
	}
//	delete rowSumsq;
//	delete colSumsq;
//	for (int i=0; i<rowIndices->size(); i++) delete rowIndices->at(i);
//	delete rowIndices;
//	for (int i=0; i<colIndices->size(); i++) delete colIndices->at(i);
//	delete colIndices;
	
	// This is to speed up the probability computation afterwards. Note however that the use of these vectors may be numerically inexact if there are very sparse rows as well as very dense columns, for example (but normally this should be fine).
	expMu = new double[n];
	//cout << "expMu: ";
	for (int i=0; i<n; i++)
	{
		expMu[i] = exp(mu[i]);
		//cout << expMu[i] << " ";
	}
	//cout << endl;
	expLambda = new double[m];
	//cout << "expLambda: ";
	for (int j=0; j<m; j++)
	{
		expLambda[j] = exp(lambda[j]);
		//cout << expLambda[j] << " ";
	}
	//cout << endl;
	expKappa = new double[p];
	//cout << "expKappa: ";
	for (int k=0; k<p; k++)
	{
		expKappa[k] = exp(kappa[k]);
		//cout << expKappa[k] << " ";
	}
	//cout << endl;
}


double NullModel3Ary::probability(int row, int col, int tub) const
{
	assert((row<n) & (col<m) & (tub<p));
//	cerr << row << " " << col << " " << tub << "\n";
//	double e = exp(mu[row]+lambda[col]);
	double e = expMu[row]*expLambda[col]*expKappa[tub]; // This is faster, but may be numerically inexact if there are very sparse rows as well as very dense columns, for example (but normally this should be fine).
	return e/(1+e);
}


double NullModel3Ary::logProbability(int row, int col, int tub) const
{
	assert((row<n) & (col<m) & (tub<p));
//	double e = exp(mu[row]+lambda[col]);
	double e = expMu[row]*expLambda[col]*expKappa[tub]; // This is faster, but may be numerically inexact if there are very sparse rows as well as very dense columns, for example (but normally this should be fine).
	return log(e)-log(1+e);
}


double NullModel3Ary::logProbability(set<int>* rows, set<int>* cols, set<int>* tubs) const
{
	double out=0;
	set<int>::iterator rowStart;
	set<int>::iterator colStart;
	set<int>::iterator tubStart;
	for (rowStart = rows->begin(); rowStart != rows->end(); rowStart++)
	{
		for (colStart = cols->begin(); colStart != cols->end(); colStart++)
		{
			for (tubStart = tubs->begin(); tubStart != tubs->end(); tubStart++)
			{
				out += logProbability(*rowStart,*colStart,*tubStart);
			}
		}
	}
	return out;
}

double NullModel3Ary::logProbabilityRowsCols(int row, int col) const
{
	double out=0;
	double outPart=1;
	for (int k = 0; k<p; k++)
	{
		outPart *= (1-probability(row,col,k));
	}
	outPart = 1-outPart;
	out += log(outPart);

	return out;
}

double NullModel3Ary::logProbabilityRowsTubs(int row, int tub) const
{
	double out=0;
	double outPart=1;
	for (int j = 0; j<m; j++)
	{
		outPart *= (1-probability(row,j,tub));
	}
	outPart = 1-outPart;
	out += log(outPart);

	return out;
}

double NullModel3Ary::logProbabilityColsTubs(int col, int tub) const
{
	double out=0;

	double outPart=1;
	for (int i = 0; i<n; i++)
	{
		outPart *= (1-probability(i,col,tub));
	}
	outPart = 1-outPart;
	out += log(outPart);

	return out;
}


double NullModel3Ary::logProbabilityRowsCols(set<int>* rows, set<int>* cols) const
{
	double out=0;
	set<int>::iterator rowStart;
	set<int>::iterator colStart;
	for (rowStart = rows->begin(); rowStart != rows->end(); rowStart++)
	{
		for (colStart = cols->begin(); colStart != cols->end(); colStart++)
		{
			double outPart=1;
			for (int k = 0; k<p; k++)
			{
				outPart *= (1-probability(*rowStart,*colStart,k));
			}
			outPart = 1-outPart;
			out += log(outPart);
		}
	}
	return out;
}

double NullModel3Ary::logProbabilityRowsTubs(set<int>* rows, set<int>* tubs) const
{
	double out=0;
	set<int>::iterator rowStart;
	set<int>::iterator tubStart;
	for (rowStart = rows->begin(); rowStart != rows->end(); rowStart++)
	{
		for (tubStart = tubs->begin(); tubStart != tubs->end(); tubStart++)
		{
			double outPart=1;
			for (int j = 0; j<m; j++)
			{
				outPart *= (1-probability(*rowStart,j,*tubStart));
			}
			outPart = 1-outPart;
			out += log(outPart);
		}
	}
	return out;
}

double NullModel3Ary::logProbabilityColsTubs(set<int>* cols, set<int>* tubs) const
{
	double out=0;
	set<int>::iterator colStart;
	set<int>::iterator tubStart;
	for (colStart = cols->begin(); colStart != cols->end(); colStart++)
	{
		for (tubStart = tubs->begin(); tubStart != tubs->end(); tubStart++)
		{
			double outPart=1;
			for (int i = 0; i<n; i++)
			{
				outPart *= (1-probability(i,*colStart,*tubStart));
			}
			outPart = 1-outPart;
			out += log(outPart);
		}
	}
	return out;
}



void NullModel3Ary::makeUnique(vector<int>* sums, vector<int>* uniqueSums, vector<vector<int>*>* indices)
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
//	int n=5;
//	int m=10;
//	int p=30;
//	vector<int>* rowSums = new vector<int>(n);
//	vector<int>* colSums = new vector<int>(m);
//	vector<int>* tubSums = new vector<int>(p);
//
//	rowSums->at(0) = 53;
//	rowSums->at(1) = 63;
//	rowSums->at(2) = 52;
//	rowSums->at(3) = 57;
//	rowSums->at(4) = 57;
//
//	colSums->at(0) = 36;
//	colSums->at(1) = 32;
//	colSums->at(2) = 26;
//	colSums->at(3) = 26;
//	colSums->at(4) = 27;
//	colSums->at(5) = 27;
//	colSums->at(6) = 27;
//	colSums->at(7) = 27;
//	colSums->at(8) = 24;
//	colSums->at(9) = 30;
//
//	tubSums->at(0) = 16;
//	tubSums->at(1) = 15;
//	tubSums->at(2) = 17;
//	tubSums->at(3) = 8;
//	tubSums->at(4) = 11;
//	tubSums->at(5) = 11;
//	tubSums->at(6) = 10;
//	tubSums->at(7) = 8;
//	tubSums->at(8) = 5;
//	tubSums->at(9) = 8;
//	tubSums->at(10) = 10;
//	tubSums->at(11) = 10;
//	tubSums->at(12) = 9;
//	tubSums->at(13) = 6;
//	tubSums->at(14) = 9;
//	tubSums->at(15) = 9;
//	tubSums->at(16) = 12;
//	tubSums->at(17) = 7;
//	tubSums->at(18) = 13;
//	tubSums->at(19) = 8;
//	tubSums->at(20) = 10;
//	tubSums->at(21) = 12;
//	tubSums->at(22) = 14;
//	tubSums->at(23) = 9;
//	tubSums->at(24) = 7;
//	tubSums->at(25) = 6;
//	tubSums->at(26) = 5;
//	tubSums->at(27) = 10;
//	tubSums->at(28) = 3;
//	tubSums->at(29) = 4;
//
//	// Make the probabilistic model
//	NullModel3Ary* nm = new NullModel3Ary(rowSums,colSums,tubSums);
//	for (int k=0; k<p; k++)
//	{
//		for (int i=0; i<n; i++)
//		{
//			for (int j=0; j<m; j++)
//			{
//				cout << nm->probability(i,j,k) << " ";
//			}
//			cout << "\n";
//		}
//		cout << "\n";
//	}
//
//	set<int>* rowSet = new set<int>();
//	rowSet->insert(0);
//	rowSet->insert(1);
//	set<int>* colSet = new set<int>();
//	colSet->insert(0);
//	colSet->insert(1);
//	set<int>* tubSet = new set<int>();
//	tubSet->insert(0);
//	tubSet->insert(1);
//	cout << "Rowset and colset: " << nm->logProbabilityRowsCols(rowSet,colSet) << "\n";
//	cout << "Rowset and tubset: " << nm->logProbabilityRowsTubs(rowSet,tubSet) << "\n";
//	cout << "Colset and tubset: " << nm->logProbabilityColsTubs(colSet,tubSet) << "\n";
//}
