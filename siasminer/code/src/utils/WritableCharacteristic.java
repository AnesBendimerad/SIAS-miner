package utils;

import java.util.ArrayList;

import model.Characteristic;
import model.Graph;
import model.Triple;

public class WritableCharacteristic {
	public ArrayList<String> positiveAttributes=new ArrayList<>();
	public ArrayList<String> negativeAttributes=new ArrayList<>();
	public double informationContent;
	public double overalDescLength;
	public double attributesDescLength;
	public double score=0;
	public WritableCharacteristic(ArrayList<String> positiveAttributes, ArrayList<String> negativeAttributes,
			double informationContent,double subjectiveInterestingness) {
		this.positiveAttributes = positiveAttributes;
		this.negativeAttributes = negativeAttributes;
		this.informationContent = informationContent;
		this.score=subjectiveInterestingness;
	}
	
	public static WritableCharacteristic createWritableCharacteristic(Graph graph, Characteristic charact){
		 ArrayList<String> positiveAttributes=new ArrayList<>();
		 ArrayList<String> negativeAttributes=new ArrayList<>();
		for (Triple<Integer,Double,Double> triple : charact.restrictions){
			if (triple.second==0){
				String score = String.valueOf(triple.third*100);
				String comp=graph.descriptorMetaData.attributesName[triple.first]+"<="+score+"%";
				positiveAttributes.add(comp);
			}
			else {
				String score = String.valueOf(triple.second*100);
				String comp=graph.descriptorMetaData.attributesName[triple.first]+">="+score+"%";
				negativeAttributes.add(comp);
			}
		}
		return new WritableCharacteristic(positiveAttributes, negativeAttributes, charact.informationContent,charact.subjectiveInterestingness);
	}
	
}
