package model;

import java.util.ArrayList;

public class Characteristic {
	public ArrayList<Triple<Integer,Double,Double>> restrictions;
	public double informationContent=0;
	public double subjectiveInterestingness=0;
//	public Bound bound=null;
	//public Characteristic(boolean useUB){
	public Characteristic(){
		restrictions=new ArrayList<>();
//		if (useUB){
//			bound=new Bound();
//		}
	}
	public Characteristic(ArrayList<Triple<Integer, Double, Double>> restrictions) {
		this.restrictions = restrictions;
	}

	public void copy(Characteristic copyIn){
		copyIn.restrictions=new ArrayList<>();
		for (Triple<Integer,Double,Double> trp : restrictions){
			copyIn.restrictions.add(new Triple<Integer, Double, Double>(trp.first, trp.second, trp.third));
		}
		copyIn.informationContent=informationContent;
//		if (bound!=null){
//			bound.copy(copyIn.bound);
//		}
	}
	
	
	@Override
	public String toString() {
		String toRet="";
		for (Triple<Integer,Double,Double> trp : restrictions){
			toRet+="("+trp.first+":["+trp.second+","+trp.third+"]), ";
		}
		return toRet;
	}
	
	@Override
	protected Characteristic clone() throws CloneNotSupportedException {
		//Characteristic cloned=new Characteristic(bound!=null);
		Characteristic cloned=new Characteristic();
		cloned.informationContent=informationContent;
		cloned.subjectiveInterestingness=subjectiveInterestingness;
		for (Triple<Integer,Double,Double> trp : restrictions){
			cloned.restrictions.add(trp.clone());
		}
//		if (bound!=null){
//			bound.copy(cloned.bound);
//		}
		return cloned;
	}
	
	
//	@Override
//	protected Characteristic clone() throws CloneNotSupportedException {
//		ArrayList<Triple<Integer,Double,Double>> newR=new ArrayList<>();
//		for (Triple<Integer,Double,Double> trp : restrictions){
//			newR.add(new Triple<Integer, Double, Double>(trp.first, trp.second, trp.third));
//		}
//		Characteristic cc=new Characteristic(newR);
//		return cc;
//	}
}
