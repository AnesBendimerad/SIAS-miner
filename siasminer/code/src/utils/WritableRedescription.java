package utils;

import model.Graph;
import model.SubgraphDescription;

public class WritableRedescription {
	public double descLength;
	public int nbExceptions;
	public int nbCircles;
	public WritableCircle[] circles;
	public String[] XMinusP;
	public String[] PMinusX;

	public WritableRedescription(WritableCircle[] circles, String[] xMinusP, double descLength, int nbCircles,
			int nbExceptions) {
		this.circles = circles;
		this.XMinusP = xMinusP;
		String descAsString = String.format("%.2f", descLength).replace(",", ".");
		this.descLength = Double.parseDouble(descAsString);
		this.nbCircles = nbCircles;
		this.nbExceptions = nbExceptions;
		this.PMinusX=new String[0];
	}

	public static WritableRedescription createWritableRedescription(SubgraphDescription desc, Graph graph,
			boolean writeDetails) {
		int nbExceptions = desc.nbExceptions;
		int nbCircles = desc.description.size();
		WritableCircle[] circles = new WritableCircle[desc.description.size()];
		for (int i = 0; i < desc.description.size(); i++) {
			circles[i] = WritableCircle.createWritableCircle(desc.description.get(i), graph);
		}
		String[] XMinusP;
		if (desc.XMinusP == null) {
			XMinusP = new String[0];
		} else {
			if (writeDetails) {
				XMinusP = new String[(int) desc.XMinusP.size()];
				for (int i = 0; i < desc.XMinusP.size(); i++) {
					XMinusP[i] = graph.vertices[desc.XMinusP.get(i)].id;
				}
			}
			else {
				XMinusP=new String[1];
				XMinusP[0]="not shown";
			}
		}
		return new WritableRedescription(circles, XMinusP, desc.subgraphDescLength, nbCircles, nbExceptions);
	}
}
