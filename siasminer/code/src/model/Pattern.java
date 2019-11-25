package model;

import java.util.ArrayList;

public class Pattern implements Comparable<Pattern> {
	public Characteristic charact ;
	public ArrayList<Integer> vertexIndices = new ArrayList<>();
	public ArrayList<Pair<Integer, Integer>> Ni_v = new ArrayList<>();// first
																		// i,
																		// second
																		// v
	public SubgraphDescription subgraphDescription = new SubgraphDescription();
	
	public double attributesDescLength;
	public double overalDescLength;
	
	public int updateId=0;
	
//	public Pattern(boolean useUB){
//		charact = new Characteristic(useUB);
//	}
	public Pattern(){
		charact = new Characteristic();
	}
	
	@Override
	public String toString() {
		String toRet = "Pattern:\n";
		toRet += "characteristic:" + charact.toString() + "\nsubgraph:";
		for (int i : vertexIndices) {
			toRet += String.valueOf(i) + ",";
		}
		toRet += "\nNi(v):";
		for (Pair<Integer, Integer> pair : Ni_v) {
			toRet += "(" + pair.first + "," + pair.second + "), ";
		}
		// toRet+="\n";
		return toRet;
	}

	@Override
	public Pattern clone() throws CloneNotSupportedException {
		//Pattern cloned = new Pattern(charact.bound!=null);
		Pattern cloned = new Pattern();
		cloned.charact = charact.clone();
		cloned.vertexIndices.addAll(vertexIndices);
		for (Pair<Integer, Integer> pair : Ni_v) {
			cloned.Ni_v.add(pair.clone());
		}
		if (subgraphDescription != null) {
			cloned.subgraphDescription = subgraphDescription.clone();
		}
		cloned.attributesDescLength=attributesDescLength;
		cloned.overalDescLength=overalDescLength;
		return cloned;
	}

	@Override
	public int compareTo(Pattern o) {
		if (charact.subjectiveInterestingness > o.charact.subjectiveInterestingness) {
			return 1;
		}
		if (charact.subjectiveInterestingness < o.charact.subjectiveInterestingness) {
			return -1;
		}
		return 0;
	}
	

}
