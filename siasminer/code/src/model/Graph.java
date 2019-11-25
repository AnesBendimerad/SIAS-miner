package model;

import java.util.Arrays;
import java.util.HashMap;

import org.apache.lucene.util.OpenBitSet;

public class Graph {
	public Vertex[] vertices;
	public DescriptorMetaData descriptorMetaData = null;
	public HashMap<String, Integer> indicesOfVertices;
	public int biggestDistance = 0;
	public int[][] vDistances = null;
	public int[] nbReachablePerVertex;
	public Pair<Integer, Integer>[][] sortedVDistances = null;
	private OpenBitSet intermVertices;
	private OpenBitSet nextVToAdd;
	private OpenBitSet alreadyAddedV;
	private int maxDistance;
	
	//private double avgDistance;

	public Graph(int numberOfVertices, int maxDistance) {
		vertices = new Vertex[numberOfVertices];
		indicesOfVertices = new HashMap<>();
		intermVertices = new OpenBitSet(numberOfVertices);
		nextVToAdd = new OpenBitSet(numberOfVertices);
		alreadyAddedV = new OpenBitSet(numberOfVertices);
		this.maxDistance = maxDistance;
	}

	public void setIndicesToVertices() {
		indicesOfVertices = new HashMap<>();
		for (int i = 0; i < vertices.length; i++) {
			indicesOfVertices.put(vertices[i].id, i);
			vertices[i].indexInGraph = i;
		}
	}

	public void calculateSumPerAttribute() {
		descriptorMetaData.sumPerAttribute = new double[descriptorMetaData.attributesName.length];
		for (int j = 0; j < descriptorMetaData.attributesName.length; j++) {
			for (int i = 0; i < vertices.length; i++) {
				descriptorMetaData.sumPerAttribute[j] += vertices[i].attributes[j];
			}
		}
	}

	public void calculateDistances() {
		System.out.println("calculating distances");
		biggestDistance = 0;
		//avgDistance=0;
		vDistances = new int[vertices.length][];
		nbReachablePerVertex = new int[vertices.length];
		for (int i = 0; i < vertices.length; i++) {
			calculateDistanceForOneV(i);
		}
		//avgDistance/=((double)(vertices.length-1)*(vertices.length));
		//System.out.println("avgDistance:"+avgDistance);
		System.out.println("sorting distances");
		// create sorted vDistances
		sortedVDistances = new Pair[vertices.length][];
		for (int i = 0; i < vertices.length; i++) {
			sortedVDistances[i] = new Pair[nbReachablePerVertex[i]];
			int cpt = 0;
			for (int j = 0; j < vertices.length; j++) {
				if (vDistances[i][j] >= 0) {
					sortedVDistances[i][cpt] = new Pair<Integer, Integer>(vDistances[i][j], j);
					cpt++;
				}
			}
			Arrays.sort(sortedVDistances[i]);
		}
		// System.out.println("biggest distance:"+biggestDistance);
	}

	private void calculateDistanceForOneV(int i) {
		nbReachablePerVertex[i] = 0;
		vDistances[i] = new int[vertices.length];
		for (int j = 0; j < vertices.length; j++) {
			vDistances[i][j] = -1;
			if (i == j) {
				vDistances[i][j] = 0;
				nbReachablePerVertex[i]++;
			}
		}
		intermVertices.clear(0, vertices.length);
		for (int nId : vertices[i].sortedNeighborsTab) {
			intermVertices.fastSet(nId);
		}
		nextVToAdd.clear(0, vertices.length);
		alreadyAddedV.clear(0, vertices.length);
		alreadyAddedV.fastSet(i);
		intermVertices.fastClear(i);
		int curDist = 1;
		while (intermVertices.cardinality() > 0 && (maxDistance == -1 || curDist <= 2*maxDistance)) {
			BitSetIterator iterator = new BitSetIterator(intermVertices);
			int curIt = -1;
			while ((curIt = iterator.getNext()) >= 0) {
				if (curDist > biggestDistance) {
					biggestDistance = curDist;
				}
				vDistances[i][curIt] = curDist;
				//avgDistance+=curDist;
				nbReachablePerVertex[i]++;
				alreadyAddedV.fastSet(curIt);
				for (int nId : vertices[curIt].sortedNeighborsTab) {
					nextVToAdd.fastSet(nId);
				}
			}
			nextVToAdd.andNot(alreadyAddedV);
			intermVertices.clear(0, vertices.length);
			intermVertices.or(nextVToAdd);
			nextVToAdd.clear(0, vertices.length);
			curDist++;
		}
	}
}
