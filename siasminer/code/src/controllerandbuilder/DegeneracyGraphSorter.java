package controllerandbuilder;


import java.util.HashSet;

import org.apache.lucene.util.OpenBitSet;

import model.BitSetIterator;
import model.Graph;
import model.Pair;
import model.Vertex;

public class DegeneracyGraphSorter {
	private Graph graph;
	//OpenBitSet sortedVertices;
	OpenBitSet remainingVertices;
	public DegeneracyGraphSorter(Graph graph) {
		this.graph = graph;
		//sortedVertices=new OpenBitSet(graph.vertices.length);
		remainingVertices=new OpenBitSet(graph.vertices.length);
	}
	
	public void sort(){
		//sortedVertices.clear(0,graph.vertices.length);
		remainingVertices.set(0,graph.vertices.length);
		BitSetIterator iterator=new BitSetIterator(remainingVertices);
		int[] newToOldIndices=new int[graph.vertices.length]; 
		int[] oldToNewIndices=new int[graph.vertices.length];
		int sortedNumber=0;
		int curIt;
		while (sortedNumber<graph.vertices.length){
			int bestCand=-1;
			int degeneracy=-1;
			iterator.reinitIterator();
			while ((curIt=iterator.getNext())>=0){
				int curDegen=getCurrentRemainingNeighbors(curIt);
				if (degeneracy==-1 || degeneracy>curDegen){
					degeneracy=curDegen;
					bestCand=curIt;
				}
			}
			newToOldIndices[sortedNumber]=bestCand;
			oldToNewIndices[bestCand]=sortedNumber;
			remainingVertices.fastClear(bestCand);
			sortedNumber++;
		}
		Vertex[] vertices=new Vertex[graph.vertices.length];
		for (int i=0;i<graph.vertices.length;i++){
			vertices[i]=graph.vertices[newToOldIndices[i]];
		}
		graph.vertices=vertices;
		for (Vertex v : graph.vertices){
			HashSet<Integer> neighbors=new HashSet<>();
			for (int old : v.neighborsSet){
				neighbors.add(oldToNewIndices[old]);
			}
			v.neighborsSet=neighbors;
		}
		graph.setIndicesToVertices();
	}
	
	private int getCurrentRemainingNeighbors(int curIt){
		int nb=0;
		for (int i : graph.vertices[curIt].neighborsSet){
			if (remainingVertices.fastGet(i)){
				nb++;
			}
		}
		return nb;
	}
}
