package interfaces;

import java.util.ArrayList;

import org.apache.lucene.util.OpenBitSet;

import model.Characteristic;
import model.EncapsInt;
import model.Pair;
import model.Pattern;
import utils.DesignPoint;

public interface IClosureManager {

	public boolean addAndclosePattern(Pattern p, int vToAdd, OpenBitSet allowedCand, OpenBitSet allCand,
			ArrayList<Integer> removedNotAllowedCand, ArrayList<Integer> removedAllowedCand,
			ArrayList<Integer> addedToPattern, Characteristic saveCharact,
			ArrayList<Pair<Integer, Integer>> removedNi_v, ArrayList<Pair<Integer, Integer>> modifiedNi_v,
			DesignPoint designPoint);

}
