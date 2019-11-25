package interfaces;

import java.util.ArrayList;

import org.apache.lucene.util.OpenBitSet;

import model.EncapsInt;
import model.Graph;
import model.SubgraphDescription;
import model.Pair;
import model.Pattern;

public interface IDescriptionLength {
	void pruneCandidates(ArrayList<Pair<Integer,Integer>> candDescriptions,OpenBitSet remainingErrors,Graph graph,Pattern p);
	int getBestCandidateGreedy( ArrayList<Pair<Integer,Integer>> candDescriptions,OpenBitSet remainingErrors,Graph graph);
	int getBestCandidateExhaustive( ArrayList<Pair<Integer,Integer>> candDescriptions,OpenBitSet remainingErrors,Graph graph,ArrayList<Integer> nbRemoved,EncapsInt minNbErrors);
	void updateDescriptionLength(SubgraphDescription description,Graph graph,Pattern p);
	double getCurrentDescriptionLength(SubgraphDescription description,Graph graph,Pattern p,OpenBitSet remainingErrors);
	double getLowerBound(SubgraphDescription desc,ArrayList<Pair<Integer,Integer>> candDescriptions,Pattern p,Graph graph,OpenBitSet remainingErrors);
	double getLowerBound2(SubgraphDescription desc,ArrayList<Pair<Integer,Integer>> candDescriptions,Pattern p,Graph graph,OpenBitSet remainingErrors,ArrayList<Integer> nbRemoved,EncapsInt minNbErrors);
	double getLowerBound3(Pattern p, OpenBitSet allowedCand);
	//double getMaxLength(Graph graph);
}
