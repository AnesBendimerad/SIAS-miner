package model;

import java.util.Arrays;
import java.util.HashSet;

public class IndexedUniques {
	public double[] uniqueValues;
	public int[] indices;
	public int[] nbRepetitions; // vcols and vrows
	
	public IndexedUniques(double[] values) {
		HashSet<Double> uniqueSet=new HashSet<>();
		for (double d : values){
			uniqueSet.add(d);
		}
		uniqueValues=new double[uniqueSet.size()];
		nbRepetitions=new int[uniqueSet.size()];
		int cpt=0;
		for (double d : uniqueSet){
			uniqueValues[cpt++]=d;
		}
		Arrays.sort(uniqueValues);
		indices=new int[values.length];
		for (int i=0;i<values.length;i++){
			indices[i]=Arrays.binarySearch(uniqueValues,values[i]);
			nbRepetitions[indices[i]]++;
		}
	}
}
