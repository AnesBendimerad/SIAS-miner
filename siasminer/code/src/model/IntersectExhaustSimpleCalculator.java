package model;

import java.util.ArrayList;

import org.apache.lucene.util.OpenBitSet;

import interfaces.IDescriptionLength;
import utils.DesignPoint;
import utils.Statistics;

public class IntersectExhaustSimpleCalculator extends ADescriptionCalculator {

	private int bitSetCurseur;
	private ArrayList<OpenBitSet> stackOfBitSets;
	private int nbVertices;
	private double currentSmallestLength = -1;
	private SubgraphDescription bestDescription = null;
	// private int curCall=0;
	// private int nbRecursive=0;
	private OpenBitSet testsBitSet;
	private OpenBitSet testsBitSet2;

	public IntersectExhaustSimpleCalculator(int nbVertices, Graph graph, int maxNbNeighborhoods,
			IDescriptionLength descriptionLengthFunction, DesignPoint designPoint,Statistics statistics) {
		super(maxNbNeighborhoods, descriptionLengthFunction, designPoint, graph,statistics);
		this.nbVertices = nbVertices;
		this.stackOfBitSets = new ArrayList<>();
		testsBitSet = new OpenBitSet(nbVertices);
		testsBitSet2 = new OpenBitSet(nbVertices);
	}

	@Override
	public void calculateDescription(Pattern p) {
		long startTime = System.currentTimeMillis();
		ArrayList<Pair<Integer, Integer>> candDescriptions = new ArrayList<>();

		for (Pair<Integer, Integer> pair : p.Ni_v) {
			try {
				candDescriptions.add(pair.clone()); // it is possible to remove
													// clone I think. For now I
													// did it cuz I'm still not
													// sure 100%
				graph.vertices[pair.second].updateKNeighborsBitSet(graph.vDistances, pair.first);
			} catch (CloneNotSupportedException e) {
				e.printStackTrace();
			}
		}
		SubgraphDescription desc = new SubgraphDescription();
		bestDescription = new SubgraphDescription();
		bitSetCurseur = 0;
		// currentSmallestLength=nbVertices;
		currentSmallestLength = -1;
		OpenBitSet intermVBitset = getBitSetOrCreate(bitSetCurseur);
		intermVBitset.set(0, nbVertices);
		for (int vId : p.vertexIndices) {
			intermVBitset.fastClear(vId);
		}
		//removeAboveMaxDist(candDescriptions);
		// pruneCandidates(candDescriptions);
		descriptionLengthFunction.pruneCandidates(candDescriptions, stackOfBitSets.get(bitSetCurseur), graph, p);
		pruneCandidates2(candDescriptions);
		// nbRecursive=0;
		enumerateDescriptions(desc, candDescriptions, p, true);
		// System.out.println("nbRecursive:"+nbRecursive);
		p.subgraphDescription = bestDescription;
		calculateAttributesDescription(p);
		long stopTime = System.currentTimeMillis();
		statistics.redescriptionTimeInMS += (stopTime - startTime);

	}

	private void enumerateDescriptions(SubgraphDescription currentDesc, ArrayList<Pair<Integer, Integer>> candDescriptions,
			Pattern p, boolean firstTime) {
		// nbRecursive++;
		if (currentSmallestLength == -1 || descriptionLengthFunction.getLowerBound(currentDesc, candDescriptions, p,
				graph, stackOfBitSets.get(bitSetCurseur)) < currentSmallestLength) {
			if (firstTime) {
				OpenBitSet intermVBitset = stackOfBitSets.get(bitSetCurseur);
				double lengthOfCurrent = descriptionLengthFunction.getCurrentDescriptionLength(currentDesc, graph, p,
						intermVBitset);
				if (currentSmallestLength == -1 || lengthOfCurrent < currentSmallestLength) {
					currentSmallestLength = lengthOfCurrent;
					bestDescription.description.clear();
					bestDescription.description.addAll(currentDesc.description);

					bestDescription.XMinusP = new ArrayList<>();
					// if (designPoint.writeDetails){
					BitSetIterator iterator = new BitSetIterator(intermVBitset);
					int curIt = -1;
					while ((curIt = iterator.getNext()) >= 0) {
						bestDescription.XMinusP.add(curIt);
					}
					bestDescription.nbExceptions = bestDescription.XMinusP.size();
					// }
					// else {
					// bestDescription.nbExceptions=(int)intermVBitset.cardinality();
					// }
					descriptionLengthFunction.updateDescriptionLength(bestDescription, graph, p);
				}
			}
			if (candDescriptions.size() > 0
					&& (maxNbNeighborhoods == -1 || (currentDesc.description.size() < maxNbNeighborhoods))) {
				ArrayList<Integer> nbRemoved = new ArrayList<>();
				// int index=getBestCandidate(candDescriptions,nbRemoved);
				EncapsInt minNbErrors=new EncapsInt();
				int index = descriptionLengthFunction.getBestCandidateExhaustive(candDescriptions,
						stackOfBitSets.get(bitSetCurseur), graph, nbRemoved,minNbErrors);
				if (currentSmallestLength != -1
						&& descriptionLengthFunction.getLowerBound2(currentDesc, candDescriptions, p, graph,
								stackOfBitSets.get(bitSetCurseur), nbRemoved,minNbErrors) >= currentSmallestLength) {
					return;
				}
				Pair<Integer, Integer> best = candDescriptions.get(index);
				currentDesc.description.add(best);
				candDescriptions.remove(index);
				ArrayList<Pair<Integer, Integer>> newCandDesc = new ArrayList<>(candDescriptions);
				bitSetCurseur++;
				OpenBitSet intermVBitset = getBitSetOrCreate(bitSetCurseur);
				intermVBitset.clear(0, nbVertices);
				intermVBitset.or(stackOfBitSets.get(bitSetCurseur - 1));
				intermVBitset.and(graph.vertices[best.second].kNeighborsBitSet);
				// pruneCandidates(newCandDesc);
				descriptionLengthFunction.pruneCandidates(newCandDesc, stackOfBitSets.get(bitSetCurseur), graph, p);
				pruneCandidates2(newCandDesc);
				enumerateDescriptions(currentDesc, newCandDesc, p, true);
				bitSetCurseur--;
				currentDesc.description.remove(best);
				enumerateDescriptions(currentDesc, candDescriptions, p, false);
				candDescriptions.add(best);
			}
		}
	}

	private void pruneCandidates2(ArrayList<Pair<Integer, Integer>> candDescriptions) {
		// updated for new DL
		// this function has a complexity of O(candDescriptions.size()^2)
		int curIndex = 0;
		OpenBitSet intermVBitset = stackOfBitSets.get(bitSetCurseur);
		boolean deleteOne = false;
		while (curIndex < candDescriptions.size()) {
			int secIndex = curIndex + 1;
			Pair<Integer, Integer> curCand = candDescriptions.get(curIndex);
			testsBitSet.clear(0, nbVertices);
			testsBitSet.or(intermVBitset);
			testsBitSet.and(graph.vertices[curCand.second].kNeighborsBitSet);
			long sizeOfOne = testsBitSet.cardinality();
			deleteOne = false;
			while (secIndex < candDescriptions.size()) {
				Pair<Integer, Integer> secCand = candDescriptions.get(secIndex);
				testsBitSet2.clear(0, nbVertices);
				testsBitSet2.or(intermVBitset);
				testsBitSet2.and(graph.vertices[secCand.second].kNeighborsBitSet);
				long sizeOfTwo = testsBitSet.cardinality();
				long interSize = OpenBitSet.intersectionCount(testsBitSet, testsBitSet2);
				if (interSize == sizeOfOne) {
					candDescriptions.remove(secIndex);
				} else if (interSize == sizeOfTwo) {
					deleteOne = true;
					break;
				} else {
					secIndex++;
				}
			}
			if (deleteOne) {
				candDescriptions.remove(curIndex);
			} else {
				curIndex++;
			}
		}
	}

	public OpenBitSet getBitSetOrCreate(int index) {
		if (stackOfBitSets.size() > index) {
			return stackOfBitSets.get(index);
		} else if (stackOfBitSets.size() == index) {
			OpenBitSet bitSet = new OpenBitSet(nbVertices);
			stackOfBitSets.add(bitSet);
			return bitSet;
		} else {
			throw new RuntimeException("problem in bitsets stack management");
		}
	}
}
