package model;

import java.util.ArrayList;

import org.apache.lucene.util.OpenBitSet;

import interfaces.IDescriptionLength;
import utils.DesignPoint;
import utils.Statistics;

public class NoDescCalculator extends ADescriptionCalculator{
	private OpenBitSet intermBitSet;
	
	public NoDescCalculator(int maxNbNeighborhoods,IDescriptionLength descLengthFunction,DesignPoint designPoint,Graph graph,Statistics statistics) {
		super(maxNbNeighborhoods,descLengthFunction,designPoint,graph,statistics);
		intermBitSet=new OpenBitSet(graph.vertices.length);
	}
	@Override
	public void calculateDescription(Pattern p) {
		p.subgraphDescription=new SubgraphDescription();
		p.subgraphDescription.description=new ArrayList<>(p.Ni_v);
		
		intermBitSet.set(0, graph.vertices.length);
		for (Pair<Integer,Integer> pair : p.Ni_v){
			BitSetIterator iterator=new BitSetIterator(intermBitSet);
			int curIt=-1;
			while ((curIt=iterator.getNext())>=0){
				if (graph.vDistances[pair.second][curIt]<0 || graph.vDistances[pair.second][curIt]>designPoint.maxDistance){
					intermBitSet.fastClear(curIt);
				}
			}
		}
		for (int i : p.vertexIndices){
			intermBitSet.fastClear(i);
		}
		p.subgraphDescription.nbExceptions=(int)intermBitSet.cardinality();
		//System.out.println("p.optimisedDescription.nbExceptions:"+p.optimisedDescription.nbExceptions);
		p.subgraphDescription.XMinusP=new ArrayList<>();
		//if (designPoint.writeDetails){
			BitSetIterator iterator=new BitSetIterator(intermBitSet);
			int curIt=-1;
			while ((curIt>=iterator.getNext())){
				p.subgraphDescription.XMinusP.add(curIt);
			}
		//}
		p.subgraphDescription.subgraphDescLength=descriptionLengthFunction.getCurrentDescriptionLength(p.subgraphDescription, graph, p, intermBitSet);
		calculateAttributesDescription(p);
	}
}
