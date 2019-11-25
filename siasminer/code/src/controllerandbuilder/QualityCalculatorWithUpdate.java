package controllerandbuilder;

import model.Graph;
import model.Pattern;
import model.Triple;
import utils.DesignPoint;
import utils.Utilities;

public class QualityCalculatorWithUpdate {

	private Graph graph;
	private DesignPoint designPoint;
	private double[][] priorLowerBounds;
	private double[][] priorUpperBounds;
	public int updateId = 0;
	private double log2Value=Math.log(2);

	public QualityCalculatorWithUpdate(Graph graph, DesignPoint designPoint) {
		this.graph = graph;
		this.designPoint = designPoint;
		priorLowerBounds = new double[graph.vertices.length][graph.descriptorMetaData.attributesName.length];
		priorUpperBounds = new double[graph.vertices.length][graph.descriptorMetaData.attributesName.length];
		for (int i = 0; i < graph.vertices.length; i++) {
			for (int j = 0; j < graph.descriptorMetaData.attributesName.length; j++) {
				priorLowerBounds[i][j] = 0;
				priorUpperBounds[i][j] = 1;
			}
		}
	}

	public void addPatternToModel(Pattern p) {
		for (int vI : p.vertexIndices) {
			for (Triple<Integer, Double, Double> charI : p.charact.restrictions) {
				priorLowerBounds[vI][charI.first] = (priorLowerBounds[vI][charI.first] >= charI.second)
						? priorLowerBounds[vI][charI.first]
						: charI.second;
				priorUpperBounds[vI][charI.first] = (priorUpperBounds[vI][charI.first] <= charI.third)
						? priorUpperBounds[vI][charI.first]
						: charI.third;
			}
		}
		updateId++;
	}

	public void updateICAndSIOfPattern(Pattern p) {
		p.charact.informationContent = 0;
		double tempD = 0;
		double tempLow = 0;
		double tempUp = 0;
		for (Triple<Integer, Double, Double> charI : p.charact.restrictions) {
			for (int vI : p.vertexIndices) {
				tempD= Utilities.probaFunction(priorUpperBounds[vI][charI.first], graph.vertices[vI].probabilities[charI.first], false)
						-Utilities.probaFunction(priorLowerBounds[vI][charI.first], graph.vertices[vI].probabilities[charI.first], true);
				tempLow = (priorLowerBounds[vI][charI.first] >= charI.second) ? priorLowerBounds[vI][charI.first]
						: charI.second; // we take the max
				tempUp = (priorUpperBounds[vI][charI.first] <= charI.third) ? priorUpperBounds[vI][charI.first]
						: charI.third; // we take the min
				tempD= (Utilities.probaFunction(tempUp, graph.vertices[vI].probabilities[charI.first], false)
						-Utilities.probaFunction(tempLow, graph.vertices[vI].probabilities[charI.first], true))/tempD;
				p.charact.informationContent-=log2(tempD);
			}
		}
		p.charact.subjectiveInterestingness=p.charact.informationContent/p.overalDescLength;
		p.updateId=updateId;
	}
	
	
	private double log2(double value){
		return Math.log(value)/log2Value;
	}

}
