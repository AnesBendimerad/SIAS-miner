package model;

import java.util.ArrayList;
import java.util.Collections;

import org.apache.lucene.util.OpenBitSet;

import interfaces.IDescriptionLength;
import utils.DesignPoint;

public class LogarithmicDescLength implements IDescriptionLength {
	// description length is :
	// (description.size()+1)*(log(|V|)+log(|maxDist|))+(|X-T|+1)*log(N)
	private DesignPoint designPoint;
	private Graph graph;
	private int niMultiplier;
	private OpenBitSet intermBitSet;
	private OpenBitSet intermBitSet2;
	private OpenBitSet intermBitSet3;
	private double log2Value=Math.log(2);
	public LogarithmicDescLength(DesignPoint designPoint, Graph graph) {
		this.intermBitSet = new OpenBitSet(graph.vertices.length);
		this.intermBitSet2 = new OpenBitSet(graph.vertices.length);
		this.intermBitSet3 = new OpenBitSet(graph.vertices.length);
		this.designPoint = designPoint;
		this.graph = graph;
		if (this.designPoint.maxDistance >= 0) {
			niMultiplier = this.designPoint.maxDistance + 1;
		} else {
			niMultiplier = graph.biggestDistance;
		}
		// niMultiplier = graph.biggestDistance;
	}

	private double log2(double value){
		return Math.log(value)/log2Value;
	}
	
	@Override
	public void pruneCandidates(ArrayList<Pair<Integer, Integer>> candDescriptions, OpenBitSet remainingErrors,
			Graph graph, Pattern p) {
		// update for new DL
		// the proof of this method is in "Subjective Interestingness in subgraphs/proofsAndDrafts/pruneCandidates"
		// double
		
		double costByNi = log2(graph.vertices.length) + log2(niMultiplier);
		double initErrorsNb=remainingErrors.cardinality();
		double initialCostOfExceptions=((double)(1+initErrorsNb))*log2(initErrorsNb+(double)p.vertexIndices.size());
		int curIndex = 0;
		while (curIndex < candDescriptions.size()) {
			Pair<Integer, Integer> curCand = candDescriptions.get(curIndex);
			double interSize = OpenBitSet.intersectionCount(remainingErrors,
					graph.vertices[curCand.second].kNeighborsBitSet);
			//double gain = intermSize - interSize;
			double costReduction=(1+interSize)*log2(interSize+(double)p.vertexIndices.size());
			costReduction=initialCostOfExceptions-costReduction;
			if (costByNi >= costReduction) {
				// with this intersection, we haven't remove more than one
				// vertex
				// so it does not decrease the length in this step
				// and the same thing for future steps, so juste remove it!
				candDescriptions.remove(curIndex);
			} else {
				curIndex++;
			}
		}
	}

	@Override
	public int getBestCandidateGreedy(ArrayList<Pair<Integer, Integer>> candDescriptions, OpenBitSet remainingErrors,
			Graph graph) {
		int curCand = 0;
		int bestCand = -1;
		long smallestInterSize = graph.vertices.length;
		for (Pair<Integer, Integer> cand : candDescriptions) {
			long interSize = OpenBitSet.intersectionCount(remainingErrors,
					graph.vertices[cand.second].kNeighborsBitSet);
			if (interSize < smallestInterSize || (interSize == smallestInterSize && bestCand >= 0
					&& candDescriptions.get(bestCand).first > cand.first)) {
				smallestInterSize = interSize;
				bestCand = curCand;
			}
			curCand++;
		}
		return bestCand;
	}

	@Override
	public int getBestCandidateExhaustive(ArrayList<Pair<Integer, Integer>> candDescriptions,
			OpenBitSet remainingErrors, Graph graph, ArrayList<Integer> nbRemoved,EncapsInt minNbErrors) {
		// updated for new DL
		intermBitSet.clear(0,graph.vertices.length);
		intermBitSet.or(remainingErrors);
		int curCand = 0;
		int bestCand = -1;
		long smallestInterSize = graph.vertices.length;
		int curSize = (int) remainingErrors.cardinality();
		for (Pair<Integer, Integer> cand : candDescriptions) {
			long interSize = OpenBitSet.intersectionCount(remainingErrors,
					graph.vertices[cand.second].kNeighborsBitSet);
			intermBitSet.intersect(graph.vertices[cand.second].kNeighborsBitSet);
			if (interSize < smallestInterSize || (interSize == smallestInterSize && bestCand >= 0
					&& candDescriptions.get(bestCand).first > cand.first)) {
				smallestInterSize = interSize;
				bestCand = curCand;
			}
			nbRemoved.add(curSize - (int) interSize);
			curCand++;
		}
		minNbErrors.value=(int)intermBitSet.cardinality();
		Collections.sort(nbRemoved, Collections.reverseOrder());
		return bestCand;
	}

	@Override
	public void updateDescriptionLength(SubgraphDescription desc, Graph graph, Pattern p) {
		double costByNi = log2(graph.vertices.length) + log2(niMultiplier);
		double nbErrors=0;
		if (desc.XMinusP != null) {
			nbErrors=desc.XMinusP.size();
		}
		double costByError = log2(nbErrors+(double)p.vertexIndices.size());
		desc.subgraphDescLength = desc.description.size() * costByNi;
		if (desc.XMinusP != null) {
			desc.subgraphDescLength += (desc.XMinusP.size() * costByError);
		}
		desc.subgraphDescLength += (costByNi + costByError);
	}

	@Override
	public double getCurrentDescriptionLength(SubgraphDescription description, Graph graph, Pattern p,
			OpenBitSet remainingErrors) {
		// updated for new DL
		// here we didn't set XMinusP yet
		double costByNi = log2(graph.vertices.length) + log2(niMultiplier);
		double nbErrors=remainingErrors.cardinality();
		
		double costByError = log2(nbErrors+(double)p.vertexIndices.size());

		double descLength = (description.description.size() + 1) * costByNi;
		descLength += ((nbErrors + 1) * costByError);
		return descLength;
	}

	@Override
	public double getLowerBound(SubgraphDescription desc, ArrayList<Pair<Integer, Integer>> candDescriptions, Pattern p,
			Graph graph, OpenBitSet remainingErrors) {
		// updated for new DL
		double costByNi = log2(graph.vertices.length) + log2(niMultiplier);
		double curErrorNb=remainingErrors.cardinality();
		double costByError = log2(curErrorNb+(double)p.vertexIndices.size());
		double curMin = (desc.description.size() + 1 + 1) * costByNi;
		double lengthOfCurrent = (desc.description.size() + 1) * costByNi;
		// OpenBitSet intermVBitset=stackOfBitSets.get(bitSetCurseur);
		lengthOfCurrent += ((curErrorNb + 1) * costByError);
		curMin = (curMin > lengthOfCurrent) ? lengthOfCurrent : curMin;
		return curMin;
	}
	@Override
	public double getLowerBound3(Pattern p, OpenBitSet allowedCand) {
		double costByNi = log2(graph.vertices.length) + log2(niMultiplier);
		double costByError = log2(graph.vertices.length);
		intermBitSet.clear(0, graph.vertices.length); // intersection of "necessarily" errors
		int nbPotentialGood=p.vertexIndices.size()+(int)allowedCand.cardinality();
		
		intermBitSet2.clear(0, graph.vertices.length); // vertices which are not necessarilly errors
		intermBitSet2.or(allowedCand);
		for (int v : p.vertexIndices) {
			intermBitSet2.fastSet(v);
		}
		int cpt = 0;
		for (Pair<Integer, Integer> curNi : p.Ni_v) {
			if (cpt == 0) {
				for (int i = 0; i < graph.vertices.length; i++) {
					if (graph.sortedVDistances[curNi.second][i].first <= curNi.first) {
						if (!intermBitSet2.fastGet(graph.sortedVDistances[curNi.second][i].second)) {
							intermBitSet.fastSet(graph.sortedVDistances[curNi.second][i].second);
						}
					} else {
						break;
					}
				}
			} else {
				BitSetIterator iterator = new BitSetIterator(intermBitSet);
				int curIt = 0;
				while ((curIt = iterator.getNext()) >= 0) {
					if (graph.vDistances[curNi.second][curIt] > curNi.first) {
						intermBitSet.fastClear(curIt);
					}
				}
			}
			cpt++;
		}
		
		// intermBitSet contains that will be error for sure
		double minNbErrors=(double) intermBitSet.cardinality(); // with all Ni_v
		double maxNbErrors=graph.vertices.length-nbPotentialGood;  // without any Ni_v
		
		
		double lb=(maxNbErrors+1)*costByError;
		double var1=costByNi+(1+minNbErrors)*costByError;
		if (var1<lb){
			lb=var1;
		}
		return lb;

		
//		intermBitSet3.clear(0,graph.vertices.length); // union of "necessarily" errors
//		for (Pair<Integer, Integer> curNi : p.Ni_v) {
//			for (int i = 0; i < graph.vertices.length; i++) {
//				if (graph.sortedVDistances[curNi.second][i].first <= curNi.first) {
//					if (!intermBitSet2.fastGet(graph.sortedVDistances[curNi.second][i].second)) {
//						intermBitSet3.fastSet(graph.sortedVDistances[curNi.second][i].second);
//					}
//				} else {
//					break;
//				}
//			}
//		}
//		double nbUnionErrors=(double)intermBitSet3.cardinality();
//		ArrayList<Double> nbRemovedFromUnion=new ArrayList<Double>();
//		for (Pair<Integer, Integer> curNi : p.Ni_v) {
//			double nbErrors=0;
//			for (int i = 0; i < graph.vertices.length; i++) {
//				if (graph.sortedVDistances[curNi.second][i].first <= curNi.first) {
//					if (!intermBitSet2.fastGet(graph.sortedVDistances[curNi.second][i].second)) {
//						nbErrors++;
//					}
//				}
//			}
//			nbRemovedFromUnion.add(nbUnionErrors-nbErrors);
//		}
//		Collections.sort(nbRemovedFromUnion,Collections.reverseOrder());
//		
//		double lb=(maxNbErrors+1)*costByError;
//		double curNbError=nbUnionErrors;
//		boolean happen=false;
//		for (int i=0;i<nbRemovedFromUnion.size();i++){
//			if (happen){
//				break;
//			}
//			curNbError-=nbRemovedFromUnion.get(i);
//			if (curNbError<=minNbErrors){
//				curNbError=minNbErrors;
//				happen=true;
//			}
//			double curLb=(2+i)*costByNi+(curNbError)*costByError;
//			if (curLb<lb){
//				lb=curLb;
//			}
//		}
//		return lb;
		
		
		
		
	}

	@Override
	public double getLowerBound2(SubgraphDescription desc, ArrayList<Pair<Integer, Integer>> candDescriptions, Pattern p,
			Graph graph, OpenBitSet remainingErrors, ArrayList<Integer> nbRemoved,EncapsInt minNbErrors) {
		// calculating the minimum number of exceptions (if we consider all
		// vertices)
		// int nbMinDesc=0;
		// intermBitSet.clear(0,graph.vertices.length);
		// for (Pair<Integer, Integer> cand : candDescriptions){
		// intermBitSet.and(graph.vertices[cand.second].kNeighborsBitSet);
		// }
		// nbMinDesc=(int)intermBitSet.cardinality();

		double costByNi = log2(graph.vertices.length) + log2(niMultiplier);
		
		
		double sizeOfRest = (double) remainingErrors.cardinality();
		double initCostByError = log2((double)p.vertexIndices.size()+sizeOfRest);
		
		double bestLb = (desc.description.size() + 1) * costByNi + (sizeOfRest + 1) * initCostByError;
		int curSumRemoved = 0;
		boolean stop = false;
		for (int i = 0; i < nbRemoved.size(); i++) {
			if (stop) {
				break;
			}
			curSumRemoved += nbRemoved.get(i);
			//if (curSumRemoved >= sizeOfRest) {
			//	stop = true;
			//}
			
			double minCurRest=sizeOfRest-curSumRemoved;
			if (minCurRest<minNbErrors.value){
				minCurRest=minNbErrors.value;
				stop = true;
			}
			//double curLb = (sizeOfRest - curSumRemoved >= 0) ? sizeOfRest - curSumRemoved : 0;
			double curLb =minCurRest;
			curLb++;
			curLb *= log2((double)p.vertexIndices.size()+minCurRest);
			curLb += ((desc.description.size() + i + 1 + 1) * costByNi);
			bestLb = (curLb >= bestLb) ? bestLb : curLb;
		}
		return bestLb;
	}
}
