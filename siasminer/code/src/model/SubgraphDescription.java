package model;

import java.util.ArrayList;

public class SubgraphDescription {
	public ArrayList<Pair<Integer,Integer>> description=new ArrayList<>();
	public ArrayList<Integer> XMinusP=new ArrayList<>();
	public int nbExceptions;
	public double subgraphDescLength=-1;
	
	
	
	
	
	@Override
	protected SubgraphDescription clone() throws CloneNotSupportedException {
		SubgraphDescription cloned=new SubgraphDescription();
		for (Pair<Integer,Integer> pair : description){
			cloned.description.add(pair.clone());
		}
		cloned.XMinusP.addAll(XMinusP);
		cloned.nbExceptions=nbExceptions;
		cloned.subgraphDescLength=subgraphDescLength;
		return cloned;
	}
}
