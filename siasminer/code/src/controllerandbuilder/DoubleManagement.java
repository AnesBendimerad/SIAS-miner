package controllerandbuilder;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;

import model.Graph;

public class DoubleManagement {
	
	
	public static void calculateEqualBins(Graph graph,double[] steps){
		//double[] steps=new double[((int)(1/oneStep))+1];
		//for (int i=0;i<steps.length;i++){
		//			steps[i]=oneStep*i;
		//}
		graph.descriptorMetaData.binsPerAttribute=new double[graph.descriptorMetaData.attributesName.length][];
		for (int i=0;i<graph.descriptorMetaData.attributesName.length;i++){
			graph.descriptorMetaData.binsPerAttribute[i]=steps;
		}
	}
	
	
	public static void calculatePercentileBins(Graph graph,double[] percentiles){
		//double[] percentiles=new double[((int)(1/percentileStep))+1];
		//for (int i=0;i<percentiles.length;i++){
		//percentiles[i]=percentileStep*i;
		//}
		graph.descriptorMetaData.binsPerAttribute=new double[graph.descriptorMetaData.attributesName.length][];
		for (int i=0;i<graph.descriptorMetaData.attributesName.length;i++){
			double[] valTable=new double[graph.vertices.length];
			for (int j=0;j<graph.vertices.length;j++){
				valTable[j]=graph.vertices[j].pValues[i];
			}
			graph.descriptorMetaData.binsPerAttribute[i]=getPercentilesOfTable(valTable,percentiles);
		}
	}
	public static void useBinsOfPNRForSpecificCase(Graph graph,double steps){
		// we have 10 attributes: 0- Outdoors & Recreation, 1- Shop & Service, 2- College & University, 3- Travel & Transport
		// 4- Event, 5- Nightlife Spot, 6- Residence, 7- Arts & Entertainment, 8- Professional & Other Places
		// 9- Food
		graph.descriptorMetaData.binsPerAttribute=new double[graph.descriptorMetaData.attributesName.length][];
		String repo="one_to_one/";
		String[] fileNames={"outdoors.txt","shop.txt","college.txt","travel.txt","event.txt","nightlife.txt","residence.txt","arts.txt","professional.txt","food.txt"};
		for (int i=0;i<fileNames.length;i++){
			graph.descriptorMetaData.binsPerAttribute[i]=getBinsFromPnrFile(repo+fileNames[i]);
		}
	}
	
	
	private static double[] getBinsFromPnrFile(String filePath) {
		try {
			BufferedReader reader=new BufferedReader(new FileReader(filePath));
			String line;
			reader.readLine();
			ArrayList<Double> values=new ArrayList<>();
			while ((line=reader.readLine())!=null){
				String[] elem=line.split(",");
				if (elem[1].contains("[") || elem[1].charAt(0)=='-'){
					break;
				}
				elem[1]=elem[1].replace("\n", "").replace("\r", "");
				values.add(Double.parseDouble(elem[1]));
			}
			double[] tab=new double[values.size()];
			for (int i=0;i<values.size();i++){
				tab[i]=values.get(i);
			}
			return tab;
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// TODO Auto-generated method stub
		return null;
	}


	private static double[] getPercentilesOfTable(double[] table,double[] percentiles){
		Arrays.sort(table);
		double[] result=new double[percentiles.length];
		for (int i=0;i<percentiles.length;i++){
			int index=(int)Math.round(((double)table.length)*percentiles[i]) ;
			if (index==0){
				result[i]=table[index];
			}
			else if (index==table.length){
				result[i]=table[index-1];
			}
			else {
				result[i]=(table[index-1]+table[index])/2;
			}
		}
		return result;
	}

}
