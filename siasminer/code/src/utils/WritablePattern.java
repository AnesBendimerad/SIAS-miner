package utils;

import model.Graph;
import model.Pattern;

public class WritablePattern {
	public String[] subgraph;
	public WritableCharacteristic characteristic;
	public WritableRedescription subgraphRedescription;
	
	
	public WritablePattern(String[] subgraph, WritableCharacteristic characteristic, WritableRedescription redescription) {
		this.subgraph = subgraph;
		this.characteristic = characteristic;
		this.subgraphRedescription = redescription;
	}



	public static WritablePattern createWritablePattern(Pattern p,Graph graph,boolean writeDetails){
		String[] subgraph=new String[p.vertexIndices.size()];
		int cpt=0;
		for (int v : p.vertexIndices){
			subgraph[cpt]=graph.vertices[v].id;
			cpt++;
		}
		
		WritableRedescription redes=WritableRedescription.createWritableRedescription(p.subgraphDescription, graph,writeDetails);
		WritableCharacteristic wChar=WritableCharacteristic.createWritableCharacteristic(graph, p.charact);
		wChar.attributesDescLength=p.attributesDescLength;
		wChar.overalDescLength=p.overalDescLength;
		WritablePattern wp=new WritablePattern(subgraph, wChar, redes);
		return wp;
	}
}








