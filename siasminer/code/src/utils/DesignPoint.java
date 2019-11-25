package utils;

import enumerations.BinningType;
import enumerations.DLApproach;
import enumerations.DescriptionLengthType;

public class DesignPoint {
	public double thresholdLow = 0.5;
	public double thresholdHigh = 0.5;
	public int maxDistance=3;
	public int maxNbNeighborhoods=-1;
	public int topK=-1;
	public String resultFolderPath="results";
	public String resultsFilePath="results.json";
	public String summaryFilePath="summary.json";
	public String sortedWithUpdateFilePath="sortedWithUpdate.json";
	public String inputFilePath="FoursquareGraph100.json";
	public String statFilePath="statistics.txt";
	public boolean writeDetails=true;
	public boolean prettyPrinting=true;
	public BinningType binningType=BinningType.equalBins;
	public int minVertices=1;
	public DLApproach dlApproach=DLApproach.exhaustiveIntersection;
	public DescriptionLengthType descLengthType=DescriptionLengthType.logarithmic;
	//public boolean useUB=false;
	public double[] bins={0,0.2,0.4,0.6,0.8,1};
	public boolean multipleMaxEnt=false;
	public int[] maxEntAttGroups=null; // att1,att2,att3;att4,att5;att6,att7
	public boolean summarize=false;
	public double jaccardValue=0.5;
	public boolean applyUpdating=false;
	public int topKWithUpdating=10;
}
