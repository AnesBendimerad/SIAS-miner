package model;

public class Triple<A,B,C> {
	public A first;
	public B second;
	public C third;
	public Triple(A first, B second, C third) {
		this.first = first;
		this.second = second;
		this.third = third;
	}
	
	@Override
	protected Triple<A,B,C> clone() throws CloneNotSupportedException {
		return new Triple<A, B, C>(first, second, third);
	}
	
	@Override
	public String toString() {
		return "("+first.toString()+":["+second.toString()+","+third.toString()+"])";
	}
}
