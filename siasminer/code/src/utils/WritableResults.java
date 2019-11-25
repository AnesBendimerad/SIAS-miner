package utils;

import java.util.ArrayList;

import model.Graph;
import model.Pattern;

public class WritableResults {
	public int numberOfPatterns;
	public WritablePattern[] patterns;
	public WritableResults(int numberOfPatterns, WritablePattern[] patterns) {
		this.numberOfPatterns = numberOfPatterns;
		this.patterns = patterns;
	}
	
	public static WritableResults createWritableResults(ArrayList<Pattern> retrievedPatterns,Graph graph,boolean writeDetails){
		WritablePattern[] patterns=new WritablePattern[retrievedPatterns.size()];
		for (int i=0;i<patterns.length;i++){
			patterns[i]=WritablePattern.createWritablePattern(retrievedPatterns.get(i), graph,writeDetails);
		}
		return new WritableResults(patterns.length,patterns);
	}
}
