package model;

import java.util.ArrayList;

import org.apache.lucene.util.OpenBitSet;

import interfaces.IDescriptionLength;
import utils.DesignPoint;
import utils.Statistics;

public class IntersectGreedyDescCalculator extends ADescriptionCalculator {
	private OpenBitSet intermVBitset;
	private Graph graph;

	public IntersectGreedyDescCalculator(int nbVertices, Graph graph, int maxNbNeighborhoods,
			IDescriptionLength descriptionLengthFunction, DesignPoint designPoint,Statistics statistics) {
		super(maxNbNeighborhoods, descriptionLengthFunction, designPoint, graph,statistics);
		intermVBitset = new OpenBitSet(nbVertices);
	}

	@Override
	public void calculateDescription(Pattern p) {
		// we will find best solution for equivalent problem: set cover of
		// V-p.subgraph
		// in other terms, we have candidates Ni(v) (with vj \in p.subgraph),
		// and we have candidates v \in V - p.subgraph
		// at each time, we add the candidate that prune the most from
		// V-p.subgraph
		// 1. we calculate candidates:

		// for each v \in p, we chose the tightest Ni(v)
		ArrayList<Pair<Integer, Integer>> candDescriptions = new ArrayList<>();

		for (Pair<Integer, Integer> pair : p.Ni_v) {
			try {
				candDescriptions.add(pair.clone()); // it is possible to remove
													// clone I think. For now I
													// did it cuz I'm still not
													// sure 100%
				graph.vertices[pair.second].updateKNeighborsBitSet(graph.vDistances, pair.first);
			} catch (CloneNotSupportedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		// from these candidates, we take the one locally optimal,
		// and we prune other solutions that includes completely the current
		// intersection
		SubgraphDescription desc = new SubgraphDescription();
		intermVBitset.set(0, graph.vertices.length);
		for (int vId : p.vertexIndices) {
			intermVBitset.fastClear(vId);
		}
		//checkCandidatesWithMccs(candDescriptions, p);
		descriptionLengthFunction.pruneCandidates(candDescriptions, intermVBitset, graph, p);
		while (candDescriptions.size() > 0
				&& (maxNbNeighborhoods == -1 || (maxNbNeighborhoods > desc.description.size()))) {
			int index = descriptionLengthFunction.getBestCandidateGreedy(candDescriptions, intermVBitset, graph);
			Pair<Integer, Integer> best = candDescriptions.get(index);
			desc.description.add(best);
			candDescriptions.remove(index);
			intermVBitset.and(graph.vertices[best.second].kNeighborsBitSet);
			descriptionLengthFunction.pruneCandidates(candDescriptions, intermVBitset, graph, p);
		}

		desc.XMinusP = new ArrayList<>();
		// if (designPoint.writeDetails){
		BitSetIterator iterator = new BitSetIterator(intermVBitset);
		int curIt = -1;
		while ((curIt = iterator.getNext()) >= 0) {
			desc.XMinusP.add(curIt);
		}
		// }
		desc.nbExceptions = (int) intermVBitset.cardinality();
		descriptionLengthFunction.updateDescriptionLength(desc, graph, p);
		p.subgraphDescription = desc;
		calculateAttributesDescription(p);
	}

}
