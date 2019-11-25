/*==========================================================================================================
 * P-N-RMiner version 1.0 - Software to mine interesting maximal Complete Connected Proper Subsets (CCSPSs)
 * from multi-relational data containing any partial order.
 *
 * Copyright (C) 2015 Eirini Spyropoulou, Tijl De Bie, Jefrey Lijffijt
 * Department of Engineering Mathematics, University of Bristol, Bristol, UK
 * ------------------------------------------------------------------------------------------------
 *
 * P-N-RMiner-1.0 licensed under the Non-Profit Open Software License 3.0 (NPOSL-3.0)
 *================================================================================================*/

/** DatabaseGenerator.cpp
 *
 *  Author: Eirini Spyropoulou
 *  email: eirini.spyropoulou@gmail.com
 **/

#include "RMiner.h"

using namespace std;

RMiner::RMiner(vector<vector<unsigned int> >* en, vector<vector<unsigned int> >* rt, vector<vector<vector<unsigned int> > >* el, vector<vector<set<unsigned int> > >* nl, map<unsigned int, unsigned int>* nt, map<unsigned int, unsigned int>* ni, map<unsigned int, string>* nn, map<unsigned int, NullModel3Ary*> m3, map<unsigned int, NullModel*> m2, vector<unsigned int>* cons, double density, unsigned int noe, vector<ntype>* ot, bool ci, map<unsigned int, set<unsigned int> >* D, map<unsigned int, set<unsigned int> >* A, map<unsigned int, set<unsigned int> >* NI, map<unsigned int,char>* toT){

	RelInstIdToEntities=en;                          //linking relationship instance ids to entities ids
	RelsToTypes=rt;                                  //linking relationship types to the participating entity types
	RelInstList=el;                                 //relationship instance ids for every entity and relationship type
	EntityAugList=nl;                               //for every entity, it stores the valid augmentation elements (or the entities it is related to) in a different set for every entity type
	EntityToType=nt;                                //linking every entity to its type
	EntityToModelIndex=ni;                          //linking entity ids to their index in the MaxEnt model of the data
	//EntityTypeToRelTypes=tr;                        //linking every entity type to the relationship types it participates
	EntityIdToEntityName=nn;                       //linking entity ids to the actual string they correspond to ex: film title, director name etc

	Models3=m3;                                  //MaxEnt model for 3-ary relationships
	Models2=m2;                                  //MaxEnt model for binary relationships
    
    Descendants = D;
    Anscestors = A;
    implied_entities = NI;

	constraints = cons;
	numOfEntities = noe;
	p=density;                                  //the parameter p in the description length equals the density of the data

	orderOfTypes=ot;
    
    compute_interestingness=ci;
    
    typeOfType=toT;

	maxDepth=0;
	sizeOfMaxNMCCS=0;
	maxSpace=0;
	numOfNMCCSs=0;
    numOfClosed=0;
    numOfPossibleBranches=0;
}

int RMiner::getNumOfNMCCSs(){
	return numOfNMCCSs;
}
int RMiner::getSizeOfMaxNMCCS(){
	return sizeOfMaxNMCCS;
}
int RMiner::getMaxDepth(){
	return maxDepth;
}

int RMiner::getMaxSpace(){
	return maxSpace;
}

int RMiner::getNumOfClosed(){
    return numOfClosed;
}

int RMiner::getNumOfBranches(){
    return numOfPossibleBranches;
}

unsigned int size_of_vecset(vector<set<unsigned int> >* vecset){
    vector<set<unsigned int> >::const_iterator sit;
    unsigned int size = 0;
    for (sit=vecset->begin(); sit!=vecset->end(); sit++){
        size+=(*sit).size();
    }
    return size;
}

//
bool RMiner::new_type_introduced(vector<bool>* Rels, vector<vector<unsigned int> > rel_insts){

	for (unsigned int i=0; i<Rels->size(); i++){
		if (!((*Rels)[i]) && rel_insts[i].size()!=0){
			return true;
		}
	}

	return false;
}

//checks whether the constraints are satisfied on the set Comp\(B U C) for every etnity type (constraint upper bound)
bool RMiner::eval_constraints(vector<set<unsigned int> >* Comp, vector<set<unsigned int> >* C, set<unsigned int>* B){

	set<unsigned int>::const_iterator cit;
	unsigned int card=0;

	for (unsigned int i=0; i<Comp->size(); i++){
		card=0;
		for (cit=(*Comp)[i].begin(); cit!=(*Comp)[i].end(); ++cit){
			if((*C)[i].find(*cit)==(*C)[i].end()){
				if (B->find(*cit)==B->end())
					card++;
			}
			else {
				card++;
			}
			if (card >= (*constraints)[i])
				break;
		}
		if (card < (*constraints)[i])
			return false;
	}
	return true;
}

bool RMiner::is_empty(vector<set<unsigned int> >* C){
    for (unsigned int i=0; i<C->size(); i++){
        if ((*C)[i].size()!=0)
            return false;
    }
    return true;
}

//check whether an element is proper
bool RMiner::is_proper(unsigned int entity, unsigned int type_id, vector<set<unsigned int> >* C){
    queue<unsigned int> Q;
    set<unsigned int>::const_iterator sit;
    for (sit= (*Anscestors)[entity].begin(); sit!=(*Anscestors)[entity].end(); sit++){
        if ((*C)[type_id].find(*sit)==(*C)[type_id].end()){
            return false;
        }
    }
    
    return true;
}

bool RMiner::is_implied(unsigned int entity, unsigned int type, set<unsigned int> entity_set){
    set<unsigned int>::const_iterator it;
   
    
    if((*typeOfType)[type]=='o' || (*typeOfType)[type]=='p'){
        for (it=(*Descendants)[entity].begin(); it!=(*Descendants)[entity].end(); it++) {
            if (entity_set.find(*it)!=entity_set.end()){
                return true;
            }
        }
    }
    
    return false;
}

double RMiner::probability_binary_set(set<unsigned int> a, set<unsigned int> b, unsigned int rel_type, bool printProbs){
    double logsum = 0;
    set<unsigned int>::const_iterator it1, it2;
    for (it1=a.begin(); it1!=a.end(); it1++){
        for (it2=b.begin(); it2!=b.end(); it2++){
            logsum+=log(1-Models2[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2]));
            if (printProbs)
            cout<<(*EntityIdToEntityName)[*it1]<<" "<<(*EntityIdToEntityName)[*it2]<<" "<<(Models2[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2]))<<endl;
        }
    }
    
    return exp(logsum);
}

double RMiner::probability_binary_entity(unsigned a, set<unsigned int> b, unsigned int rel_type, unsigned int otype_index, bool printProbs){
    double logsum = 0;
    set<unsigned int>::const_iterator it1;
    for (it1=b.begin(); it1!=b.end(); it1++){
        if (otype_index==1)
            logsum+=log(1-Models2[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[a]));
        else
            logsum+=log(1-Models2[rel_type]->probability((*EntityToModelIndex)[a],(*EntityToModelIndex)[*it1]));
        if (printProbs)
        cout<<(*EntityIdToEntityName)[*it1]<<" "<<(*EntityIdToEntityName)[a]<<" "<<(Models2[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[a]))<<endl;
        
    }
    
    return exp(logsum);
    
}

double RMiner::probability_triary_set(set<unsigned int> a, set<unsigned int> b, set<unsigned int> c, unsigned int rel_type, bool printProbs){
    double logsum = 0;
    set<unsigned int>::const_iterator it1, it2, it3;
    for (it1=a.begin(); it1!=a.end(); it1++){
        for (it2=b.begin(); it2!=b.end(); it2++){
            for (it3=c.begin(); it3!=c.end(); it3++){
                logsum+=log(1-Models3[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*it3]));
                if (printProbs)
                cout<<(*EntityIdToEntityName)[*it1]<<" "<<(*EntityIdToEntityName)[*it2]<<" "<<(*EntityIdToEntityName)[*it3]<<(Models3[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*it3]))<<endl;
            }
        }
    }
    
    return exp(logsum);
}

/*double RMiner::probability_triary_entity(unsigned int a, unsigned int b, set<unsigned int> c, unsigned int rel_type,unsigned int otype_index, bool printProbs){
    double logsum = 0;
    set<unsigned int>::const_iterator it2, it3;
    
    for (it2=b.begin(); it2!=b.end(); it2++){
        for (it3=c.begin(); it3!=c.end(); it3++){
            if (otype_index==0)
                logsum+=log(1-Models3[rel_type]->probability((*EntityToModelIndex)[a],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*it3]));
            else if (otype_index==1)
                logsum+=log(1-Models3[rel_type]->probability((*EntityToModelIndex)[*it2],(*EntityToModelIndex)[a],(*EntityToModelIndex)[*it3]));
            else
                logsum+=log(1-Models3[rel_type]->probability((*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*it3],(*EntityToModelIndex)[a]));
            
            if (printProbs)
            cout<<(*EntityIdToEntityName)[a]<<" "<<(*EntityIdToEntityName)[*it2]<<" "<<(*EntityIdToEntityName)[*it3]<<(Models3[rel_type]->probability((*EntityToModelIndex)[a],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*it3]))<<endl;
        }
    
    }
    
    return exp(logsum);
}*/


//computes probability of a rel type when one of the entity types is a partial order
double RMiner::prob_binary_rel(set<unsigned int> C, unsigned int entity, set<unsigned int> entities, unsigned int rel_type, unsigned int i, unsigned int po_type_index, bool printProbs){
    set<unsigned int>::const_iterator sit, ait, eit, iit;
    double prob=0;
    ait=C.begin();
    advance(ait,i);
    unsigned int j=i;
    
    for (sit=ait; sit!=C.end(); sit++){
        set<unsigned int> new_entities = entities;
        new_entities.insert(*sit);
        double p=1;
        
        set<unsigned int> probset;
        for (eit=new_entities.begin(); eit!=new_entities.end(); eit++){
            if ((*EntityToModelIndex).find(*eit)!=(*EntityToModelIndex).end()){
                probset.insert(*eit);
            }
            for (iit=(*implied_entities)[*eit].begin(); iit!=(*implied_entities)[*eit].end(); iit++){
                probset.insert(*iit);
            }
        }
        
        for (eit=probset.begin(); eit!=probset.end(); eit++){
            if (po_type_index==0){
                p*=(1-Models2[rel_type]->probability((*EntityToModelIndex)[*eit],(*EntityToModelIndex)[entity]));
                if (printProbs)
                    cout<<(*EntityIdToEntityName)[*eit]<<" "<<(*EntityIdToEntityName)[entity]<<" "<<Models2[rel_type]->probability((*EntityToModelIndex)[*eit],(*EntityToModelIndex)[entity])<<endl;
            }
            else {
                p*=(1-Models2[rel_type]->probability((*EntityToModelIndex)[entity],(*EntityToModelIndex)[*eit]));
                if (printProbs)
                    cout<<(*EntityIdToEntityName)[*eit]<<" "<<(*EntityIdToEntityName)[entity]<<" "<<Models2[rel_type]->probability((*EntityToModelIndex)[entity],(*EntityToModelIndex)[*eit])<<endl;
            }
            
        }
        probset.clear();
        
        if (new_entities.size()%2==0){
            prob-=p;
        }
        else {
            prob+=p;
        }
       
        j=j+1;
        prob+=prob_binary_rel(C, entity, new_entities, rel_type, j, po_type_index, printProbs);
    }
    
    return prob;
}

//computes probability of a rel type when one of the entity types is a partial order
double RMiner::prob_triary_rel(set<unsigned int> C, unsigned int entity1, unsigned int entity2, set<unsigned int> entities, unsigned int rel_type, unsigned int i, unsigned int po_type_index, bool printProbs){
    set<unsigned int>::const_iterator sit, ait, eit, iit;
    double prob=0;
    ait=C.begin();
    advance(ait,i);
    unsigned int j=i;
    
    for (sit=ait; sit!=C.end(); sit++){
        set<unsigned int> new_entities = entities;
        new_entities.insert(*sit);
        double p=1;
        
        set<unsigned int> probset;
        for (eit=new_entities.begin(); eit!=new_entities.end(); eit++){
            if ((*EntityToModelIndex).find(*eit)!=(*EntityToModelIndex).end()){
                probset.insert(*eit);
            }
            for (iit=(*implied_entities)[*eit].begin(); iit!=(*implied_entities)[*eit].end(); iit++){
                probset.insert(*iit);
            }
        }
        
        for (eit=probset.begin(); eit!=probset.end(); eit++){
            if (po_type_index==0)
                p*=(1-Models3[rel_type]->probability((*EntityToModelIndex)[*eit],(*EntityToModelIndex)[entity1],(*EntityToModelIndex)[entity2]));
            else if (po_type_index==1)
                p*=(1-Models3[rel_type]->probability((*EntityToModelIndex)[entity1],(*EntityToModelIndex)[*eit],(*EntityToModelIndex)[entity2]));
            else
                p*=(1-Models3[rel_type]->probability((*EntityToModelIndex)[entity1],(*EntityToModelIndex)[entity2],(*EntityToModelIndex)[*eit]));

        }
        
        if (new_entities.size()%2==0)
            prob-=p;
        else
            prob+=p;
        
        j=j+1;
        prob+=prob_triary_rel(C, entity1, entity2, new_entities, rel_type, j, po_type_index, printProbs);
    }
    
    
    return prob;
}

//computes part of the self information of the NMCCS C based on the binary relationship rel_type
double RMiner::self_info_binary_rel(vector<set<unsigned int> >* C, unsigned int rel_type){
	double si=0.0;
	set<unsigned int>::const_iterator sit1, sit2;
	set<unsigned int>::const_iterator it1,it2;
    
	vector<unsigned int> entity_types = (*RelsToTypes)[rel_type];

	for (sit1=(*C)[entity_types[0]].begin(); sit1!=(*C)[entity_types[0]].end(); ++sit1){
        for (sit2=(*C)[entity_types[1]].begin(); sit2!=(*C)[entity_types[1]].end(); ++sit2){
            si-=Models2[rel_type]->logProbability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2]);
        }
	}
	return si;
}

//computes part of the self information of the NMCCS C based on the 3ary relationship rel_type (ONLY VALID WHEN ALL THREE TYPES ARE PRESENT)
double RMiner::self_info_3ary_rel(vector<set<unsigned int> >* C, unsigned int rel_type){
	double si=0.0;
	set<unsigned int>::const_iterator sit1,sit2,sit3;
	set<unsigned int>::const_iterator it1,it2,it3;

	vector<unsigned int> entity_types = (*RelsToTypes)[rel_type];

	for (unsigned int i=0; i<entity_types.size(); i++){
		
        for (sit1=(*C)[entity_types[0]].begin(); sit1!=(*C)[entity_types[0]].end(); ++sit1){
            
            for (sit2=(*C)[entity_types[1]].begin(); sit2!=(*C)[entity_types[1]].end(); ++sit2){
                
                for (sit3=(*C)[entity_types[2]].begin(); sit3!=(*C)[entity_types[2]].end(); ++sit3){
                    
                    si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2],(*EntityToModelIndex)[*sit3]);
                }
            }
        }
                
    }
	return si;
}
//computes the self information of the NMCCS C as a sum over the self information for every relationship present in the pattern
//(remember the MaxEnt model is a product of independent distributions one for every relationship type)
double RMiner::self_info(vector<set<unsigned int> >* C, bool printProbs){
	double si = 0.0;
	int types_involved = 0;
    set<unsigned int>::const_iterator sit1, sit2;

	for (unsigned int i=0; i<(*RelsToTypes).size(); i++){
        if (excludedRelIdsFromMeasure.find(i)!=excludedRelIdsFromMeasure.end()){
            continue;
        }
		types_involved = 0;
		for (unsigned int j=0; j<(*RelsToTypes)[i].size(); j++){
			if (!(*C)[(*RelsToTypes)[i][j]].empty()){
				types_involved++;
			}
		}
		if (types_involved>=2){
			if ((*RelsToTypes)[i].size()==2){
                set<unsigned int> entities;
                set<unsigned int>::const_iterator cit;
                int po_type_index = -1;
                unsigned int no_type_index = -1;
                
                for (unsigned int t=0; t<(*RelsToTypes)[i].size(); t++){
                    if ((*typeOfType)[(*RelsToTypes)[i][t]]=='o' || (*typeOfType)[(*RelsToTypes)[i][t]]=='p'){
                        po_type_index=t;
                        
                    }
                    else {
                        no_type_index=t;
                    }
                }
                //cout<<"test: ";
                if (po_type_index!=-1){
                    for (sit1=(*C)[(*RelsToTypes)[i][no_type_index]].begin(); sit1!=(*C)[(*RelsToTypes)[i][no_type_index]].end(); sit1++){
                        double p=prob_binary_rel((*C)[(*RelsToTypes)[i][po_type_index]],*sit1,entities,i,0,po_type_index,printProbs);
                        if (p<1.0e-8)
                            si+=p;
                        else
                            si-=log(1-p);
                    }
                   //cout<<-log(1-p)<<" ";
                }
                else {
                    si+=self_info_binary_rel(C,i);
                    //cout<<self_info_binary_rel(C,i)<<endl;
                }
			}
			else {
                set<unsigned int> entities;
                set<unsigned int>::const_iterator cit;
                int po_type_index = -1;
                vector<unsigned int> no_type_indices;
            
                for (unsigned int t=0; t<(*RelsToTypes)[i].size(); t++){
                    if ((*typeOfType)[(*RelsToTypes)[i][t]]=='o' || (*typeOfType)[(*RelsToTypes)[i][t]]=='p'){
                        po_type_index=t;
                    }
                    no_type_indices.push_back(t);
                }
                
                if (po_type_index!=-1){
                    for (sit1=(*C)[(*RelsToTypes)[i][no_type_indices[0]]].begin(); sit1!=(*C)[(*RelsToTypes)[i][no_type_indices[0]]].end(); sit1++){
                        for (sit2=(*C)[(*RelsToTypes)[i][no_type_indices[1]]].begin(); sit2!=(*C)[(*RelsToTypes)[i][no_type_indices[1]]].end(); sit2++){
                            double p = prob_triary_rel((*C)[(*RelsToTypes)[i][po_type_index]],*sit1,*sit2,entities,i,0,po_type_index,printProbs);
                            if (p<1.0e-8)
                                si+=p;
                            else
                                si-=log(1-p);

                        }
                    }
                }
                else
                    si+=self_info_3ary_rel(C,i);
			}
		}

	}

	return si;
}

//computes part of the self information of the NMCCS C based on the binary relationship rel_type, in the case the iterative output of patterns is used,
//which at every iteration takes into account only the self information of relationship instances not already conveyed to the user.
double RMiner::self_info_binary_rel(vector<set<unsigned int> > C, unsigned int rel_type, vector<set<unsigned int> >* Conveyed){
	double si=0.0;
	set<unsigned int>::const_iterator sit1, sit2;
	set<unsigned int>::const_iterator it1,it2;

	vector<unsigned int> entity_types = (*RelsToTypes)[rel_type];

	for (sit1=(C)[entity_types[0]].begin(); sit1!=(C)[entity_types[0]].end(); ++sit1){
        if ((*EntityToModelIndex).find(*sit1)!=(*EntityToModelIndex).end()){
            for (sit2=(C)[entity_types[1]].begin(); sit2!=(C)[entity_types[1]].end(); ++sit2){
                if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end())){
                    if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                            si-=Models2[rel_type]->logProbability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2]);
                    }
                    else {
                        double prod=1;
                        for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                prod*=(1-Models2[rel_type]->probability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*it2]));
                        }
                        si-=log(1-prod);
                    }
                }
            }
        }
        else {
            for (sit2=(C)[entity_types[1]].begin(); sit2!=(C)[entity_types[1]].end(); ++sit2){
                if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end())){
                    double prod=1;
                    for (it1=(*implied_entities)[*sit1].begin(); it1!=(*implied_entities)[*sit1].end(); it1++){
                        if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                                prod*=(1-Models2[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*sit2]));
                        }
                        
                        for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                prod*=(1-Models2[rel_type]->probability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2]));
                        }
                    }
                    si-=log(1-prod);
                }
            }
        }
	}
	return si;
}

//computes part of the self information of the NMCCS C based on the 3-ary relationship rel_type in the case the iterative output of patterns is used,
//which at every iteration takes into account only the self information of relationship instances not already conveyed to the user.
double RMiner::self_info_3ary_rel(vector<set<unsigned int> > C, unsigned int rel_type, vector<set<unsigned int> >* Conveyed){
	double si=0.0;
	set<unsigned int>::const_iterator sit1, sit2, sit3;
	set<unsigned int>::const_iterator it1, it2, it3;

	vector<unsigned int> entity_types = (*RelsToTypes)[rel_type];

	for (unsigned int i=0; i<entity_types.size(); i++){
		//the cases when one of the entity types of the 3-ary relationship is not present in the pattern
		if ((C)[entity_types[0]].empty()){
			for (sit1=(C)[entity_types[1]].begin(); sit1!=(C)[entity_types[1]].end(); ++sit1){
                if ((*EntityToModelIndex).find(*sit1)!=(*EntityToModelIndex).end()){
                    for (sit2=(C)[entity_types[2]].begin(); sit2!=(C)[entity_types[2]].end(); ++sit2){
                        if (!((*Conveyed)[entity_types[1]].find(*sit1)!=(*Conveyed)[entity_types[1]].end() && (*Conveyed)[entity_types[2]].find(*sit2)!=(*Conveyed)[entity_types[2]].end())){
                            if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                                    si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2]);
                            }
                            for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                    si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*it2]);
                            }
                        }
                    }
                }
                else {
                    for (sit2=(C)[entity_types[2]].begin(); sit2!=(C)[entity_types[2]].end(); ++sit2){
                        if (!((*Conveyed)[entity_types[1]].find(*sit1)!=(*Conveyed)[entity_types[1]].end() && (*Conveyed)[entity_types[2]].find(*sit2)!=(*Conveyed)[entity_types[2]].end())){
                                for (it1=(*implied_entities)[*sit1].begin(); it1!=(*implied_entities)[*sit1].end(); it1++){
                                if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                                        si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*sit2]);
                                }
                                for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                        si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2]);
                                }
                            }
                        }
                    }
                }
			}
		}
		else if ((C)[entity_types[1]].empty()){
            for (sit1=(C)[entity_types[0]].begin(); sit1!=(C)[entity_types[1]].end(); ++sit1){
                if ((*EntityToModelIndex).find(*sit1)!=(*EntityToModelIndex).end()){
                    for (sit2=(C)[entity_types[2]].begin(); sit2!=(C)[entity_types[2]].end(); ++sit2){
                        if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[2]].find(*sit2)!=(*Conveyed)[entity_types[2]].end())){
                            if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                                    si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2]);
                            }
                            for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                    si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*it2]);
                            }
                        }
                    }
                }
                else {
                    for (sit2=(C)[entity_types[2]].begin(); sit2!=(C)[entity_types[2]].end(); ++sit2){
                        if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[2]].find(*sit2)!=(*Conveyed)[entity_types[2]].end())){
                            for (it1=(*implied_entities)[*sit1].begin(); it1!=(*implied_entities)[*sit1].end(); it1++){
                                if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                                        si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*sit2]);
                                }
                                for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                        si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2]);
                                }
                            }
                        }
                    }
                }
            }
		}
		else if ((C)[entity_types[2]].empty()){
            for (sit1=(C)[entity_types[0]].begin(); sit1!=(C)[entity_types[0]].end(); ++sit1){
                if ((*EntityToModelIndex).find(*sit1)!=(*EntityToModelIndex).end()){
                    for (sit2=(C)[entity_types[1]].begin(); sit2!=(C)[entity_types[1]].end(); ++sit2){
                        if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end())){
                            if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                                    si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2]);
                            }
                            for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                    si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*it2]);
                            }
                        }
                    }
                }
                else {
                    for (sit2=(C)[entity_types[1]].begin(); sit2!=(C)[entity_types[1]].end(); ++sit2){
                        if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end())){
                            for (it1=(*implied_entities)[*sit1].begin(); it1!=(*implied_entities)[*sit1].end(); it1++){
                                if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                                        si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*sit2]);
                                }
                                for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                        si-=Models3[rel_type]->logProbabilityColsTubs((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2]);
                                }
                            }
                        }
                    }
                }
            }
		}
		else {
			//the case when all entity types are present in the pattern
			for (sit1=(C)[entity_types[0]].begin(); sit1!=(C)[entity_types[0]].end(); ++sit1){
                if ((*EntityToModelIndex).find(*sit1)!=(*EntityToModelIndex).end()){
                    for (sit2=(C)[entity_types[1]].begin(); sit2!=(C)[entity_types[1]].end(); ++sit2){
                        if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                            for (sit3=(C)[entity_types[2]].begin(); sit3!=(C)[entity_types[2]].end(); ++sit3){
                                if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end() && (*Conveyed)[entity_types[2]].find(*sit3)!=(*Conveyed)[entity_types[2]].end())){
                                    if ((*EntityToModelIndex).find(*sit3)!=(*EntityToModelIndex).end()){
                                            si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2],(*EntityToModelIndex)[*sit3]);
                                    }
                                    for (it3=(*implied_entities)[*sit3].begin(); it3!=(*implied_entities)[*sit3].end(); it3++){
                                            si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*sit2],(*EntityToModelIndex)[*it3]);
                                    }
                                }
                            }
                        }
                        else {
                            for (sit3=(C)[entity_types[2]].begin(); sit3!=(C)[entity_types[2]].end(); ++sit3){
                                if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end() && (*Conveyed)[entity_types[2]].find(*sit3)!=(*Conveyed)[entity_types[2]].end())){
                                    for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                        if ((*EntityToModelIndex).find(*sit3)!=(*EntityToModelIndex).end()){
                                                si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*sit3]);
                                        }
                                        for (it3=(*implied_entities)[*sit3].begin(); it3!=(*implied_entities)[*sit3].end(); it3++){
                                                si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*sit1],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*it3]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                   for (sit2=(C)[entity_types[1]].begin(); sit2!=(C)[entity_types[1]].end(); ++sit2){
                       if ((*EntityToModelIndex).find(*sit2)!=(*EntityToModelIndex).end()){
                           for (sit3=(C)[entity_types[2]].begin(); sit3!=(C)[entity_types[2]].end(); ++sit3){
                               if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end() && (*Conveyed)[entity_types[2]].find(*sit3)!=(*Conveyed)[entity_types[2]].end())){
                                   for (it1=(*implied_entities)[*sit1].begin(); it1!=(*implied_entities)[*sit1].end(); it1++){
                                       if ((*EntityToModelIndex).find(*sit3)!=(*EntityToModelIndex).end()){
                                               si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*sit2],(*EntityToModelIndex)[*sit3]);
                                       }
                                       for (it3=(*implied_entities)[*sit3].begin(); it3!=(*implied_entities)[*sit3].end(); it3++){
                                               si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*sit2],(*EntityToModelIndex)[*it3]);
                                       }
                                   }
                               }
                           }
                       }
                       else {
                           for (sit3=(C)[entity_types[2]].begin(); sit3!=(C)[entity_types[2]].end(); ++sit3){
                               if (!((*Conveyed)[entity_types[0]].find(*sit1)!=(*Conveyed)[entity_types[0]].end() && (*Conveyed)[entity_types[1]].find(*sit2)!=(*Conveyed)[entity_types[1]].end() && (*Conveyed)[entity_types[2]].find(*sit3)!=(*Conveyed)[entity_types[2]].end())){
                                   for (it2=(*implied_entities)[*sit2].begin(); it2!=(*implied_entities)[*sit2].end(); it2++){
                                       for (it1=(*implied_entities)[*sit1].begin(); it1!=(*implied_entities)[*sit1].end(); it1++){
                                           if ((*EntityToModelIndex).find(*sit3)!=(*EntityToModelIndex).end()){
                                                   si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*sit3]);
                                           }
                                           for (it3=(*implied_entities)[*sit3].begin(); it3!=(*implied_entities)[*sit3].end(); it3++){
                                                   si-=Models3[rel_type]->logProbability((*EntityToModelIndex)[*it1],(*EntityToModelIndex)[*it2],(*EntityToModelIndex)[*it3]);
                                           }
                                       }
                                   }
                               }
                           
                           }
                       }
                   }
                }
			}
		}
	}
	return si;
}

//computes the self information of the NMCCS C in the case the iterative output of patterns is used,
//which at every iteration takes into account only the self information of relationship instances not already conveyed to the user.
//(remember the MaxEnt model is a product of independent distributions one for every relationship type)
double RMiner::self_info(vector<set<unsigned int> > C, vector<set<unsigned int> >* Conveyed){
	double si = 0.0;
	int types_involved = 0;

	for (unsigned int i=0; i<(*RelsToTypes).size(); i++){
        if (excludedRelIdsFromMeasure.find(i)!=excludedRelIdsFromMeasure.end()){
            continue;
        }
        // if 
		types_involved = 0;
		for (unsigned int j=0; j<(*RelsToTypes)[i].size(); j++){
			if (!(C)[(*RelsToTypes)[i][j]].empty()){
				types_involved++;
			}
		}
		if (types_involved>=2){
			if ((*RelsToTypes)[i].size()==2){
				si+=self_info_binary_rel(C,i,Conveyed);
			}
			else {
				si+=self_info_3ary_rel(C,i,Conveyed);
			}
		}

	}

	return si;
}

//computes the description length of the pattern C using the global variable p as a parameter
double RMiner::desc_length(vector<set<unsigned int> >* C){
	vector<set<unsigned int> >::const_iterator sit;
	int entitiesIn=0;
	double res;

	for (sit=C->begin(); sit!=C->end(); sit++){
		entitiesIn+=sit->size();
	}
	res = -entitiesIn*log(p)-(numOfEntities-entitiesIn)*log(1-p);
	return res;
}

//from the set of all NMCCSs it iteratively prints k, each time considering
//the sel-information of the relationship instances not already presented to the user
//when k=0 it simply sorts and prints the whole list of patterns.
void RMiner::iteratively_print(int k, fstream& outfile){
	int i=0;
	
	set<unsigned int>::const_iterator sit;
	vector<inode>::iterator lit, eit;
    set<unsigned int> set_int;
	vector<set<unsigned int> > entitiesAlreadyConveyed;

    if (k!=0){
        while (i<k && i<NMCCSsList.size()){
            i++;
            i_sort sortingfunction;
            sort(InfoList.begin(), InfoList.end(), sortingfunction);
            int j = InfoList[0].patternIndex;
            outfile<<InfoList[0].interestingness<<" ";
        
            for (unsigned int m=0; m<NMCCSsList[j].size(); m++){
                entitiesAlreadyConveyed.push_back(set_int);
                for (sit=NMCCSsList[j][m].begin(); sit!=NMCCSsList[j][m].end(); ++sit){
                    if (!is_implied(*sit, m, NMCCSsList[j][m])){
                        outfile<<(*EntityIdToEntityName)[*sit]<<" ";
                        entitiesAlreadyConveyed[m].insert(*sit);
                    }
                }
            }
        
            outfile<<endl;
            InfoList.erase(InfoList.begin());
            for (unsigned int m=0; m<InfoList.size(); m++){
                int c = InfoList[m].patternIndex;
                vector<set<unsigned int> > C_ni;
                set<unsigned int>::const_iterator it;
                
                for (unsigned int p=0; p<NMCCSsList[c].size(); p++){
                    set<unsigned int> temp;
                    for (it=NMCCSsList[c][p].begin(); it!=NMCCSsList[c][p].end(); it++){
                        if (!is_implied(*it,p,NMCCSsList[c][p]))
                            temp.insert(*it);
                    }
                    C_ni.push_back(temp);
                    temp.clear();
                }
                //InfoList[m].interestingness=self_info(C_ni,&entitiesAlreadyConveyed)/(InfoList[m].desc_length);
                // we removed the descLength part because it is calculated in postprocessing
                InfoList[m].interestingness=self_info(C_ni,&entitiesAlreadyConveyed);
                C_ni.clear();
            }
        }
    }
    else {
        if (compute_interestingness){
            i_sort sortingfunction;
            sort(InfoList.begin(), InfoList.end(), sortingfunction);
            for (unsigned int j=0; j<NMCCSsList.size(); j++){
                int k=InfoList[j].patternIndex;
                outfile<<InfoList[j].interestingness<<" ";
                for (unsigned int m=0; m<NMCCSsList[j].size(); m++){
                    for (sit=NMCCSsList[k][m].begin(); sit!=NMCCSsList[k][m].end(); ++sit){
                        if (!is_implied(*sit, m, NMCCSsList[k][m]))
                            outfile<<(*EntityIdToEntityName)[*sit]<<" ";
                    }
                }
                outfile<<endl;
            }
        }
        else {
            for (unsigned int j=0; j<NMCCSsList.size(); j++){
                for (unsigned int m=0; m<NMCCSsList[j].size(); m++){
                    for (sit=NMCCSsList[j][m].begin(); sit!=NMCCSsList[j][m].end(); ++sit){
                    if (!is_implied(*sit, m, NMCCSsList[j][m]))
                        outfile<<(*EntityIdToEntityName)[*sit]<<" ";
                    }
                }
                outfile<<endl;
            }
        }
    }
}

//returns true if the entity participates in at least one relationship instance for every relationship type
//that the entity type participates in, false otherwise (due to errors in the data)
bool RMiner::connected_to_all_rel_types(unsigned int entityType, unsigned int entity){

    /*cout<<(*EntityIdToEntityName)[entity]<<endl;
	for (unsigned int i=0; i<(*EntityTypeToRelTypes)[entityType].size(); i++){
        cout<<(*EntityTypeToRelTypes)[entityType][i]<<" "<<(*RelInstList)[entity][(*EntityTypeToRelTypes)[entityType][i]].size()<<endl;
		if ((*RelInstList)[entity][(*EntityTypeToRelTypes)[entityType][i]].empty())
			return false;
	}*/
	return true;
}

//evaluate the constraints on the augmentation elements of the entity
bool RMiner::eval_early_constraints(unsigned int entity){

	for (unsigned int i=0; i<(*EntityAugList)[entity].size(); i++){
        
        if ((*EntityAugList)[entity][i].size()<(*constraints)[i] && (*EntityAugList)[entity][i].size()!=0)
            return false;
        
	}
	return true;
}

//
bool RMiner::is_NMCCS(vector<set<unsigned int> >* C, vector<set<unsigned int> >* Comp, vector<bool>* activeEntityTypes, unsigned int level){
	
	set<unsigned int>::const_iterator sit;
	int sizeOfComp = 0;

    for(unsigned int i=0; i<Comp->size(); i++){
		if ((*activeEntityTypes)[i] || level==0){
			for (sit=(*Comp)[i].begin(); sit!=(*Comp)[i].end(); ++sit){
				if ((*C)[i].find(*sit)==(*C)[i].end()){
					sizeOfComp++;
				}
			}
		}
	}
	if (sizeOfComp==0)
		return true;
	else
		return false;
}

//checking compatibility of an element with part of a set corresponding to one relationship type
//iteratively constructs all critical sets and checks for compatibility
//rel_type is the relationship type
//entityTypes the entity types involved in this relationship type
//s the set
//s_rel_insts intersection of the relationship instances of the relationship rel_type of the entities in the critical set
bool RMiner::is_comp_with_one_rel(unsigned int rel_type, vector<unsigned int>* s_rel_insts, vector<set<unsigned int> >* s, set<unsigned int> entityTypes){

	set<unsigned int>::const_iterator tit;
	set<unsigned int>::const_iterator sit;
    set<unsigned int>::const_iterator dit;
    
    
	bool ok = true;

	if (s_rel_insts->empty())          //checks compatibility by checking whether the intersection of relationship instances is non-empty
		return false;

	for (tit=entityTypes.begin(); tit!=entityTypes.end(); ++tit){ //expands the current critical set
		for (sit=(*s)[*tit].begin(); sit!=(*s)[*tit].end(); ++sit){
			vector<unsigned int> new_s_rel_insts;
            if (s_rel_insts->size()<(*RelInstList)[*sit][rel_type].size())
                new_s_rel_insts.reserve(s_rel_insts->size());
            else
                new_s_rel_insts.reserve((*RelInstList)[*sit][rel_type].size());
            
            insert_iterator<vector<unsigned int> > it = insert_iterator<vector<unsigned int> >(new_s_rel_insts, new_s_rel_insts.begin());
            set_intersection(s_rel_insts->begin(), s_rel_insts->end(), (*RelInstList)[*sit][rel_type].begin(), (*RelInstList)[*sit][rel_type].end(), it); //computes intersection of the new set
            //}
            set<unsigned int> new_entityTypes=entityTypes;
			new_entityTypes.erase(*tit); //at the next recursion only consider different entity types (this is how critical sets are built)
			ok = is_comp_with_one_rel(rel_type, &new_s_rel_insts, s, new_entityTypes);
			if (!ok)
				return false;
		}
	}
	return ok;
}

int RMiner::find_num_of_entities(int type, vector<set<unsigned int> >* s){
    int size = 0;
    set<unsigned int>::const_iterator it, dit;
    queue<unsigned int> q;
    
    for (it=(*s)[type].begin(); it!=(*s)[type].end(); it++){
        if ((*Descendants).find(*it)!=(*Descendants).end()){
            size += (*implied_entities)[*it].size();
        }
        else {
            size++;
        }
    }
    
    return size;
}

//checks compatibility of an element with a set
bool RMiner::is_comp(int entity, vector<set<unsigned int> >* s){

	
	set<unsigned int>::const_iterator sit;
	set<unsigned int>::const_iterator nit;
    set<unsigned int>::const_iterator dit;

	unsigned int num_of_rel_insts=1;
	bool empty_rel=true;

	if (!connected_to_all_rel_types((*EntityToType)[entity], entity)){
		return false;
	}
	else {
        for (unsigned int j=0; j<(*RelInstList)[entity].size(); j++){
    
			set<unsigned int> entityTypes;
			num_of_rel_insts=1;
			empty_rel=true;

			for (unsigned int i=0; i<(*RelsToTypes)[j].size(); i++){
				if ((*RelsToTypes)[j][i]!=(*EntityToType)[entity]){
                    entityTypes.insert((*RelsToTypes)[j][i]);     //construct vector of all entity types of the relationship type except the type of the entity
                    /*if (!(*s)[(*RelsToTypes)[j][i]].empty()){     //calculates the number of critical sets that should be covered for the current relationship type
                        num_of_rel_insts*=find_num_of_entities((*RelsToTypes)[j][i], s);
                        empty_rel = false;
                     }*/
				}
			}

			//if (empty_rel)
				num_of_rel_insts=0;

			if (!(*RelInstList)[entity][j].empty()){                           //if empty it means that this entity type is not part of this relationship type
				if ((*RelInstList)[entity][j].size()>=num_of_rel_insts){       //actually only checks compatibility if the number of relationship instances that the entity participates is greater than the number of critical sets that should be covered
                    vector<unsigned int> rel_inst_list;
                    rel_inst_list = (*RelInstList)[entity][j];
					if (!is_comp_with_one_rel(j, &rel_inst_list, s, entityTypes))
							return false;
				}
				else {
					return false;
				}
			}

		}
	}

	return true;
}

void RMiner::find_Comp(unsigned int element, vector<ntype> new_orderOfTypes, vector<bool>* activeEntityTypes, vector<bool> new_activeEntityTypes, vector<set<unsigned int> >& new_Comp, vector<set<unsigned int> >* Comp, vector<set<unsigned int> >* newC){
    set<unsigned int>::const_iterator cit;
    for (unsigned int s=0; s<new_orderOfTypes.size(); s++){                    //the members of the C are just directly inserted
        unsigned int tt = new_orderOfTypes[s].type;
        if (new_activeEntityTypes[tt]){
            if (!(*activeEntityTypes)[tt])                                    //entity type has just become active
                new_Comp[tt]=(*EntityAugList)[element][tt];
            else {
                set<unsigned int> temp;
                new_Comp[tt]=temp;
                for (cit=(*Comp)[tt].begin(); cit!=(*Comp)[tt].end(); ++cit){
                    if ((*newC)[tt].find(*cit)!=(*newC)[tt].end())
                        new_Comp[tt].insert(*cit);
                    else {
                        bool a=is_comp(*cit, newC);
                        if (a)
                            new_Comp[tt].insert(*cit);
                    }
                }
            }
        }
        else {
            new_Comp[tt]=(*Comp)[tt];
        }
    }
}

vector<bool> RMiner::find_closure(vector<ntype> new_orderOfTypes, vector<bool> activeEntityTypes, vector<set<unsigned int> >* new_Comp, vector<set<unsigned int> >& closed_newC, set<unsigned int>* newB, vector<bool>& new_activeRelTypes, bool& overlap){
    
    set<unsigned int>::const_iterator sit;
    vector<bool> new_activeEntityTypes = activeEntityTypes;
    for (unsigned int s=0; s<new_orderOfTypes.size(); s++){
        unsigned int tt = new_orderOfTypes[s].type;
        if (activeEntityTypes[tt]){
            for (sit=(*new_Comp)[tt].begin(); sit!=(*new_Comp)[tt].end(); ++sit){
                if (closed_newC[tt].find(*sit)==closed_newC[tt].end()){
                    
                    if ((((*typeOfType)[tt]=='o' || (*typeOfType)[tt]=='p') && is_proper(*sit, tt, &closed_newC)) || ((*typeOfType)[tt]!='o' && (*typeOfType)[tt]!='p'))   {
                        bool a =is_comp(*sit, new_Comp);
                        if (a){
                            if (newB->find(*sit)!=newB->end()){
                                overlap=true;
                                break;
                            }
                            else {
                                closed_newC[tt].insert(*sit);
                                for (unsigned int k=0; k<(*RelInstList)[*sit].size(); k++){
                                    if ((*RelInstList)[*sit][k].size()>0){
                                        new_activeRelTypes[k]=true;
                                        vector<unsigned int> types = (*RelsToTypes)[k];					//find which entity types are involved in this rel type
                                        for (unsigned int j=0; j<types.size(); j++){
                                            if (types[j]!=(*EntityToType)[*sit])
                                                new_activeEntityTypes[types[j]]=true;                               //update the active entity types
                                            
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return new_activeEntityTypes;
}



//the actual algorithm C: an N-CCS, B:the set B as in the pseudocode, Comp:the set of compatible elements,
//activeRelTypes:boolean vector over all relationship types containing 1 if there is at least one entity in Cfrom an entity type participating in the relationship type
//activeEntityTypes:boolean vector over all entity types containing 1 if the entity type is related to at least one entity type already in the solution, 0 otherwise
//level: the depth of the search tree
//consat: boolean value specifying wheher the constraints have been met
void RMiner::run(vector<set<unsigned int> >* C, set<unsigned int>* B, vector<set<unsigned int> >* Comp, vector<bool>* activeRelTypes, vector<bool>* activeEntityTypes, unsigned int level, bool consat){

	set<unsigned int>::const_iterator sit;
	set<unsigned int>::const_iterator cit;
	set<unsigned int>::const_iterator nit;
    set<unsigned int>::const_iterator dit;

//*************************************************************
/** lines to get the current usage of space (only for Unix!!!)**/

	struct rusage usage;
		getrusage(RUSAGE_SELF, &usage);
		if (maxSpace<usage.ru_maxrss)
				maxSpace=usage.ru_maxrss;

//************************************************************
    
	if(is_NMCCS(C,Comp,activeEntityTypes,level)){  //if C is an NMCCS

		unsigned int pattern_size=0;
		for (unsigned int i=0; i<C->size(); i++){
			pattern_size+=(*C)[i].size();
		}

		NMCCSsList.push_back(*C);
        if (compute_interestingness){
            
            vector<set<unsigned int> > C_ni;
            set<unsigned int>::const_iterator it;
            bool print=false;
            for (unsigned int i=0; i<(*C).size(); i++){
                set<unsigned int> temp;
                for (it=(*C)[i].begin(); it!=(*C)[i].end(); it++){
                    if (!is_implied(*it,i,(*C)[i]))
                        temp.insert(*it);
                }
                
                C_ni.push_back(temp);
                temp.clear();
            }
            
            inode n;
            n.patternIndex = numOfNMCCSs;
            //n.desc_length = desc_length(&C_ni);
            n.desc_length = 1;
            //n.interestingness = self_info(&C_ni,print)/desc_length(&C_ni);
            // we have removed description length part, since it will be calculated in post processing:
            n.interestingness = self_info(&C_ni,print);
            
            InfoList.push_back(n);
            
            C_ni.clear();
        }

		if (pattern_size>sizeOfMaxNMCCS)
			sizeOfMaxNMCCS=pattern_size;

		numOfNMCCSs++;
        //cout<<"NMCCS: "<<numOfNMCCSs<<" ";
        //cout<<"depth: "<<level<<endl;
        
        

	}
	else {

		//decide the order in which the entities are going to be considered
		//the order depends only on the entity type and is decided based on
		//the following rule (function p_sort): if there is an entity type
		//with an associated constraint that is not yet satisfied then entities
		//of this type are considered first. Otherwise the ordering of types depends
		//on their average connectivity (i.e. average number of related relationship instances)
		//entity types with lower av. connectivity are considered first.
        
        

		vector<ntype> new_orderOfTypes = (*orderOfTypes);
		for (unsigned int t=0; t<orderOfTypes->size(); t++){
			unsigned int nt = (*orderOfTypes)[t].type;
			if (((*C)[nt].size()<(*constraints)[nt]) || ((*typeOfType)[nt]!='o' && (*typeOfType)[nt]!='p'))
				new_orderOfTypes[t].priority=true;
		}
		p_sort sortingfunction;
		sort(new_orderOfTypes.begin(), new_orderOfTypes.end(), sortingfunction);
       /* cout<<"Comp: "<<endl;
        for (int i=0; i<(*Comp).size(); i++){
            for (sit=(*Comp)[i].begin(); sit!=(*Comp)[i].end(); sit++){
                cout<<(*EntityIdToEntityName)[*sit]<<" ";//<<" "<<EntityIdToEntityName[*sit]<<endl;
            }
            cout<<endl;
        }*/
        /*cout<<"CCS: ";
        for (unsigned int m=0; m<C->size(); m++){
            for (sit=(*C)[m].begin(); sit!=(*C)[m].end(); ++sit){
                cout<<(*EntityIdToEntityName)[*sit]<<" ";
            }
        }
        cout<<endl;*/

		set<unsigned int> newB = (*B);
		for (unsigned int t=0; t<new_orderOfTypes.size(); t++){
			if ((*activeEntityTypes)[new_orderOfTypes[t].type] || is_empty(C)){    //using the active node types ensures that we always pick elements from the Aug set.
			unsigned int nt = new_orderOfTypes[t].type;

			for (nit=(*Comp)[nt].begin(); nit!=(*Comp)[nt].end(); nit++){
                
               // cout<<"to insert: "<<level<<" "<<(*EntityIdToEntityName)[*nit]<<": "<<endl;
                
				if ((*C)[nt].find(*nit)==(*C)[nt].end()){//check if already there
                   
                        if (B->find(*nit)==B->end()){
                            
                            //if ((*typeOfType)[nt]=='o' || (*typeOfType)[nt]=='p')
                              //  cout<<"is proper: "<<is_proper(*nit, nt, C)<<endl;
                           
                            if ((((*typeOfType)[nt]=='o' || (*typeOfType)[nt]=='p') && is_proper(*nit, nt, C)) || ((*typeOfType)[nt]!='o' && (*typeOfType)[nt]!='p'))   {           //check properness if needed
                            
                                
                                
                                numOfPossibleBranches++;
                                vector<set<unsigned int> > newC = (*C);         //initialise the new set with the elements already in C

                                newC[nt].insert(*nit);                                    //add the current element
                                newB.insert(*nit);
                                
                                if (eval_early_constraints(*nit)){									//since for all types i in Aug(C) it holds that Aug_i(C)=Comp_i(C) and Aug(C union e)=Aug(C) intersection Aug(e), if the constraints are not satisfied in Aug(e) the upperbound of the constraint is going to be false
                                // cout<<"inserted: "<<(*EntityIdToEntityName)[*nit]<<endl;
                                 //cout<<"level: "<<level<<endl;

                                 vector<bool> new_activeRelTypes = (*activeRelTypes);
                                 vector<bool> new_activeEntityTypes = (*activeEntityTypes);

                               //for all rel types of the relationship instance list

                                for (unsigned int k=0; k<(*RelInstList)[*nit].size(); k++){
                                    if ((*RelInstList)[*nit][k].size()>0){
                                        new_activeRelTypes[k]=true;
                                        vector<unsigned int> types = (*RelsToTypes)[k];					//find which entity types are involved in this rel type
                                        for (unsigned int j=0; j<types.size(); j++){
                                            if (types[j]!=(*EntityToType)[*nit])
                                                new_activeEntityTypes[types[j]]=true;                               //update the active entity types
                                        }
                                    }
                                }

                                //update the set of compatible elements
                                vector<set<unsigned int> > new_Comp;
                                for (unsigned int s=0; s<new_orderOfTypes.size(); s++){
                                    set<unsigned int> set_int;
                                    new_Comp.push_back(set_int);
                                }
                                
                                find_Comp(*nit, new_orderOfTypes, activeEntityTypes, new_activeEntityTypes, new_Comp, Comp, &newC);
                                
                                
                                bool potential_consat;                                                              //true if the constraints are already satidfied or if the upper bound is true
                                if (consat){
                                    potential_consat=true;
                                }
                                else {
                                    potential_consat=eval_constraints(&new_Comp, &newC, &newB);
                                }
                                
                                

                                if (potential_consat){                                           //only compute closure if the upper
                                                                                                //bound of the constraint is satisfied and if not at depth 0.
                                    //cout<<"potential consat: "<<potential_consat<<endl;
                                    //**** Find closure ************//
                                    bool overlap = false;
                                    unsigned int prev_newC_size = size_of_vecset(&newC);
                                    vector<set<unsigned int> > closed_newC = newC;
                                    vector<bool> act = find_closure(new_orderOfTypes, new_activeEntityTypes, &new_Comp, closed_newC, &newB, new_activeRelTypes,overlap);
                                    vector<bool> prevact;
                                    while (size_of_vecset(&closed_newC)>prev_newC_size){
                                        prev_newC_size=size_of_vecset(&closed_newC);
                                        prevact=act;
                                        act = find_closure(new_orderOfTypes, prevact, &new_Comp, closed_newC, &newB, new_activeRelTypes,overlap);
                                    }
                                    
                                    

                                    bool newconsat;

                                    if (consat) {
                                        newconsat=true;
                                    }
                                    else {
                                        newconsat=true;
                                        for (unsigned int d=0; d<closed_newC.size(); d++){
                                            if (closed_newC[d].size()<(*constraints)[d])
                                                newconsat=false;
                                        }
                                    }
                                    if (!overlap){ //closure has no overlap with B
                                        set<unsigned int> closed_newB = newB;
                                        unsigned int new_level=level+1;
                                        numOfClosed++;
                                        
                                        if (new_level>maxDepth)
                                            maxDepth=level;
                                        run(&closed_newC, &closed_newB, &new_Comp, &new_activeRelTypes, &new_activeEntityTypes, new_level, newconsat);
                                        closed_newB.clear();
                                    }
                                    //else {
                                      //  cout<<"OVERLAP!!!"<<endl;
                                    //}
                                    closed_newC.clear();
                                }
                                new_activeRelTypes.clear();
                                new_activeEntityTypes.clear();
                                newC.clear();
                                new_Comp.clear();
                            //}
                            }
                        }
                    }
				}
			}
		}
		}
	}
}



