package mainpackage;

import java.util.ArrayList;
import java.util.Collections;

import controllerandbuilder.DescCalculatorBuilder;
import controllerandbuilder.DescriptionLengthFunctionsBuilder;
import controllerandbuilder.GraphBuilder;
import controllerandbuilder.JaccardSummarizer;
import controllerandbuilder.LazySorterWithUpdate;
import controllerandbuilder.PatternComputer;
import controllerandbuilder.ResultsWriter;
import interfaces.IDescriptionLength;
import model.ADescriptionCalculator;
import model.Graph;
import model.Pattern;
import utils.DesignPoint;
import utils.Statistics;
import utils.Utilities;

public class MainClass {

	public static void main(String[] args) {
		int mb = 1024*1024;
		Runtime runtime = Runtime.getRuntime();
		DesignPoint designPoint=new DesignPoint();
		if (args.length != 1) {
			System.out.println("you can specify one parameter : parameters File path. Otherwise, default values will be considered");
		}
		else {
			Utilities.readParametersFromFile(designPoint, args[0]);
		}
		// TODO Auto-generated method stub
		Statistics statistics=new Statistics();
		System.out.println("building graph");
		long startTime = System.currentTimeMillis();
		Graph graph=new GraphBuilder(designPoint).build();
		IDescriptionLength descLengthFunction=new DescriptionLengthFunctionsBuilder(designPoint,graph).build();
		ADescriptionCalculator descCalculator=new DescCalculatorBuilder(graph, designPoint,descLengthFunction,statistics).build();
		long stopTime = System.currentTimeMillis();
		statistics.preProcessingTimeInMS = stopTime - startTime;
		//System.out.println(designPoint.bins[1]);
		PatternComputer computer=new PatternComputer(graph, designPoint,statistics,descCalculator);
		System.out.println("mining patterns");
		computer.retrievePatterns();
		System.out.println("fini :)");
		System.out.println("nbRecursiveCalls:"+statistics.nbRecursiveCalls);
		System.out.println("miningTimeInMS:"+statistics.miningTimeInMS);
		System.out.println("redescriptionTimeInMS:"+statistics.redescriptionTimeInMS);
		System.out.println("nbFoundPatterns:"+statistics.nbFoundPatterns);
		System.out.println("closureTimeInMS:"+statistics.closureTimeInMS);
		System.out.println("backtrackingTimeInMS:"+statistics.backtrackingTimeInMS);
		System.out.println("choosingCandTimeInMS:"+statistics.choosingCandTimeInMS);
		System.out.println("preProcessingTimeInMS:"+statistics.preProcessingTimeInMS);
		
		Collections.sort(computer.cAndPManager.foundPatterns, Collections.reverseOrder());
		ArrayList<Pattern> summary=null;
		ArrayList<Pattern> sortedWithUpdate=null;
		if (designPoint.summarize){
			summary=JaccardSummarizer.getSummary(computer.cAndPManager.foundPatterns, designPoint);
		}
		
		if (designPoint.applyUpdating) {
			startTime = System.currentTimeMillis();
			LazySorterWithUpdate sorter=new LazySorterWithUpdate(computer.cAndPManager.foundPatterns, designPoint, graph);
			sortedWithUpdate=sorter.computeTopKWithUpdateList();
			stopTime = System.currentTimeMillis();
			statistics.iterativeUpdatingTimeInMS = stopTime - startTime;
		}
		
		
		//System.out.println("nonRedundancy:"+CorrectVerifier.checkRedundancy(computer.foundPatterns));
		statistics.usedMemoryInMB=runtime.totalMemory() / mb;
		System.out.println("Total Memory:" + runtime.totalMemory() / mb);
		new ResultsWriter(designPoint, computer.cAndPManager.foundPatterns, graph, statistics).setSummary(summary).setSortedWithUpdate(sortedWithUpdate).write();
		//ResultsWriter writer=new ResultsWriter(designPoint, computer.cAndPManager.foundPatterns, graph, statistics).setSummary(summary);
		
		

	}

}
