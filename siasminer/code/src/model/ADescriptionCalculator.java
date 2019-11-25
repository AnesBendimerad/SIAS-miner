package model;

import interfaces.IDescriptionLength;
import utils.DesignPoint;
import utils.Statistics;

public abstract class ADescriptionCalculator {
	protected int maxNbNeighborhoods=-1;
	protected Graph graph;
	//protected int maxDist=-1;
	public IDescriptionLength descriptionLengthFunction;
	protected DesignPoint designPoint;
	protected final boolean pruneWithPNRResult=true;
	public Statistics statistics;
	private double log2Value=Math.log(2);
	public ADescriptionCalculator(int maxNbNeighborhoods,IDescriptionLength descriptionLengthFunction,DesignPoint designPoint,Graph graph,Statistics statistics) {
		this.designPoint=designPoint;
		this.maxNbNeighborhoods = maxNbNeighborhoods;
		this.descriptionLengthFunction=descriptionLengthFunction;
		this.graph=graph;
		this.statistics=statistics;
		//this.maxDist=maxDist;
	}

	public abstract void calculateDescription(Pattern p);
	
	
	protected void calculateAttributesDescription(Pattern p){
		p.attributesDescLength=(1+(double)p.charact.restrictions.size())*log2(graph.descriptorMetaData.attributesName.length);
		for (Triple<Integer,Double,Double> restriction : p.charact.restrictions){
			p.attributesDescLength+=1+log2((double)graph.descriptorMetaData.binsPerAttribute[restriction.first].length);
		}
		p.overalDescLength=p.attributesDescLength+p.subgraphDescription.subgraphDescLength;
	}
	private double log2(double value){
		return Math.log(value)/log2Value;
	}
//	protected void removeAboveMaxDist(ArrayList<Pair<Integer,Integer>> candDescriptions){
//		int cpt=0;
//		while (cpt<candDescriptions.size()){
//			if (candDescriptions.get(cpt).first>designPoint.maxDistance || candDescriptions.get(cpt).first<0){
//				candDescriptions.remove(cpt);
//			}
//			else {
//				cpt++;
//			}
//		}
//	}
	
	protected int getTightestNeighborhood(int curV, Pattern p) {
		// we use kNeighborsBitset to memorize kNeighbors
		int maxDist=0;
		for (int curIt : p.vertexIndices){
			if (graph.vDistances[curV][curIt] == -1) {
				maxDist = -1;
				return maxDist;
			}
			if (graph.vDistances[curV][curIt]>maxDist){
				maxDist=graph.vDistances[curV][curIt];
				if (maxDist > this.designPoint.maxDistance && pruneWithPNRResult) {
					return maxDist;
				}
			}
		}
		// update kNeighborsBitset
		graph.vertices[curV].updateKNeighborsBitSet(graph.vDistances, maxDist);
		return maxDist;
	}
	
}
