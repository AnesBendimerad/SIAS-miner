package controllerandbuilder;

import java.util.ArrayList;
import java.util.Collections;

import model.Pattern;
import utils.DesignPoint;

public class JaccardSummarizer {

	public static ArrayList<Pattern> getSummary(ArrayList<Pattern> initialPatterns,DesignPoint designPoint){
		ArrayList<Pattern> summary=new ArrayList<>();
		for (Pattern p : initialPatterns){
			Collections.sort(p.vertexIndices);
		}
		
		int i=0;
		while (i<initialPatterns.size()){
			if (i%10000==0){
				System.out.println("summary, i="+i);
			}
			Pattern currentP=initialPatterns.get(i);
			boolean covered=false;
			for (int j=summary.size()-1;j>=0;j--){
				Pattern p =summary.get(j);
				if (isJaccardThresholdExceeded(currentP, p,designPoint)){
					covered=true;
					break;
				}
			}
			if (!covered){
				summary.add(currentP);
			}
			i++;
		}
		return summary;
	}

	public static boolean isJaccardThresholdExceeded(Pattern p1, Pattern p2,DesignPoint designPoint){
		double interSize=0;
		double sumSize=p1.vertexIndices.size()+p2.vertexIndices.size();
		Pattern tmp1,tmp2;
		double upperBound=0;
		if (p1.vertexIndices.size()<p2.vertexIndices.size()){
			tmp1=p1;
			tmp2=p2;
		}
		else {
			tmp1=p2;
			tmp2=p1;
		}
		int i=0;
		int j=0;
		while (i<tmp1.vertexIndices.size() && j<tmp2.vertexIndices.size()){
			while (j<tmp2.vertexIndices.size() && tmp1.vertexIndices.get(i)>tmp2.vertexIndices.get(j)){
				j++;
			}
			if (j<tmp2.vertexIndices.size() && tmp1.vertexIndices.get(i)==tmp2.vertexIndices.get(j)){
				interSize++;
				j++;
			}
			i++;
			if ((interSize/(sumSize-interSize))>=designPoint.jaccardValue){
				return true;
			}
			upperBound=interSize+tmp1.vertexIndices.size()-i;
			if ((upperBound/(sumSize-upperBound))<=designPoint.jaccardValue){
				return false;
			}
		}
		if ((interSize/(sumSize-interSize))>=designPoint.jaccardValue){
			return true;
		}
		else {
			return false;
		}
	}
	

}
