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

#include <dirent.h>
#include <iostream>
#include <errno.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <sstream>
#include <cstring>
#include "RMiner.h"

using namespace std;


void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

//comparison operator used to sort all the entities in terms of increasing number of the relationship instances
//they participate in, or in other words their connectivity.
struct bsort {
  bool operator()(cnode i,cnode j) {
	  if (i.connectivity<j.connectivity)
		  return true;
	  else
		  return false;
  }
}my_sort;

typedef struct queue_el {
    unsigned int id;
    int start;
    int end;
}Qel;

//populates ientities and isentities sets which correspond to all the implied and implying entities of an entity in the partial order
void populate(unsigned int entity, unsigned int entity_type, set<unsigned int> ans, map<unsigned int, set<unsigned int> >* Anscestors, unsigned int h, unsigned int maxheight, map<unsigned int, set<unsigned int> >& ientities, map<unsigned int, set<unsigned int> >& isentities, map<unsigned int, string>* EntityIdToEntityName, vector<set<unsigned int> >& Comp){
    set<unsigned int>::const_iterator it;
    unsigned int newh;
    
    for (it = ans.begin(); it!=ans.end(); it++){
        newh=h;
        isentities[entity].insert(*it);
        
        ientities[*it].insert(entity);
        Comp[entity_type].insert(*it);
        newh++;
        if (newh<=maxheight)
            populate(entity, entity_type, (*Anscestors)[*it],Anscestors,newh,maxheight,ientities,isentities, EntityIdToEntityName, Comp);
    }
    return;
}

//takes as input the set of numeric values and constructs the partial order
int construct_partial_order(set<double> numeric_values, string type, unsigned int type_id, unsigned int& last_index, int start, int end, unsigned int height, map<unsigned int, string>& EntityIdToEntityName, map<string, unsigned int>& EntityValueToEntityId, map<unsigned int, set<unsigned int> >& Descendants, map<unsigned int, set<unsigned int> >& Anscestors, vector<set<unsigned int> >& Comp, map<unsigned int, unsigned int>& EntityToType, map<unsigned int, set<unsigned int> >& imp_entities, map<unsigned int, set<unsigned int> >& imp_sup_entities){
    
    //initiallisation
    int nstart = start;
    int nend = end;
    
    int num_el_to_add = 0;  //this counts the number of entities in the partial order
    int num_width = (int) numeric_values.size();
    for (int i = 0; i <= height; i++) {
        num_width--;
        num_el_to_add += num_width;
    }
    
    set<double>::iterator ti;
    set<unsigned int>::const_iterator ci;
    
    queue<Qel> Q;
    set<double>::iterator it1, it2;
    it1 = numeric_values.begin();
    advance(it1,start);
    it2 = numeric_values.begin();
    advance(it2,end);
    
    
    int cid1, cid2;
    
    stringstream parent_stream;
    parent_stream<<type<<".["<<*it1<<" "<<*it2<<"]";
    string parent = parent_stream.str();
    EntityIdToEntityName[last_index]=parent;
    Qel qi;
    qi.id = last_index;
    EntityToType[last_index]=type_id;
    last_index++;
    qi.start = start;
    qi.end = end;
    Q.push(qi);
    
    while (!Q.empty()) {
        
        Qel pid_el;
        pid_el = Q.front();
        Q.pop();
        nstart = pid_el.start;
        nend = pid_el.end;
        
        it1 = numeric_values.begin();
        advance(it1,nstart);
        it2 = numeric_values.begin();
        advance(it2,nend);
    
        if (nstart!=nend-1){
            
            stringstream child1_stream;
            child1_stream<<type<<".["<<*next(it1)<<" "<<(*it2)<<"]";
            string child1 = child1_stream.str();
            if (EntityValueToEntityId.find(child1)!=EntityValueToEntityId.end())
                cid1 = EntityValueToEntityId[child1];
            else {
                cid1 = last_index;
                EntityToType[last_index]=type_id;
                EntityIdToEntityName[cid1]=child1;
                EntityValueToEntityId[child1]=cid1;
                Qel cid_el;
                cid_el.id = cid1;
                cid_el.start = nstart+1;
                cid_el.end  = nend;
                Q.push(cid_el);
                last_index++;
                
            }
            Anscestors[cid1].insert(pid_el.id);
            Descendants[pid_el.id].insert(cid1);
            
            
            
            stringstream child2_stream;
            child2_stream<<type<<".["<<(*it1)<<" "<<*prev(it2)<<"]";
            string child2 = child2_stream.str();
            if (EntityValueToEntityId.find(child2)!=EntityValueToEntityId.end())
                cid2 = EntityValueToEntityId[child2];
            else {
                cid2 = last_index;
                EntityToType[last_index]=type_id;
                EntityIdToEntityName[cid2]=child2;
                EntityValueToEntityId[child2]=cid2;
                Qel cid_el;
                cid_el.id = cid2;
                cid_el.start = nstart;
                cid_el.end  = nend-1;
                Q.push(cid_el);
                last_index++;
               
            }
            Anscestors[cid2].insert(pid_el.id);
            Descendants[pid_el.id].insert(cid2);
            
        }
        else {
            stringstream child1_stream;
            child1_stream<<type<<"."<<(*it1);
            string child1 = child1_stream.str();
            
            if (EntityValueToEntityId.find(child1)!=EntityValueToEntityId.end()){
                cid1 = EntityValueToEntityId[child1];
                Anscestors[cid1].insert(pid_el.id);
                Descendants[pid_el.id].insert(cid1);
                
                populate(cid1, type_id, Anscestors[cid1], &Anscestors, 0,height, imp_entities, imp_sup_entities, &EntityIdToEntityName, Comp);
            }
            
            stringstream child2_stream;
            child2_stream<<type<<"."<<(*it2);
            string child2 = child2_stream.str();
            
            if (EntityValueToEntityId.find(child2)!=EntityValueToEntityId.end()){
                cid2 = EntityValueToEntityId[child2];
                Anscestors[cid2].insert(pid_el.id);
                Descendants[pid_el.id].insert(cid2);
                
                populate(cid2, type_id, Anscestors[cid2], &Anscestors, 0,height, imp_entities, imp_sup_entities, &EntityIdToEntityName, Comp);
            }
        
        }
    }
    numeric_values.clear();
    
    return num_el_to_add;
}

int main(int argc, char **argv) {

	string line;
	string type;
    string field;
	int argnumber=0;


	string outfile;
    string baseline_o_dir;
    string baseline_n_dir;
    
	vector<string> OnetoOne;
	vector<string> Nary;
	vector<unsigned int> constraints;
	vector<unsigned int> read_constraints;

	DIR *dp;
	struct dirent *dirp;

	vector<vector<unsigned int> > RelInstIdToEntities; 			  //linking relationship instance ids to entities ids
	vector<vector<unsigned int> > RelsToTypes;					  //linking relationship types to the participating entity types
	vector<vector<vector<unsigned int> > > RelInstList;           //relationship instance ids for every entity and relationship type
	vector<vector<set<unsigned int> > > EntityAugList;            //for every entity, it stores the valid augmentation elements (or the entities it
                                                                  //is related to) in a different set for every entity type
    map<unsigned int, set<unsigned int> > Descendants;                         //for every non-categorical entity all the descendants of the hierarchy
    map<unsigned int, set<unsigned int> > Anscestors;                          //for every non-categorical entity all the anscestors of the hierarchy
    map<unsigned int, set<unsigned int> > implied_entities;
    map<unsigned int, set<unsigned int> > implied_sup_entities;
    
	vector<map<unsigned int,unsigned int> > EntityIDToUniqueID;  //structure used to give ids to entities in terms of increasing number of
                                                                  //relationship instances they participate in
	map<unsigned int, unsigned int> EntityToType;                            //linking every entity to its type
	//vector<vector<unsigned int> > EntityTypeToRelTypes;           //linking every entity type to the relationship types it participates
    
    
	map<unsigned int, unsigned int> EntityToModelIndex;                      //linking entity ids to their index in the MaxEnt model of the data
	map<unsigned int,string> EntityIdToEntityName;            //linking entity ids to the actual string they correspond to ex: film title, director name etc
    map<unsigned int,double> EntityIdToEntityValue;            //linking entity ids to the actual value they correspond to (only for numeric)
    
    map<string, unsigned int> EntityValueToEntityId;        //from value (as string) to Id (only for numeric)
	map<unsigned int, unsigned int> numOfEntitiesPerType;
	map<unsigned int, unsigned int> numOfRelInstsPerType;
	map<string, unsigned int> typeToTypeID;

	map<unsigned int, NullModel3Ary*> Models3;								//MaxEnt model for 3-ary relationships
	map<unsigned int, NullModel*> Models2;									//MaxEnt model for binary relationships

	//initialisations
	int entityID = 0;
    unsigned int numOfEntities = 0;
	unsigned int entityIndex = 0;
	unsigned int relInstIndex = 0;
	unsigned int relInstID = 0;
	unsigned int entityType = 0;
    unsigned int reltype=0;
	double numOfPossibleRelInsts = 0;
    unsigned int first_index = 0;
    unsigned int last_index = 0;

	vector<set<unsigned int> > C;							  //initial solution C
	vector<set<unsigned int> > Comp;						  //Compatible elements initialisation (i.e. all entities) (per type)
	vector<set<unsigned int> > RelInsts_C;					  //Relationship instance ids in the set C
	vector<set<unsigned int> > RelInsts_Comp;                 //Relationship instance ids in the set Comp
	vector<bool> activeRelTypes;							  //boolean vector over all relationship types containing 1 if there is at least one
                                                              //entity from an entity type participating in the relationship type
	vector<bool> activeEntityTypes;							  //boolean vector over all entity types containing 1 if the entity type is related
                                                              //to at least one entity type already in the solution, 0 otherwise
    
    vector<char> typeOfType;                                 //specifies whether an entity type is hierarchy
                                                            //to be used when constructing partial orders
    vector<unsigned int> height;                           //has a 10^6 element of an entity type that is not a partial order or is a partial order for which we consider the whole height of the tree or >0 element to specify a specific tree height to consider about a partial order.
    map<unsigned int, char> typeIDToTypeofType;

	vector<map <unsigned int, int> >::const_iterator tit;
	map <unsigned int, int>::const_iterator nit;
	vector<cnode> nodeVector;
    set<string> excludedRelNamesFromMeasure;
    set<int> excludedRelIdsFromMeasure;
    bool compute_interestingness=true;
    int num_to_print=200;
    bool read_nary=false;

    FILE* fp;
    char file[1000];
    char lineo[1000];
    char cons[50];
	fp = fopen(argv[1], "r");
    char* p;
    char *l;
    
    if (fp==NULL){
        fprintf(stderr, "Can't read %s\n", argv[1]);
        exit(1);
    }
	while (fgets(lineo, 1000, fp)!=NULL){ //read the config file
		argnumber++;
        bool isi=false;
        bool isr=false;
        //if (lineo.find('\r')!=string::npos || line.find('\n')!=string::npos)
            //line.erase(line.length()-1);
        l=strchr(lineo, '\r');
        if (l!=NULL)
            *l='\0';
        l=strchr(lineo, '\n');
        if (l!=NULL)
            *l='\0';
        //std::cout<<"whole line:"<<lineo<<std::endl;
        vector<string> elements=split(string(lineo), ' ');
        if (argnumber==1 || argnumber==2){
            isi=false;
            isr=false;
            
            char* f=lineo;
            while (*f!=' '){
                if (*f=='-'){
                    
                }
                else if (*f=='i')
                    isi=true;
                else if (*f=='r')
                    isr=true;
                else{
                    cout<<"Please specify the -r and -i parameters in the config file."<<endl;
                    return errno;
                }
                f++;
            }
            int i=0;
            char n[20];
            while (*f!='\0'){
                if (isi){
                    if (*f=='1')
                        compute_interestingness=true;
                    else
                        compute_interestingness=false;
                }
                else if (isr){
                    n[i]=*f;
                }
                
                f++;
                i++;
            }
            if (isr){
                n[i]='\0';
                num_to_print=atoi(n);
                if (compute_interestingness==false)
                    num_to_print=0;
            }
        }
		else if (argnumber==3){ //output file
			outfile=lineo;
            cout<<"read outfile: "<<lineo<<endl;
        }
        else if (elements[0].compare("-e")==0){
            vector<string> exRelNames=split(elements[1], ',');
            for (string exRel : exRelNames){
                excludedRelNamesFromMeasure.insert(exRel);
            }
            //cout<<"-e found "<<endl;
        }
		else {
            
            p=strchr(lineo, ' ');
            
            if (p!=NULL){ //one_to_one file with constraint
                
                
                int i=0;
                char* f;
                f=lineo;
                while (f!=p){
                    file[i] = *f;
                    i++;
                    f++;
                }
                file[i]='\0';
                
                FILE * pFile;
                pFile = fopen (file,"r");
                if (pFile==NULL){
                    cout<<"Error opening file: "<<file<<endl;
                    return errno;
                }
                fclose(pFile);
                cout<<"read one_to_one: "<<file<<" ";
                OnetoOne.push_back(file);
                height.push_back(1000000);
                i=0;
                f++;
                if (*f!=' '){
                    while (*f!=' '){
                        cons[i]=*f;
                        f++;
                        i++;
                    }
                }
                cons[i]='\0';
                //cout<<atoi(cons)<<endl;
                read_constraints.push_back(atoi(cons));
                constraints.push_back(0);
                f++;
                if (*f!='\0'){
                    typeOfType.push_back(*f);
                    cout<<*f<<endl;
                }
                f++;
                if (*f!='\0'){
                    height[height.size()-1]=atoi(f);
                }
            }
            else {
                
                //if (!read_nary){
                    if((dp  = opendir(lineo)) == NULL) {
                        cout << "Error(" << errno << ") nary opening directory: " <<lineo << endl;
                        return errno;
                    }
                    //read_nary = true;
                    cout<<"read nary dir: "<<lineo<<endl;
                    while ((dirp = readdir(dp)) != NULL) {
                        
                        if(string(dirp->d_name)==string(".") || string(dirp->d_name)==string("..") || string(dirp->d_name)==string(".svn"))
                            continue;
                        Nary.push_back(string(lineo)+string(dirp->d_name));
                        //init
                        activeRelTypes.push_back(false);
                        set<unsigned int> tempt;
                        RelInsts_Comp.push_back(tempt);
                        RelInsts_C.push_back(tempt);
                    }
                    closedir(dp);
            }
        }
    }
    
    for (unsigned int i=0; i<OnetoOne.size(); i++){
         if (typeOfType[i]!='h'){
            map<unsigned int,unsigned int> temp;
            set<unsigned int> temps;
            vector<unsigned int> tempv;
            EntityIDToUniqueID.push_back(temp);
            activeEntityTypes.push_back(false);
            Comp.push_back(temps);
            C.push_back(temps);
            
            fstream onestream(OnetoOne[i].c_str(),ios::in);
            getline(onestream,line);
            
            if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
                line.erase(line.length()-1);
            
            stringstream line_stream(line);
            getline(line_stream, type, ','); //gets the id
            getline(line_stream, type, ','); //gets the type
            
            if (typeToTypeID.find(type)==typeToTypeID.end()){
                typeToTypeID[type]=entityType;
                typeIDToTypeofType[entityType]=typeOfType[i];
                entityType++;
            }
         }
    }
    
	//process the relationship files first to get the number of relationship instances for every entity and sort the entities based on it
	for (unsigned int b=0; b<Nary.size(); b++){
        
		vector<unsigned int> types;
        vector<string> type_names;
		unsigned int i;
        
        reltype++;

		fstream narystream(Nary[b].c_str(),ios::in);
		getline(narystream, line);
		if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
					line.erase(line.length()-1);

		stringstream line_stream(line);
		while (getline(line_stream, type, ',')){

			types.push_back(typeToTypeID[type]);
            type_names.push_back(type);
            if (numOfEntitiesPerType.find(typeToTypeID[type])==numOfEntitiesPerType.end())
                numOfEntitiesPerType[typeToTypeID[type]]=0;
		}
        
		while(getline(narystream, line)){
			if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
				line.erase(line.length()-1);
			stringstream line_stream(line);
			i=0;
            
			while (getline(line_stream, field, ',')){
                
                if (EntityIDToUniqueID[types[i]].find(atoi(field.c_str()))==EntityIDToUniqueID[types[i]].end()){
                    cnode n;
                    n.type=types[i];
                    n.node=atoi(field.c_str());
                    n.connectivity=1;
                    n.model_index=numOfEntitiesPerType[types[i]];
                    nodeVector.push_back(n);
                    EntityIDToUniqueID[types[i]][atoi(field.c_str())]=nodeVector.size()-1;
                    numOfEntitiesPerType[types[i]]=numOfEntitiesPerType[types[i]]+1;
                    numOfEntities++;
                }
                else {
                    nodeVector[EntityIDToUniqueID[types[i]][atoi(field.c_str())]].connectivity++;
                }
				i++;
			}
		}
		types.clear();
	}

    
	//sort the entities based on number of rel instances
	sort(nodeVector.begin(), nodeVector.end(), my_sort);
    
    //find index where to start the ids of single entities (interval entities should always have smaller ids because they need to be entered first)
    for (unsigned int i=0; i<OnetoOne.size(); i++){
        
        if (typeOfType[i]=='o'){
            unsigned int num = (EntityIDToUniqueID[i].size()*(EntityIDToUniqueID[i].size()-1))/2;
            first_index+=num;
        }
        else {
            
        }
    }
    
    //read files defining a hierarchy
    for (unsigned int i=0; i<OnetoOne.size(); i++){
        if (typeOfType[i]=='h'){
            fstream onestream(OnetoOne[i].c_str(),ios::in);
            
            getline(onestream,line);
            
            if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
                line.erase(line.length()-1);
            
            stringstream line_stream(line);
            getline(line_stream, type, ','); //gets the type
            getline(line_stream, type, ','); //gets the type
            
            
            char delim;
            string id1, id2;
            string name;
            unsigned int idn1, idn2;
            while(getline(onestream,line)){
                
                if (line.find('\t')!=string::npos)
                    delim='\t';
                else
                    delim=',';
                
                if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
                    line.erase(line.length()-1);
                
                stringstream line_stream(line);
                getline(line_stream, id1, delim);
                getline(line_stream, id2, delim);
                
                idn1 = atoi(id1.c_str());
                idn2 = atoi(id2.c_str());
                
                //if it does not already exist (entities at the lowest level have already an id)
                if (EntityIDToUniqueID[typeToTypeID[type]].find(idn1)==EntityIDToUniqueID[typeToTypeID[type]].end()){
                    EntityIDToUniqueID[typeToTypeID[type]][idn1]=first_index;
                    EntityToType[first_index]=typeToTypeID[type];
                    Comp[typeToTypeID[type]].insert(first_index);
                    first_index++;
                }
                if (EntityIDToUniqueID[typeToTypeID[type]].find(idn2)==EntityIDToUniqueID[typeToTypeID[type]].end()){
                    EntityIDToUniqueID[typeToTypeID[type]][idn2]=first_index;
                    EntityToType[first_index]=typeToTypeID[type];
                    Comp[typeToTypeID[type]].insert(first_index);
                    first_index++;
                }
             }
        }
    }
    
    EntityAugList.reserve(nodeVector.size()+first_index);
    for (unsigned int i=0; i<nodeVector.size()+first_index; i++){
        
        vector<set<unsigned int> > vset;
        for (unsigned int j=0; j<entityType; j++){
            set<unsigned int> s;
            vset.push_back(s);
        }
        EntityAugList.push_back(vset);
        
        vector<vector<unsigned int> > r_set;
        for (unsigned int j=0; j<reltype; j++){
            vector<unsigned int> r;
            r_set.push_back(r);
            
        }
        RelInstList.push_back(r_set);
    }

	//change entity ids based on the sorting
	for (unsigned int i=0; i<nodeVector.size(); i++){
        //new ids for the entities
        unsigned int a = i+first_index;
		EntityIDToUniqueID[nodeVector[i].type][nodeVector[i].node]=a;
        EntityToModelIndex[a]=nodeVector[i].model_index;
        EntityToType[a]=nodeVector[i].type;
        
        
	}
    
    nodeVector.clear();

	//read the one_to_one files and assign a string to every entity id
	for (unsigned int a=0; a<OnetoOne.size(); a++){
       	if (typeOfType[a]!='h'){
        
        set<double> numeric_values; //to be filled only in the case of numeric entity types
        
        unsigned found = OnetoOne[a].find_last_of("/");
        string file = OnetoOne[a].substr(found);
        
		fstream onestream(OnetoOne[a].c_str(),ios::in);
		getline(onestream,line);

		if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
                	line.erase(line.length()-1);

		stringstream line_stream(line);
		getline(line_stream, type, ','); //gets the id
		getline(line_stream, type, ','); //gets the type
		
		entityType=typeToTypeID[type];
        
		constraints[entityType]=read_constraints[a];
        
		
		char delim;
		string id;
		string name;
		unsigned int idn;

		while(getline(onestream,line)){
            
			if (line.find('\t')!=string::npos)
				delim='\t';
			else
				delim=',';

			if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
				line.erase(line.length()-1);

			stringstream line_stream(line);
			getline(line_stream, id, delim);
			getline(line_stream, name, delim);
            
            if (name.at(name.length()-1)=='0')
                name = name.substr(0, name.size()-2);

			idn = atoi(id.c_str());
			if (EntityIDToUniqueID[entityType].find(idn)!=EntityIDToUniqueID[entityType].end()){
				entityID=EntityIDToUniqueID[entityType][idn];
				EntityIdToEntityName[entityID]=type+"."+name;
				Comp[entityType].insert(entityID);
                
                if (typeIDToTypeofType[entityType]=='o'){
                    EntityValueToEntityId[type+"."+name]=entityID;
                    numeric_values.insert(stod(name));
                }
				
			}
		}
		numOfRelInstsPerType[entityType]=0;
        
        if (typeIDToTypeofType[entityType]=='o'){
           
            numOfEntities += construct_partial_order(numeric_values, type, entityType, last_index, 0, numeric_values.size()-1, height[a], EntityIdToEntityName, EntityValueToEntityId, Descendants, Anscestors, Comp, EntityToType, implied_entities, implied_sup_entities);
            
            set<unsigned int>::const_iterator cit, ait;
            bool delet;
            for (cit=Comp[entityType].begin(); cit!=Comp[entityType].end(); cit++){
                delet = false;
                for (ait=Anscestors[*cit].begin(); ait!=Anscestors[*cit].end(); ait++){
                    if (Comp[entityType].find(*ait)==Comp[entityType].end()){
                        delet=true;
                        break;

                    }
                }
                if (delet){
                    set<unsigned int> temp;
                    Anscestors[*cit]=temp;
                }
            }
            
           //end of test
        }
        }
    }
    
    for (unsigned int i=0; i<OnetoOne.size(); i++){
        if (typeOfType[i]=='h'){
            fstream onestream(OnetoOne[i].c_str(),ios::in);
            
            getline(onestream,line);
            
            if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
                line.erase(line.length()-1);
            
            stringstream line_stream(line);
            getline(line_stream, type, ','); //gets the type
            getline(line_stream, type, ','); //gets the type
            
            char delim;
            string id1, id2;
            string name;
            unsigned int idn1, idn2;
            while(getline(onestream,line)){
                
                if (line.find('\t')!=string::npos)
                    delim='\t';
                else
                    delim=',';
                
                if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
                    line.erase(line.length()-1);
                
                stringstream line_stream(line);
                getline(line_stream, id1, delim);
                getline(line_stream, id2, delim);
                
                idn1 = atoi(id1.c_str());
                idn2 = atoi(id2.c_str());
                
                unsigned int parent = EntityIDToUniqueID[typeToTypeID[type]][idn1];
                unsigned int child = EntityIDToUniqueID[typeToTypeID[type]][idn2];
                
                Anscestors[child].insert(parent);
                Descendants[parent].insert(child);
                
            }
            
            set<unsigned int>::const_iterator cit;
            set<unsigned int> up_set;
            set<unsigned int>::const_iterator ci;
            set<unsigned int>::iterator uit;
            
            
            for (cit=Comp[typeToTypeID[type]].begin(); cit!=Comp[typeToTypeID[type]].end(); cit++){
                if (*cit>=first_index){
                    for (ci=Anscestors[*cit].begin(); ci!=Anscestors[*cit].end(); ci++){
                        up_set.insert(*ci);
                        implied_entities[*ci].insert(*cit);
                        implied_sup_entities[*cit].insert(*ci);
                    }
                    
                    while (!up_set.empty()){
                        
                        uit=up_set.begin();
                        
                        if (Anscestors.find(*uit)!=Anscestors.end()){
                            
                            for (ci=Anscestors[*uit].begin(); ci!=Anscestors[*uit].end(); ci++){
                                
                                up_set.insert(*ci);
                                implied_entities[*ci].insert(*cit);
                                implied_sup_entities[*cit].insert(*ci);
                            }
                           
                        }
                        up_set.erase(*uit);
                    }
                    up_set.clear();
                }
            }
        }
        
    }
    

	//Read again the relationship files to initialise structures, assign rel instance ids to entities and compute the number of rel instances for every entity in order to buid the MaxEnt model
    reltype=0;
	for (unsigned int b=0; b<Nary.size(); b++){
        
		relInstIndex = 0;
		vector<vector<int> > sumsdim;           //model marginals
		vector<unsigned int> typeIDs;
        vector<string> types;
        
        unsigned found = Nary[b].find_last_of("/");
        string file = Nary[b].substr(found);
        
        bool exludedFromMeasure=(excludedRelNamesFromMeasure.find(Nary[b].substr(found+1))!=excludedRelNamesFromMeasure.end());
        
        
		fstream narystream(Nary[b].c_str(),ios::in);
		getline(narystream, line);
        
        vector<set<unsigned int> > combs;
        
		if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
			line.erase(line.length()-1);

		stringstream line_stream(line);

		int relation_dim=0;
		double num=1;

		while (getline(line_stream, type, ',')){
            
			relation_dim++;
			num = num * numOfEntitiesPerType[typeToTypeID[type]];
			//associate every type with the relationship
			typeIDs.push_back(typeToTypeID[type]);
            
			//EntityTypeToRelTypes[typeToTypeID[type]].push_back(reltype);
			//initialise marginals for Null model
            types.push_back(type);
			vector<int> temp;
            
			for (unsigned int i=0; i<numOfEntitiesPerType[typeToTypeID[type]]; i++){
				temp.push_back(0);
			}
			sumsdim.push_back(temp);
			temp.clear();
		}
		RelsToTypes.push_back(typeIDs);
        if (exludedFromMeasure){
            excludedRelIdsFromMeasure.insert(((int)RelsToTypes.size())-1);
        }
		
		numOfPossibleRelInsts+=num;

		unsigned int i;
		unsigned int node=0;
		vector<unsigned int> fields;
		vector<unsigned int> newfields;
		string edge;
		while(getline(narystream, line)){
            
            for (int t=0; t<relation_dim; t++){
                set<unsigned int> combs_i;
                combs.push_back(combs_i);
            }
            
			if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
				line.erase(line.length()-1);
			stringstream line_stream(line);

			i=0;
			while (getline(line_stream, field, ',')){
				node=EntityIDToUniqueID[RelsToTypes[reltype][i]][atoi(field.c_str())];
				fields.push_back(node);
				sumsdim[i][EntityToModelIndex[node]]=sumsdim[i][EntityToModelIndex[node]]+1;
				i++;
			}
            
			relInstID++;
			relInstIndex++;
			RelInstIdToEntities.push_back(fields);
			RelInsts_Comp[reltype].insert(relInstID);

			for (int t=0; t<relation_dim; t++){
                
				RelInstList[fields[t]][reltype].push_back(relInstID);
                combs[t].insert(fields[t]);
                
                if (typeIDToTypeofType[typeIDs[t]]=='o' || typeIDToTypeofType[typeIDs[t]]=='p'){ /// for all anscestors add the relinst
                  
                    set<unsigned int>::const_iterator eid;
                    for (eid=implied_sup_entities[fields[t]].begin(); eid!=implied_sup_entities[fields[t]].end(); eid++){
                        
                        
                        RelInstList[*eid][reltype].push_back(relInstID);
                    
                        combs[t].insert(*eid);
                    }
                    
                    
                }
				
                for (int f=0; f<relation_dim; f++){
                    
					if (f!=t){
						EntityAugList[fields[t]][typeIDs[f]].insert(fields[f]);
                        if (typeIDToTypeofType[typeIDs[t]]=='o' || typeIDToTypeofType[typeIDs[t]]=='p'){
                            set<unsigned int>::const_iterator eid, kid;
                            for (eid=implied_sup_entities[fields[t]].begin(); eid!=implied_sup_entities[fields[t]].end(); eid++){
                                
                                EntityAugList[*eid][typeIDs[f]].insert(fields[f]);
                                if (typeIDToTypeofType[typeIDs[f]]=='o' || typeIDToTypeofType[typeIDs[f]]=='p'){
                                    for (kid=implied_sup_entities[fields[f]].begin(); kid!=implied_sup_entities[fields[f]].end(); kid++){
                                        EntityAugList[*eid][typeIDs[f]].insert(*kid);
                                    }
                                }
                            }
                            
                        }
                        if (typeIDToTypeofType[typeIDs[f]]=='o' || typeIDToTypeofType[typeIDs[f]]=='p'){
                            set<unsigned int>::const_iterator eid, kid;
                            for (eid=implied_sup_entities[fields[f]].begin(); eid!=implied_sup_entities[fields[f]].end(); eid++){
                                EntityAugList[fields[t]][typeIDs[f]].insert(*eid);
                                if (typeIDToTypeofType[typeIDs[t]]=='o' || typeIDToTypeofType[typeIDs[t]]=='p'){
                                    for (kid=implied_sup_entities[fields[t]].begin(); kid!=implied_sup_entities[fields[t]].end(); kid++){
                                        EntityAugList[*kid][typeIDs[f]].insert(*eid);
                                    }
                                }
                            }
                        }
                    }
				}
			}
            
            fields.clear();
            combs.clear();
        }
        
        if (!exludedFromMeasure){
            //builing the MaxEnt model
            if (relation_dim==2){
                NullModel* nm2 = new NullModel(&sumsdim[0], &sumsdim[1]);
                Models2[reltype]=nm2;
            }
            else if (relation_dim==3){
                NullModel3Ary* nm3 = new NullModel3Ary(&sumsdim[0], &sumsdim[1], &sumsdim[2]);
                Models3[reltype]=nm3;
            }
        }
		for (unsigned int i=0; i<RelsToTypes[reltype].size(); i++){
			numOfRelInstsPerType[RelsToTypes[reltype][i]]+=relInstIndex;
		}
		reltype++;
        sumsdim.clear();
    
    }
    
    map<unsigned int, set<unsigned int> >::const_iterator mit;
    set<unsigned int>::const_iterator sit;
    
  
    
    cout<<"-------------------------------------------------------------"<<endl;
    

	////Find order of node types based on average number of relationship instances

	 vector<ntype> order;

	 for (unsigned int i=0; i<OnetoOne.size(); i++){
         if (typeOfType[i]!='h'){
             fstream onestream(OnetoOne[i].c_str(),ios::in);
             getline(onestream,line);
         
             if (line.find('\r')!=string::npos || line.find('\n')!=string::npos)
                 line.erase(line.length()-1);
         
             stringstream line_stream(line);
             getline(line_stream, type, ','); //gets the id
             getline(line_stream, type, ','); //gets the type
             unsigned int et = typeToTypeID[type];
         
          
             ntype ct;
             ct.type=et;
             ct.average_degree = (double)numOfRelInstsPerType[et]/(double)numOfEntitiesPerType[et];
             ct.priority=false;
             order.push_back(ct);
          }
	 }

	 p_sort sortingfunction;
	 sort(order.begin(),order.end(),sortingfunction);

	bool csat=true;
	for (unsigned int i=0; i<order.size(); i++){
		if (constraints[i]!=0)
			csat=false;
	}
    

    EntityIDToUniqueID.clear();
    EntityValueToEntityId.clear();
    
	
	set<unsigned int> B;

	double density = ((double)relInstID)/numOfPossibleRelInsts; //the density of the data to be used as the parameter p in the description length
	fstream outputfile(outfile.c_str(),ios::out);
	clock_t begin = clock();
	RMiner* r = new RMiner(&RelInstIdToEntities, &RelsToTypes, &RelInstList, &EntityAugList, &EntityToType, &EntityToModelIndex, &EntityIdToEntityName, Models3, Models2, &constraints, density, numOfEntities, &order, compute_interestingness, &Descendants, &Anscestors, &implied_entities, &typeIDToTypeofType);
    
    r->setExcludedRelIdsFromMeasure(excludedRelIdsFromMeasure);
    
    //Closure of the empty set
    bool insert=true;

    /*for (int i=0; i<Comp.size(); i++){
        for (sit=Comp[i].begin(); sit!=Comp[i].end(); sit++){
            insert=true;
            for (unsigned int j=0; j<EntityAugList[*sit].size(); j++){
                if (EntityAugList[*sit][j].size()!=0){
                    if (EntityAugList[*sit][j].size()!=Comp[j].size()){
                        insert=false;
                            break;
                    }
                }
            }
            if (insert){
                C[i].insert(*sit);
                //cout<<"inserted before: "<<EntityIdToEntityName[*sit]<<endl;
                /*for (unsigned int j=0; j<EntityAugList[*sit].size(); j++){
                    cout<<"type: "<<j<<"  "<<EntityAugList[*sit][j].size()<<" "<<Comp[j].size()<<endl;
                }
                for (unsigned int j=0; j<EntityAugList[*sit].size(); j++){
                    if (EntityAugList[*sit][j].size()!=0){
                        activeEntityTypes[j]=true;
                    }
                }
                for (unsigned int k=0; k<(RelInstList)[*sit].size(); k++){
                    if ((RelInstList)[*sit][k].size()>0){
                        activeRelTypes[k]=true;
                    }
                }
            }
        }
        
        cout<<"-------------------------------------------------------------------------------"<<endl;
    }*/
    
    /*for (unsigned int i=0; i<EntityAugList.size(); i++){
        cout<<EntityIdToEntityName[i]<<": ";
        if (EntityToType[i]==2){
        for (unsigned int j=0; j<EntityAugList[i].size(); j++){
            for (sit=EntityAugList[i][j].begin(); sit!=EntityAugList[i][j].end(); sit++){
                cout<<EntityIdToEntityName[*sit]<<" ";
            }
        }
        }
        cout<<endl;
    }*/
    
	r->run(&C, &B, &Comp, &activeRelTypes, &activeEntityTypes, 0, csat);
	clock_t end = clock();
	double time_sec = (double(end - begin) / CLOCKS_PER_SEC);
	r->iteratively_print(num_to_print,outputfile);
	
	/************** Print the statistics *******************/
	cout<<"time (sec): "<<time_sec<<endl;
	cout<<"NMCCSs: "<<r->getNumOfNMCCSs()<<endl;
	cout<<"Max NMCCS: "<<r->getSizeOfMaxNMCCS()<<endl;
	cout<<"Max Depth: "<<r->getMaxDepth()<<endl;	
    cout<<"Num of closed: "<<r->getNumOfClosed()<<endl;
    cout<<"Num of possible branches: "<<r->getNumOfBranches()<<endl;
    cout<<"Max space: "<<r->getMaxSpace()<<endl;
    
    outputfile.close();
    RelInstIdToEntities.clear();
    RelsToTypes.clear();
    RelInstList.clear();
    EntityAugList.clear();
    EntityToType.clear();
    //EntityTypeToRelTypes.clear();
    EntityToModelIndex.clear();
    EntityIdToEntityName.clear();
    Descendants.clear();
    Anscestors.clear();
	
}
