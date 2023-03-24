package controllerandbuilder;

import java.util.ArrayList;
import java.util.Arrays;

import javax.swing.text.html.HTMLDocument.Iterator;

import org.apache.lucene.util.OpenBitSet;

import interfaces.IClosureManager;
import model.BitSetIterator;
import model.Characteristic;
import model.Graph;
import model.Pair;
import model.Pattern;
import model.Triple;
import utils.DesignPoint;
import utils.Statistics;

public class ClosureManager implements IClosureManager {
	// private double thresholdLow = 0.5;
	// private double thresholdHigh = 0.5;
	private Graph graph;
	private QualCalculator calculator;
	private DesignPoint designPoint;
	private Triple<Integer, Double, Double> intermRest = new Triple<Integer, Double, Double>(0, 0., 0.);
	private Statistics statistics;

	public ClosureManager(Graph graph, QualCalculator calculator, DesignPoint designPoint, Statistics statistics) {
		this.statistics = statistics;
		this.designPoint = designPoint;
		this.graph = graph;
		this.calculator = calculator;
		graph.descriptorMetaData.maxBelowLowThreshold = new double[graph.descriptorMetaData.attributesName.length];
		graph.descriptorMetaData.minAboveHighThreshold = new double[graph.descriptorMetaData.attributesName.length];
		for (int i = 0; i < graph.descriptorMetaData.attributesName.length; i++) {
			boolean maxBelowInit = false;
			boolean minAboveInit = false;
			for (int j = 0; j < graph.descriptorMetaData.binsPerAttribute[i].length; j++) {
				if (graph.descriptorMetaData.binsPerAttribute[i][j] > designPoint.thresholdHigh
						&& (graph.descriptorMetaData.minAboveHighThreshold[i] > graph.descriptorMetaData.binsPerAttribute[i][j]
								|| !minAboveInit)) {
					graph.descriptorMetaData.minAboveHighThreshold[i] = graph.descriptorMetaData.binsPerAttribute[i][j];
					minAboveInit = true;
				}
				if (graph.descriptorMetaData.binsPerAttribute[i][j] < designPoint.thresholdLow
						&& (graph.descriptorMetaData.maxBelowLowThreshold[i] < graph.descriptorMetaData.binsPerAttribute[i][j]
								|| !maxBelowInit)) {
					graph.descriptorMetaData.maxBelowLowThreshold[i] = graph.descriptorMetaData.binsPerAttribute[i][j];
					maxBelowInit = true;
				}
			}
		}
	}

	@Override
	public boolean addAndclosePattern(Pattern p, int vToAdd, OpenBitSet allowedCand, OpenBitSet allCand,
			ArrayList<Integer> removedNotAllowedCand, ArrayList<Integer> removedAllowedCand,
			ArrayList<Integer> addedToPattern, Characteristic saveCharact,
			ArrayList<Pair<Integer, Integer>> removedNi_v, ArrayList<Pair<Integer, Integer>> modifiedNi_v,
			DesignPoint designPoint) {
		if (removedNotAllowedCand == null || removedAllowedCand == null || addedToPattern == null || saveCharact == null
				|| removedNi_v == null || modifiedNi_v == null) {
			throw new RuntimeException("parameters must be initialized");
		}

		long startTime = System.currentTimeMillis();
		p.charact.copy(saveCharact);
		long stopTime = System.currentTimeMillis();
		statistics.charactCopyTime += (stopTime - startTime);

//		if (designPoint.useUB) {
//			p.charact.bound.UbIC = 0;
//		}
		// int initSize = p.vertexIndices.size();
		addedToPattern.add(vToAdd);
		p.vertexIndices.add(vToAdd);
		allowedCand.fastClear(vToAdd);
		allCand.fastClear(vToAdd);
		if (p.vertexIndices.size() == 1) {
			startTime = System.currentTimeMillis();
			initCharacteristic(p, vToAdd);
			stopTime = System.currentTimeMillis();
			statistics.initCharactTime += (stopTime - startTime);
			// calculator.calculateIC(p);
		} else {
			startTime = System.currentTimeMillis();
			updateCharacteristic(p, vToAdd);
			stopTime = System.currentTimeMillis();
			statistics.updateCharactTime += (stopTime - startTime);
		}
		if (p.charact.restrictions.size() == 0) {
			return false;
		}
		startTime = System.currentTimeMillis();
		int cpt = 0;

		if (p.vertexIndices.size() == 1) {
			for (int i = 0; i < graph.sortedVDistances[vToAdd].length; i++) {
				if (graph.sortedVDistances[vToAdd][i].first > designPoint.maxDistance) {
					break;
				}
				p.Ni_v.add(new Pair<Integer, Integer>(graph.sortedVDistances[vToAdd][i].first,
						graph.sortedVDistances[vToAdd][i].second));
			}
		} else {
			while (cpt < p.Ni_v.size()) {
				Pair<Integer, Integer> curPair = p.Ni_v.get(cpt);
				if (graph.vDistances[vToAdd][curPair.second] <= curPair.first
						&& graph.vDistances[vToAdd][curPair.second] >= 0) {
					cpt++;
				} else if (graph.vDistances[vToAdd][curPair.second] <= designPoint.maxDistance
						&& graph.vDistances[vToAdd][curPair.second] >= 0) {
					modifiedNi_v.add(new Pair<Integer, Integer>(curPair.first, curPair.second));
					curPair.first = graph.vDistances[vToAdd][curPair.second];
					cpt++;
				} else {
					removedNi_v.add(curPair);
					p.Ni_v.remove(cpt);
				}
			}
		}
		stopTime = System.currentTimeMillis();
		statistics.pruningNiTime += (stopTime - startTime);

		if (p.Ni_v.size() == 0) {
			return false;
		}

		BitSetIterator iterator = new BitSetIterator(allCand);
		int curIt = 0;
		while ((curIt = iterator.getNext()) >= 0) {
			startTime = System.currentTimeMillis();
			int ansChar = checkCandidateNeighb(p, curIt, true, vToAdd);
			stopTime = System.currentTimeMillis();
			statistics.checkCandNeighbTime += (stopTime - startTime);
			if (ansChar == -1) {
				if (allowedCand.fastGet(curIt)) {
					removedAllowedCand.add(curIt);
				} else {
					removedNotAllowedCand.add(curIt);
				}
				allCand.fastClear(curIt);
				allowedCand.fastClear(curIt);
			} else {
				startTime = System.currentTimeMillis();
				int ansNeighb = checkCandidateChar(p, curIt, ansChar == 1);
				stopTime = System.currentTimeMillis();
				statistics.checkCandCharTime += (stopTime - startTime);
				if (ansNeighb == -1) {
					if (allowedCand.fastGet(curIt)) {
						removedAllowedCand.add(curIt);
					} else {
						removedNotAllowedCand.add(curIt);
					}
					allCand.fastClear(curIt);
					allowedCand.fastClear(curIt);
				} else if (ansNeighb == 1 && ansChar == 1) {
					if (allowedCand.fastGet(curIt)) {
						addedToPattern.add(curIt);
						allowedCand.fastClear(curIt);
						allCand.fastClear(curIt);
						p.vertexIndices.add(curIt);
					} else {
						return false;
					}
				} else {
//					if (allowedCand.fastGet(curIt) && designPoint.useUB) {
//						p.charact.bound.UbIC += p.charact.bound.newFragUbIC;
//					}
				}
			}
		}
		calculator.updateICAfterAddVertices(p, addedToPattern.size());
		// double saveIC=p.charact.informationContent;
		// calculator.calculateIC(p);
		// if (saveIC!=p.charact.informationContent){
		// double dif=saveIC-p.charact.informationContent;
		// if (dif<0){
		// dif*=-1;
		// }
		// if (biggestError<dif){
		// biggestError=dif;
		// }
		// }
		// p.charact.informationContent=saveIC;
		return true;
	}

	public double biggestError = 0;

	private void initCharacteristic(Pattern p, int vToAdd) {
		p.charact.restrictions.clear();
		for (int i = 0; i < graph.descriptorMetaData.attributesName.length; i++) {
			double vVal = graph.vertices[vToAdd].pValues[i];
			double minVal;
			double maxVal;

			if (vVal <= graph.descriptorMetaData.maxBelowLowThreshold[i]) {
				minVal = 0;
				int index = 0;
				while (index < graph.descriptorMetaData.binsPerAttribute[i].length - 1
						&& graph.descriptorMetaData.binsPerAttribute[i][index] < vVal) {
					index++;
				}
				maxVal = graph.descriptorMetaData.binsPerAttribute[i][index];
				if (maxVal < designPoint.thresholdLow) {
					Triple<Integer, Double, Double> trp = new Triple<Integer, Double, Double>(i, minVal, maxVal);
					p.charact.restrictions.add(trp);
				}
			} else if (vVal >= graph.descriptorMetaData.minAboveHighThreshold[i]) {
				maxVal = 1;
				int index = graph.descriptorMetaData.binsPerAttribute[i].length - 1;
				while (index > 0 && graph.descriptorMetaData.binsPerAttribute[i][index] > vVal) {
					index--;
				}
				minVal = graph.descriptorMetaData.binsPerAttribute[i][index];
				if (minVal > designPoint.thresholdHigh) {
					Triple<Integer, Double, Double> trp = new Triple<Integer, Double, Double>(i, minVal, maxVal);
					p.charact.restrictions.add(trp);
				}
			}
		}
	}

	
	private int checkCandidateNeighb(Pattern p, int curIt, boolean toAdd, int newAddedV) {
		boolean toRemove = true;
		if (graph.vDistances[curIt][newAddedV] == -1) {
			return -1;
		}

		for (Pair<Integer, Integer> pair : p.Ni_v) {
			if (graph.vDistances[curIt][pair.second] >= 0
					&& graph.vDistances[curIt][pair.second] <= designPoint.maxDistance) {
				toRemove = false;
			} else {
				toAdd = false;
			}
			if (!toRemove && !toAdd) {
				break;
			}
		}
		if (toRemove) {
			return -1;
		}
		if (toAdd) {
			return 1;
		}
		return 0;
	}

	private int checkCandidateChar(Pattern p, int curIt, boolean toAdd) {
		boolean toRemove = true;
		// boolean toAdd = true;
//		if (designPoint.useUB) {
//			p.charact.bound.newFragUbIC = 0;
//		}
		for (Triple<Integer, Double, Double> rest : p.charact.restrictions) {
			if (graph.vertices[curIt].pValues[rest.first] >= rest.second
					&& graph.vertices[curIt].pValues[rest.first] <= rest.third) {
				toRemove = false;
//				if (designPoint.useUB) {
//					p.charact.bound.newFragUbIC += calculator.getMaxICByVertexAndChar(curIt, rest);
//				} else {
				if (!toAdd) {
					break;
				}
				// }
			} else {
				toAdd = false;
				// check if possible to extend this in the futur
				if ((rest.second == 0
						&& graph.vertices[curIt].pValues[rest.first] <= graph.descriptorMetaData.maxBelowLowThreshold[rest.first])
						|| (rest.third == 1
								&& graph.vertices[curIt].pValues[rest.first] >= graph.descriptorMetaData.minAboveHighThreshold[rest.first])) {
					toRemove = false;
//					if (designPoint.useUB) {
//						intermRest.first = rest.first;
//						intermRest.second = rest.second;
//						intermRest.third = rest.third;
//						updateRestriction(intermRest, graph.vertices[curIt].pValues[rest.first]);
//						p.charact.bound.newFragUbIC += calculator.getMaxICByVertexAndChar(curIt, intermRest);
//					}
				}
//				if (!designPoint.useUB && !toRemove) {
//					break;
//				}
			}
		}
		if (toRemove) {
			return -1;
		}
		if (toAdd) {
			return 1;
		}
		return 0;
	}

	public void updateCharacteristic(Pattern p, int addedV) {
		int cpt = 0;
		while (cpt < p.charact.restrictions.size()) {
			Triple<Integer, Double, Double> rest = p.charact.restrictions.get(cpt);
			if (graph.vertices[addedV].pValues[rest.first] >= rest.second
					&& graph.vertices[addedV].pValues[rest.first] <= rest.third) {
				cpt++;
			} else {
				calculator.updateICAfterRemoveRestriction(p, p.vertexIndices.size() - 1, rest);

				if (updateRestriction(rest, graph.vertices[addedV].pValues[rest.first])) {
					calculator.updateICAfterAddRestriction(p, p.vertexIndices.size() - 1, rest);
					cpt++;
				} else {
					p.charact.restrictions.remove(cpt);
				}
			}
		}
	}

	private boolean updateRestriction(Triple<Integer, Double, Double> rest, double d) {
		double newBound;
		if (rest.second == 0 && d <= graph.descriptorMetaData.maxBelowLowThreshold[rest.first]) {
			int index = Arrays.binarySearch(graph.descriptorMetaData.binsPerAttribute[rest.first], rest.third);
			while (index < graph.descriptorMetaData.binsPerAttribute[rest.first].length - 1
					&& graph.descriptorMetaData.binsPerAttribute[rest.first][index] < d) {
				index++;
			}
			newBound = graph.descriptorMetaData.binsPerAttribute[rest.first][index];
			if (newBound >= designPoint.thresholdLow) {
				return false;
			} else {
				rest.third = newBound;
			}
		} else if (rest.third == 1 && d >= graph.descriptorMetaData.minAboveHighThreshold[rest.first]) {
			int index = Arrays.binarySearch(graph.descriptorMetaData.binsPerAttribute[rest.first], rest.second);
			while (index > 0 && graph.descriptorMetaData.binsPerAttribute[rest.first][index] > d) {
				index--;
			}
			// rest.second =
			// graph.descriptorMetaData.binsPerAttribute[rest.first][index];
			newBound = graph.descriptorMetaData.binsPerAttribute[rest.first][index];
			if (newBound <= designPoint.thresholdHigh) {
				return false;
			} else {
				rest.second = newBound;
			}
		} else {
			return false;
		}
		return true;
	}

	public double getInitialUbIC(Pattern p) {
		double initialUbIc = 0;
		for (int i = 0; i < graph.vertices.length; i++) {
			initialUbIc += getMaxIcByVertex(i, p);
		}
		return initialUbIc;
	}

	public double getMaxIcByVertex(int vertex, Pattern p) {
		double maxIc = 0;
		if (p.vertexIndices.size() > 0) {
			for (Triple<Integer, Double, Double> triple : p.charact.restrictions) {
				if (graph.vertices[vertex].pValues[triple.first] >= triple.second
						&& graph.vertices[vertex].pValues[triple.first] <= triple.third) {
					maxIc += calculator.getMaxICByVertexAndChar(vertex, triple);
				} else {
					// check if possible to extend this in the futur
					if ((triple.second == 0
							&& graph.vertices[vertex].pValues[triple.first] <= graph.descriptorMetaData.maxBelowLowThreshold[triple.first])
							|| (triple.third == 1
									&& graph.vertices[vertex].pValues[triple.first] >= graph.descriptorMetaData.minAboveHighThreshold[triple.first])) {
						intermRest.first = triple.first;
						intermRest.second = triple.second;
						intermRest.third = triple.third;
						updateRestriction(intermRest, graph.vertices[vertex].pValues[triple.first]);
						maxIc += calculator.getMaxICByVertexAndChar(vertex, intermRest);
					}
				}
			}
		} else {
			for (int i = 0; i < graph.descriptorMetaData.attributesName.length; i++) {
				double vVal = graph.vertices[vertex].pValues[i];
				double minVal;
				double maxVal;
				if (vVal < designPoint.thresholdLow) {
					minVal = 0;
					int index = 0;
					while (index < graph.descriptorMetaData.binsPerAttribute[i].length - 1
							&& graph.descriptorMetaData.binsPerAttribute[i][index] < vVal) {
						index++;
					}
					maxVal = graph.descriptorMetaData.binsPerAttribute[i][index];
					if (maxVal < designPoint.thresholdLow) {
						intermRest.first = i;
						intermRest.second = minVal;
						intermRest.third = maxVal;
						maxIc += calculator.getMaxICByVertexAndChar(vertex, intermRest);
					}
				} else if (vVal > designPoint.thresholdHigh) {
					maxVal = 1;
					int index = graph.descriptorMetaData.binsPerAttribute[i].length - 1;
					while (index > 0 && graph.descriptorMetaData.binsPerAttribute[i][index] > vVal) {
						index--;
					}
					minVal = graph.descriptorMetaData.binsPerAttribute[i][index];
					if (minVal > designPoint.thresholdHigh) {
						intermRest.first = i;
						intermRest.second = minVal;
						intermRest.third = maxVal;
						maxIc += calculator.getMaxICByVertexAndChar(vertex, intermRest);
					}
				}
			}
		}
		return maxIc;
	}

	// public double getInitialUb(Pattern p, OpenBitSet allowedCand){

	// }

}
