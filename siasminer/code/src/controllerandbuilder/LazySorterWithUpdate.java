package controllerandbuilder;

import java.util.ArrayList;
import java.util.Collections;

import model.Graph;
import model.Pattern;
import utils.DesignPoint;

public class LazySorterWithUpdate {
	public ArrayList<Pattern> allPatterns;
	public DesignPoint designPoint;
	public ArrayList<Pattern> topKWithUpdateList;
	public Graph graph;
	public LazySorterWithUpdate( ArrayList<Pattern> allPatterns,DesignPoint designPoint,Graph graph) {
		this.allPatterns=new ArrayList<>(allPatterns);
		this.designPoint = designPoint;
		this.graph=graph;
	}
	
	public ArrayList<Pattern> computeTopKWithUpdateList() {
		
		topKWithUpdateList=new ArrayList<>();
		QualityCalculatorWithUpdate calculator=new QualityCalculatorWithUpdate(graph, designPoint);
		while (topKWithUpdateList.size()<designPoint.topKWithUpdating && allPatterns.size()>0) {
			int i=getBest(calculator);
			Pattern p=allPatterns.get(i);
			allPatterns.remove(i);
			topKWithUpdateList.add(p);
			calculator.addPatternToModel(p);
		}
		return topKWithUpdateList;
	}

	private int getBest(QualityCalculatorWithUpdate calculator) {
		int i=0;
		while (true) {
			Pattern p=allPatterns.get(i);
			if (p.updateId==calculator.updateId) {
				break;
			}
			else {
				allPatterns.remove(i);
				calculator.updateICAndSIOfPattern(p);
				int index=Collections.binarySearch(allPatterns,p,Collections.reverseOrder());
				if (index<0) {
					index*=-1;
					index--;
				}
				allPatterns.add(index, p);
			}
		}
		return i;
	}

}






