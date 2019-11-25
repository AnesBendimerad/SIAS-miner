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

/** RMiner.h
 *
 *  Author: Eirini Spyropoulou
 *  email: eirini.spyropoulou@gmail.com
 **/

#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <sstream>
#include <cstring>
#include <ctime>
#include "NullModel3Ary.h"
#include <sys/resource.h>        //used to get usage of space (only for linux!!!)
#include "DatabaseGenerator.h"

using namespace std;

typedef struct {
	unsigned int node;
	unsigned int type;
    unsigned int model_index;
	unsigned int connectivity;
}cnode;

typedef struct {
	unsigned int patternIndex;
	double interestingness;
	double desc_length;
}inode;

typedef struct {
	unsigned int type;
	double average_degree;
	bool priority;
}ntype;

typedef struct {
  bool operator()(ntype i,ntype j) {
	  if (i.priority && !j.priority)
		  return true;
	  else if (!i.priority && j.priority)
		  return false;
	  else
		  return (i.average_degree<j.average_degree);
  }
}p_sort;

typedef struct {
  bool operator()(inode i, inode j) {
	  return (i.interestingness>j.interestingness);
  }
}i_sort;

class RMiner
{
public:
	RMiner(vector<vector<unsigned int> >* en, vector<vector<unsigned int> >* rt, vector<vector<vector<unsigned int> > >* el, vector<vector<set<unsigned int> > >* nl, map<unsigned int, unsigned int>* nt, map<unsigned int, unsigned int>* ni, map<unsigned int, string>* nn, map<unsigned int, NullModel3Ary*> m3, map<unsigned int, NullModel*> m2, vector<unsigned int>* cons, double density, unsigned int numOfEntities, vector<ntype>* order, bool ci, map<unsigned int, set<unsigned int> >* Descendants, map<unsigned int, set<unsigned int> >* Anscestors, map<unsigned int, set<unsigned int> >* IE, map<unsigned int, char>* toT);
	void run(vector<set<unsigned int> >* C, set<unsigned int>* B, vector<set<unsigned int> >* Comp, vector<bool>* activeRelTypes, vector<bool>* activeNodeTypes, unsigned int level, bool consat);
	void iteratively_print(int k, fstream& outfile);
	int getNumOfNMCCSs();
	int getSizeOfMaxNMCCS();
	int getMaxDepth();
	int getMaxSpace();
    int getNumOfClosed();
    int getNumOfBranches();
    bool is_comp_with_one_rel(unsigned int rel_type, vector<unsigned int>* s_rel_insts, vector<set<unsigned int> >* s, set<unsigned int> nodeTypes);
    bool is_comp(int node, vector<set<unsigned int> >* s);
    void setExcludedRelIdsFromMeasure(set<int> & excludedRelIdsFromMeasureInt){ excludedRelIdsFromMeasure=excludedRelIdsFromMeasureInt;}
private:
	bool eval_constraints(vector<set<unsigned int> >* Comp, vector<set<unsigned int> >* C, set<unsigned int>* B);
	bool eval_early_constraints(unsigned int node);
    bool is_proper(unsigned int entity, unsigned int type_id, vector<set<unsigned int> >* C);
    double prob_binary_rel(set<unsigned int> C, unsigned int entity, set<unsigned int> entities, unsigned int rel_type, unsigned int i, unsigned int po_type_index, bool printProbs);
    double prob_triary_rel(set<unsigned int> C, unsigned int entity1, unsigned int entity2, set<unsigned int> entities, unsigned int rel_type, unsigned int i, unsigned int po_type_index, bool print);
    double probability_binary_set(set<unsigned int> a, set<unsigned int> b, unsigned int rel_type, bool print);
    double probability_triary_set(set<unsigned int> a, set<unsigned int> b, set<unsigned int> c, unsigned int rel_type, bool print);
    double probability_binary_entity(unsigned int a, set<unsigned int> b, unsigned int rel_type, unsigned int i, bool print);
    double probability_triary_entity(unsigned int a, set<unsigned int> b, set<unsigned int> c, unsigned int rel_type, unsigned int i, bool print);
	double self_info_binary_rel(vector<set<unsigned int> >* C, unsigned int rel_type);
	double self_info_3ary_rel(vector<set<unsigned int> >* C, unsigned int rel_type);
	double self_info(vector<set<unsigned int> >* C, bool print);
	double self_info_binary_rel(vector<set<unsigned int> > C, unsigned int rel_type, vector<set<unsigned int> >* Conveyed);
	double self_info_3ary_rel(vector<set<unsigned int> > C, unsigned int rel_type, vector<set<unsigned int> >* Conveyed);
	double self_info(vector<set<unsigned int> > C, vector<set<unsigned int> >* Conveyed);
	double desc_length(vector<set<unsigned int> >* C);
	bool is_NMCCS(vector<set<unsigned int> >* C, vector<set<unsigned int> >* Comp, vector<bool>* activeNodeTypes, unsigned int level);
    int find_num_of_entities(int type, vector<set<unsigned int> >* s);	
	int intersection_size(set<unsigned int>* s1, set<unsigned int>* s2);
	bool new_type_introduced(vector<bool>* Rels, vector<vector<unsigned int> > rel_insts);
	bool connected_to_all_rel_types(unsigned int nodeType, unsigned int node);
	bool active_related_types(vector<bool>* activeNodeTypes, unsigned int type);
    bool is_implied(unsigned int entity, unsigned int type, set<unsigned int> entity_set);
    bool is_empty(vector<set<unsigned int> >* C);
    vector<bool> find_closure(vector<ntype> new_orderOfTypes, vector<bool> activeEntityTypes, vector<set<unsigned int> >* new_Comp, vector<set<unsigned int> >& closed_newC, set<unsigned int>* newB, vector<bool>& new_activeRelTypes, bool& overlap);
    void find_Comp(unsigned int element, vector<ntype> new_orderOfTypes, vector<bool>* activeEntityTypes, vector<bool> new_activeEntityTypes, vector<set<unsigned int> >& new_Comp, vector<set<unsigned int> >* Comp, vector<set<unsigned int> >* newC);

	vector<vector<unsigned int> >* RelInstIdToEntities;
	vector<vector<unsigned int> >* RelsToTypes;
	vector<vector<vector<unsigned int> > >* RelInstList;
	vector<vector<set<unsigned int> > >* EntityAugList;
	map<unsigned int, unsigned int>* EntityToType;
	//vector<vector<unsigned int> >* EntityTypeToRelTypes;
	map<unsigned int, unsigned int>* EntityToModelIndex;
	map<unsigned int, string>*  EntityIdToEntityName;

	map<unsigned int, NullModel3Ary*> Models3;
	map<unsigned int, NullModel*> Models2;
    
    map<unsigned int, set<unsigned int> >* Descendants;
    map<unsigned int, set<unsigned int> >* Anscestors;
    map<unsigned int, set<unsigned int> >* implied_entities;
    
    map<unsigned int, char>* typeOfType;

	vector<unsigned int>* constraints;
    
    bool compute_interestingness;

	vector<ntype>* orderOfTypes;

	vector< vector<set<unsigned int> > > NMCCSsList;                     //vector storing all N-MCCSs
	vector<inode> InfoList;												//vector keeping the interestingness and description length of NMCCSs


	unsigned int numOfEntities;
	double p;

	unsigned int maxDepth;
	unsigned int sizeOfMaxNMCCS;
	unsigned int numOfNMCCSs;
	unsigned int maxSpace;
    unsigned int numOfClosed;
    unsigned int numOfPossibleBranches;
    set<int> excludedRelIdsFromMeasure;
};
