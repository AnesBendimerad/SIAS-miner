package utils;

import java.util.ArrayList;

import model.Pattern;

public class CorrectVerifier {
	
	public static boolean checkRedundancy(ArrayList<Pattern> patterns){
		int cpt=0;
		for (Pattern p : patterns){
			cpt++;
			for (int i=cpt;i<patterns.size();i++){
				if (arePatEquals(p, patterns.get(i))){
					return false;
				}
			}
		}
		return true;
	}
	
	private static boolean arePatEquals(Pattern p1, Pattern p2){
		// we check vertices list
		if (p1.vertexIndices.size()!=p2.vertexIndices.size()){
			return false;
		}
		for (int v : p1.vertexIndices){
			boolean found=false;
			for (int v2 : p2.vertexIndices){
				if (v==v2){
					found=true;
					break;
				}
			}
			if (!found){
				return false;
			}
		}
		return true;
	}

}
