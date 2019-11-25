package model;

public class Pair<A extends Comparable<A>,B> implements Comparable<Pair<A,B>>{
	public A first;
	public B second;
	
	
	public Pair(A first, B second) {
		this.first = first;
		this.second = second;
	}



	@Override
	public int compareTo(Pair<A, B> o) {
		return first.compareTo(o.first);
	}
	
	@Override
	protected Pair<A ,B> clone() throws CloneNotSupportedException {
		return new Pair<A, B>(first, second);
	}
	
	@Override
	public boolean equals(Object obj) {
		if (obj instanceof Pair){
			@SuppressWarnings("unchecked")
			Pair<A,B> pair=(Pair<A,B>)obj;
			if (first.equals(pair.first ) && second.equals(pair.second)){
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
	@Override
	public String toString() {
		return "("+first.toString()+","+second.toString()+")";
	}
	
	
}
