package utils;

import model.Graph;
import model.Pair;

public class WritableCircle {
	public int radius;
	public String center;
	//public String[] contents;

//	public WritableCircle(int radius, String center, String[] contents) {
//		this.radius = radius;
//		this.center = center;
//		//this.contents = contents;
//	}
	public WritableCircle(int radius, String center) {
		this.radius = radius;
		this.center = center;
	}

	public static WritableCircle createWritableCircle(Pair<Integer,Integer> pair, Graph graph) {
		int radius = pair.first;
		String center = graph.vertices[pair.second].id;
//		ArrayList<String> contentsList = new ArrayList<>();
//		for (int i = 0; i < graph.vertices.length; i++) {
//			if (graph.vDistances[pair.second][i] <= pair.first) {
//				contentsList.add(graph.vertices[i].id);
//			}
//		}
//		String[] contents = new String[contentsList.size()];
//		for (int i = 0; i < contentsList.size(); i++) {
//			contents[i] = contentsList.get(i);
//		}
		//return new WritableCircle(radius, center, contents);
		return new WritableCircle(radius, center);
	}
}
