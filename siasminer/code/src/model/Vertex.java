package model;

import java.util.Arrays;
import java.util.HashSet;

import org.apache.lucene.util.OpenBitSet;

public class Vertex implements Comparable<Vertex>{
	public String id;
	public double[] attributes;
	public double[] probabilities;
	public double[] pValues;
	public double attSum=0;
	public int[] sortedNeighborsTab;
	public HashSet<Integer> neighborsSet;
	public int indexInGraph;
	public OpenBitSet kNeighborsBitSet;
	public int sizeOfGraph;
	public int nbNeighbors;
	
	public Vertex(String id, double[] attributesValues,int sizeOfGraph) {
		this.id = id;
		this.attributes = attributesValues;		
		this.sizeOfGraph=sizeOfGraph;
		neighborsSet = new HashSet<>();
		attSum=0;
		for (double attV : attributesValues){
			attSum+=attV;
		}
	}
	
	public void setupSortedNeighborsTab(){
		kNeighborsBitSet= new OpenBitSet(sizeOfGraph);
		sortedNeighborsTab=new int[neighborsSet.size()];
		int cpt=0;
		for (int nId : neighborsSet){
			sortedNeighborsTab[cpt++]=nId;
		}
		Arrays.sort(sortedNeighborsTab);
	}
	
	
	@Override
	public boolean equals(Object obj) {
		return id.equals(((Vertex) obj).id);
	}

	@Override
	public int hashCode() {
		return id.hashCode();
	}
	
	public void updateKNeighborsBitSet(int[][] vDistances,int maxDist){
		kNeighborsBitSet.clear(0,sizeOfGraph);
		for (int i=0;i<sizeOfGraph;i++){
			if (vDistances[indexInGraph][i]>=0 && vDistances[indexInGraph][i] <= maxDist) {
				kNeighborsBitSet.fastSet(i);
			}
		}
	}

	@Override
	public int compareTo(Vertex o) {
		if (nbNeighbors>o.nbNeighbors){
			return 1;
		}
		if (nbNeighbors<o.nbNeighbors){
			return -1;
		}
		return 0;
	}
	
}
