package model;

import java.util.HashMap;

import utils.DesignPoint;

public class DescriptorMetaData {
	public String descriptorName;	
	public String[] attributesName;
	//public HashMap<String,Integer> attributesIndices=null;
	public double[] sumPerAttribute;
	public double[][] binsPerAttribute;
	public double[] maxBelowLowThreshold;
	public double[] minAboveHighThreshold;
	//public int[][] maxEntGroupsIndices;
	public int [] maxEntGroups;
	
	public DescriptorMetaData(String descriptorName, String[] attributesName) {
		this.descriptorName = descriptorName;
		this.attributesName = attributesName;
	}
//	public void makeAttributesIndices(){
//		attributesIndices=new HashMap<>();
//		for (int i=0;i<attributesName.length;i++){
//			attributesIndices.put(attributesName[i], i);
//		}
//	}
	public void makeMaxEntGroups(DesignPoint designPoint){
		if (!designPoint.multipleMaxEnt){
			maxEntGroups=new int[1];
			maxEntGroups[0]=attributesName.length;
		}
		else {
			maxEntGroups=designPoint.maxEntAttGroups;
		}
	}
//	public void makeMaxEntGroups(DesignPoint designPoint){
//		if (!designPoint.multipleMaxEnt){
//			maxEntGroupsIndices=new int[1][attributesName.length];
//			for (int i=0;i<attributesName.length;i++){
//				maxEntGroupsIndices[0][i]=i;
//			}
//		}
//		else {
////			if (attributesIndices==null){
////				makeAttributesIndices();
////			}
//			String[] groups=designPoint.maxEntAttGroups.split(";");
//			maxEntGroupsIndices=new int[groups.length][];
//			for (int i=0;i<groups.length;i++){
//				String[] gAttNames=groups[i].split(",");
//				maxEntGroupsIndices[i]=new int[gAttNames.length];
//				for (int j=0;j<gAttNames.length;j++){
//					maxEntGroupsIndices[i][j]=attributesIndices.get(gAttNames[j]);
//				}
//			}
//		}
//	}
	

}
