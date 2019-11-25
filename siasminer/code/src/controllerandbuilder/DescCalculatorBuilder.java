package controllerandbuilder;

import interfaces.IDescriptionLength;
import model.ADescriptionCalculator;
import model.Graph;
import model.IntersectExhaustSimpleCalculator;
import model.IntersectGreedyDescCalculator;
import model.NoDescCalculator;
import utils.DesignPoint;
import utils.Statistics;

public class DescCalculatorBuilder {
	private Graph graph;
	private int nbVertices;
	private DesignPoint designPoint;
	IDescriptionLength descLengthFunction;
	private Statistics statistics;
	public DescCalculatorBuilder(Graph graph, DesignPoint designPoint,IDescriptionLength descLengthFunction,Statistics statistics) {
		this.graph = graph;
		this.nbVertices = graph.vertices.length;
		this.designPoint=designPoint;
		this.descLengthFunction=descLengthFunction;
		this.statistics=statistics;
	}


	public ADescriptionCalculator build(){
		ADescriptionCalculator descCalculator=null;
		
		switch (designPoint.dlApproach) {
		case greedyIntersection:
			//graph.calculateDistances();
			descCalculator=new IntersectGreedyDescCalculator(nbVertices, graph,designPoint.maxNbNeighborhoods,descLengthFunction,designPoint,statistics);
			break;
		case exhaustiveIntersection:
			//graph.calculateDistances();
			descCalculator=new IntersectExhaustSimpleCalculator(nbVertices, graph,designPoint.maxNbNeighborhoods,descLengthFunction,designPoint,statistics);
			break;
		case noDescription:
			descCalculator=new NoDescCalculator(designPoint.maxNbNeighborhoods,descLengthFunction,designPoint,graph,statistics);
			break;
		}
		return descCalculator;
		
	}

}
