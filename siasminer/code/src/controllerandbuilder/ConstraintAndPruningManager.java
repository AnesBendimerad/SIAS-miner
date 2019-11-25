package controllerandbuilder;

import java.util.ArrayList;
import java.util.Collections;

import javax.management.RuntimeErrorException;

import model.Graph;
import model.Pattern;
import utils.DesignPoint;

public class ConstraintAndPruningManager {
	private Graph graph;
	private DesignPoint designPoint;
	public ArrayList<Pattern> foundPatterns;
	public ConstraintAndPruningManager(Graph graph,DesignPoint designPoint){
		this.graph=graph;
		this.designPoint=designPoint;
	}
	
	public boolean checkConstraintsBeforeDL(Pattern p){
		return true;
	}
	public boolean checkConstraintsAfterDL(Pattern p){
		if (designPoint.topK==-1 || foundPatterns.size()<designPoint.topK){
			return true;
		}
		if (designPoint.topK==0){
			// this is somehow stupid hah right? but ok we'll return the top 0 patterns :p
			return false;
		}
		if (foundPatterns.get(foundPatterns.size()-1).charact.subjectiveInterestingness<p.charact.subjectiveInterestingness){
			return true;
		}
		return false;
	}
	
	public void insertWithOrder(Pattern p){
		int index = Collections.binarySearch(foundPatterns, p, Collections.reverseOrder());
		if (index<0){
			index*=-1;
			index--;
		}
		try {
			foundPatterns.add(index, p.clone());
		} catch (CloneNotSupportedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (designPoint.topK!=-1 && foundPatterns.size()-designPoint.topK>1){
			throw new RuntimeException("this is not possible in normal conditions");
		}
		if (designPoint.topK!=-1 && foundPatterns.size()>designPoint.topK){
			foundPatterns.remove(foundPatterns.size()-1);
		}
		
	}
	//public boolean pruneWithUB(double ub){
	//	if (designPoint.topK==-1 || foundPatterns.size()<designPoint.topK){
	//			return false;
	//	}
	//		else {
	//			if (foundPatterns.get(foundPatterns.size()-1).charact.subjectiveInterestingness>=ub){
	//				return true;
	//			}
	//		}
	//		return false;
	//}
}
