package model;

import org.apache.lucene.util.OpenBitSet;

public class BitSetIterator {
	private OpenBitSet bitSet;
	private int i;
	public BitSetIterator(OpenBitSet bitSet) {
		this.bitSet = bitSet;
		i=0;
	}
	public void reinitIterator(){
		i=0;
	}
	
	public int getNext(){
		int k=bitSet.nextSetBit(i);
		i=k+1;
		return k;
	}

}
