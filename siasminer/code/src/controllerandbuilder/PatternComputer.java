package controllerandbuilder;

import java.util.ArrayList;

import org.apache.lucene.util.OpenBitSet;

import model.ADescriptionCalculator;
import model.BitSetIterator;
import model.Characteristic;
import model.Graph;
import model.Pair;
import model.Pattern;
import model.Triple;
import utils.DesignPoint;
import utils.Statistics;

public class PatternComputer {
	public ConstraintAndPruningManager cAndPManager;
	private Graph graph;
	private DesignPoint designPoint;
	private ClosureManager closureManager;
	private QualCalculator calculator;
	private Statistics statistics;
	private ADescriptionCalculator descCalculator;
	public int currentDepth;
	public int maxDepth;
	// private OpenBitSet intermBitSet;

	public PatternComputer(Graph graph, DesignPoint designPoint, Statistics statistics,
			ADescriptionCalculator descCalculator) {
		this.graph = graph;
		this.designPoint = designPoint;
		this.statistics = statistics;
		this.calculator = new QualCalculator(graph, designPoint);
		this.descCalculator = descCalculator;
		closureManager = new ClosureManager(graph, calculator, designPoint, statistics);
		cAndPManager = new ConstraintAndPruningManager(graph, designPoint);
		// intermBitSet=new OpenBitSet(graph.vertices.length);
	}

	public void retrievePatterns() {
		this.statistics.nbRecursiveCalls = 0;
		long startTime = System.currentTimeMillis();
		cAndPManager.foundPatterns = new ArrayList<>();
		// create first pattern
		//Pattern p = new Pattern(designPoint.useUB);
		Pattern p = new Pattern();
		// for (int i = 0; i < graph.vertices.length; i++) {
		// p.Ni_v.add(new Pair<Integer, Integer>(0, i));
		// }
		OpenBitSet allowedCand = new OpenBitSet(graph.vertices.length);
		OpenBitSet allCand = new OpenBitSet(graph.vertices.length);
		allowedCand.set(0, graph.vertices.length);
		allCand.set(0, graph.vertices.length);
//		if (designPoint.useUB) {
//			p.charact.bound.UbIC = closureManager.getInitialUbIC(p);
//		}
		currentDepth = maxDepth = 0;
		enumerateClosedPatterns(p, allowedCand, allCand);
		long stopTime = System.currentTimeMillis();
		statistics.miningTimeInMS = stopTime - startTime;
		statistics.nbFoundPatterns = cAndPManager.foundPatterns.size();
		System.out.println("maxDepth:" + maxDepth);
	}

	private void enumerateClosedPatterns(Pattern p, OpenBitSet allowedCand, OpenBitSet allCand) {
		maxDepth = (maxDepth > currentDepth) ? maxDepth : currentDepth;
		this.statistics.nbRecursiveCalls++;
		// long nbRec=this.statistics.nbRecursiveCalls;
		// if (nbRec==2276){
		// System.out.println("hoh");
		// }
		// System.out.println(p);

		if (p.vertexIndices.size() + allowedCand.cardinality() < designPoint.minVertices) {
			return;
		}
//		if (designPoint.useUB) {
//			p.charact.bound.LbDL = descCalculator.descriptionLengthFunction.getLowerBound3(p, allowedCand);
//			double ubSI = p.charact.bound.UbIC / p.charact.bound.LbDL;
//			if (cAndPManager.pruneWithUB(ubSI)) {
//				return;
//			}
//		}
		long startTime = System.currentTimeMillis();
		// int vToAdd = allowedCand.nextSetBit(0);
		int vToAdd = chooseWorstCandidate(p, allowedCand);
		long stopTime = System.currentTimeMillis();
		statistics.choosingCandTimeInMS += (stopTime - startTime);
		// int vToAdd = chooseCandidateMaximizingValidRestrictions(p,
		// allowedCand);
		if (vToAdd == -1) {
			if (cAndPManager.checkConstraintsBeforeDL(p)) {
				// here we can add pattern to result blablabla
				p.subgraphDescription = null;
				descCalculator.calculateDescription(p);
				//p.charact.subjectiveInterestingness = p.charact.informationContent / p.subgraphDescription.subgraphDescLength;
				p.charact.subjectiveInterestingness = p.charact.informationContent / p.overalDescLength;
				if (cAndPManager.checkConstraintsAfterDL(p)) {
					// if ( p.optimisedDescription.description.size()==1 &&
					// p.optimisedDescription.description.get(0).second==36 &&
					// p.optimisedDescription.description.get(0).first==3){
					// // check allCand:
					// BitSetIterator iterator=new BitSetIterator(allCand);
					// System.out.println(allCand.cardinality());
					// int curIt;
					// while ((curIt=iterator.getNext())>=0){
					// System.out.println(curIt);
					// }
					// System.out.println("voila");
					// }
					// try {
					cAndPManager.insertWithOrder(p);
					// } catch (CloneNotSupportedException e) {
					// e.printStackTrace();
					// }
				}
			}
		} else {
			ArrayList<Integer> removedNotAllowedCand = new ArrayList<>();
			ArrayList<Integer> removedAllowedCand = new ArrayList<>();
			ArrayList<Integer> addedToPattern = new ArrayList<>();
			//Characteristic saveCharact = new Characteristic(designPoint.useUB);
			Characteristic saveCharact = new Characteristic();
			ArrayList<Pair<Integer, Integer>> removedNi_v = new ArrayList<>();
			ArrayList<Pair<Integer, Integer>> modifiedNi_v = new ArrayList<>();
			startTime = System.currentTimeMillis();
			boolean success = closureManager.addAndclosePattern(p, vToAdd, allowedCand, allCand, removedNotAllowedCand,
					removedAllowedCand, addedToPattern, saveCharact, removedNi_v, modifiedNi_v, designPoint);
			stopTime = System.currentTimeMillis();
			statistics.closureTimeInMS += (stopTime - startTime);
			if (success) {
				currentDepth++;
				enumerateClosedPatterns(p, allowedCand, allCand);
				currentDepth--;
			}
			startTime = System.currentTimeMillis();
			backTrackPatternAndCand(p, allowedCand, allCand, removedNotAllowedCand, removedAllowedCand, addedToPattern,
					saveCharact, removedNi_v, modifiedNi_v);
			stopTime = System.currentTimeMillis();
			statistics.backtrackingTimeInMS += (stopTime - startTime);
//			double icOfLastAdded = 0;
//			if (designPoint.useUB) {
//				icOfLastAdded = closureManager.getMaxIcByVertex(vToAdd, p);
//				p.charact.bound.UbIC -= icOfLastAdded;
//			}
			allowedCand.fastClear(vToAdd);
			currentDepth++;
			enumerateClosedPatterns(p, allowedCand, allCand);
			currentDepth--;
//			if (designPoint.useUB) {
//				p.charact.bound.UbIC += icOfLastAdded;
//			}
			allowedCand.fastSet(vToAdd);
		}
	}

	private void backTrackPatternAndCand(Pattern p, OpenBitSet allowedCand, OpenBitSet allCand,
			ArrayList<Integer> removedNotAllowedCand, ArrayList<Integer> removedAllowedCand,
			ArrayList<Integer> addedToPattern, Characteristic saveCharact,
			ArrayList<Pair<Integer, Integer>> removedNi_v, ArrayList<Pair<Integer, Integer>> modifiedNi_v) {
		p.charact = saveCharact;
		for (int i = 0; i < addedToPattern.size(); i++) {
			allowedCand.fastSet(addedToPattern.get(i));
			allCand.fastSet(addedToPattern.get(i));
			p.vertexIndices.remove(p.vertexIndices.size() - 1);
		}
		for (int i : removedNotAllowedCand) {
			allCand.fastSet(i);
		}
		for (int i : removedAllowedCand) {
			allCand.fastSet(i);
			allowedCand.fastSet(i);
		}
		if (p.vertexIndices.size() == 0) {
			p.Ni_v.clear();
//			for (int i = 0; i < graph.vertices.length; i++) {
//				p.Ni_v.add(new Pair<Integer, Integer>(0, i));
//			}
			p.charact.restrictions.clear();
		} else {
			for (Pair<Integer, Integer> pair : modifiedNi_v) {
				for (Pair<Integer, Integer> otherPair : p.Ni_v) {
					if (pair.second == otherPair.second) {
						otherPair.first = pair.first;
						break;
					}
				}
			}
			p.Ni_v.addAll(removedNi_v);
		}
	}

	public int chooseCandidateMinimizingValidRestrictions(Pattern p, OpenBitSet allowedCand) {
		if (p.vertexIndices.size() == 0) {
			return allowedCand.nextSetBit(0);
		}
		int chosenCand = -1;
		int nbValidRestrictions = graph.descriptorMetaData.attributesName.length;
		BitSetIterator iterator = new BitSetIterator(allowedCand);
		int curIt;
		while ((curIt = iterator.getNext()) >= 0) {
			int curNbValid = 0;
			for (Triple<Integer, Double, Double> triple : p.charact.restrictions) {
				if (graph.vertices[curIt].pValues[triple.first] >= triple.second
						&& graph.vertices[curIt].pValues[triple.first] <= triple.third) {
					curNbValid++;
				}
			}
			if (curNbValid < nbValidRestrictions) {
				nbValidRestrictions = curNbValid;
				chosenCand = curIt;
			}
		}
		return chosenCand;
	}

	public int chooseOneFurthestCandidate(Pattern p, OpenBitSet allowedCand) {
		if (p.vertexIndices.size() == 0) {
			return allowedCand.nextSetBit(0);
		}
		double avgDistance = -1;
		int bestCand = -1;
		BitSetIterator iterator = new BitSetIterator(allowedCand);
		int curIt;
		while ((curIt = iterator.getNext()) >= 0) {
			double curDist = 0;
			boolean keepit = true;
			for (int v : p.vertexIndices) {
				if (graph.vDistances[v][curIt] == -1) {
					keepit = false;
					break;
				}
				curDist += graph.vDistances[v][curIt];
			}
			if (keepit) {
				curDist /= ((double) p.vertexIndices.size());
				if (avgDistance == -1 || avgDistance < curDist) {
					avgDistance = curDist;
					bestCand = curIt;
				}
			}
		}
		if (bestCand == -1) {
			return allowedCand.nextSetBit(0);
		}
		return bestCand;
	}

	public int chooseWorstCandidate(Pattern p, OpenBitSet allowedCand) {
		// applying fail first principle
		if (p.vertexIndices.size() == 0) {
			return allowedCand.nextSetBit(0);
		}
		int smallestDescNb = -1;
		int worstCand = -1;
		// if (designPoint.maxDistance >= 0) {
		// smallestDescNb = graph.vertices.length * (designPoint.maxDistance +
		// 1);
		// } else {
		// smallestDescNb = graph.vertices.length * (graph.biggestDistance + 1);
		// }
		// smallestDescNb += graph.descriptorMetaData.attributesName.length;
		BitSetIterator iterator = new BitSetIterator(allowedCand);
		int curIt;
		while ((curIt = iterator.getNext()) >= 0) {
			int curDescNb = 0;
			for (Triple<Integer, Double, Double> triple : p.charact.restrictions) {
				if (graph.vertices[curIt].pValues[triple.first] >= triple.second
						&& graph.vertices[curIt].pValues[triple.first] <= triple.third) {
					curDescNb++;
				} else {
					if ((triple.second == 0
							&& graph.vertices[curIt].pValues[triple.first] <= graph.descriptorMetaData.maxBelowLowThreshold[triple.first])
							|| (triple.third == 1
									&& graph.vertices[curIt].pValues[triple.first] >= graph.descriptorMetaData.minAboveHighThreshold[triple.first])) {
						curDescNb++;
					}
				}
			}
			if (designPoint.maxDistance >= 0) {
				for (Pair<Integer, Integer> pair : p.Ni_v) {
					if (graph.vDistances[curIt][pair.second] >= 0
							&& graph.vDistances[curIt][pair.second] <= designPoint.maxDistance) {
						curDescNb++;
					}
				}
			}
			if (smallestDescNb == -1 || curDescNb < smallestDescNb) {
				smallestDescNb = curDescNb;
				worstCand = curIt;
			}
		}
		return worstCand;

	}

}
