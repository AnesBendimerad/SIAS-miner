package controllerandbuilder;

import interfaces.IDescriptionLength;
import model.Graph;
import model.LogarithmicDescLength;
import utils.DesignPoint;

public class DescriptionLengthFunctionsBuilder {
	private DesignPoint designPoint;
	private Graph graph;
	public DescriptionLengthFunctionsBuilder(DesignPoint designPoint,Graph graph) {
		this.designPoint = designPoint;
		this.graph=graph;
	}
	
	public IDescriptionLength build(){
		IDescriptionLength descLengthFunction=null;
		switch (designPoint.descLengthType){
		case simple:
			throw new RuntimeException("this type has been deleted");
		case weighted:
			throw new RuntimeException("this type has been deleted");
		case logarithmic:
			descLengthFunction=new LogarithmicDescLength(designPoint,graph);
			break;
		}
		return descLengthFunction;
	}
	
}