package controllerandbuilder;

import model.Graph;
import model.Pattern;
import model.Triple;
import utils.DesignPoint;
import utils.Utilities;

public class QualCalculator {
	private Graph graph;
	private DesignPoint designPoint;
	private double log2Value = Math.log(2);
	// private Triple<Integer,Double,Double> intermRest=new Triple<Integer,
	// Double, Double>(0, 0., 0.);

	public QualCalculator(Graph graph, DesignPoint designPoint) {
		this.graph = graph;
		this.designPoint = designPoint;
	}

	public void calculateIC(Pattern pattern) {
		pattern.charact.informationContent = 0;
		for (Triple<Integer, Double, Double> triple : pattern.charact.restrictions) {
			for (int v : pattern.vertexIndices) {
				pattern.charact.informationContent -= log2(Utilities.probaFunction(triple.third,
						graph.vertices[v].probabilities[triple.first], false)
						- Utilities.probaFunction(triple.second, graph.vertices[v].probabilities[triple.first], true));
			}
		}
	}

	public void updateICAfterAddRestriction(Pattern pattern, int nbFirstConsideredV,
			Triple<Integer, Double, Double> restriction) {
		int cpt = 0;
		for (int v : pattern.vertexIndices) {
			if (cpt >= nbFirstConsideredV) {
				break;
			}
			cpt++;
			pattern.charact.informationContent -= log2(Utilities.probaFunction(restriction.third,
					graph.vertices[v].probabilities[restriction.first], false)
					- Utilities.probaFunction(restriction.second, graph.vertices[v].probabilities[restriction.first],
							true));
		}
	}

	public void updateICAfterRemoveRestriction(Pattern pattern, int nbFirstConsideredV,
			Triple<Integer, Double, Double> restriction) {
		int cpt = 0;
		for (int v : pattern.vertexIndices) {
			if (cpt >= nbFirstConsideredV) {
				break;
			}
			cpt++;
			pattern.charact.informationContent += log2(Utilities.probaFunction(restriction.third,
					graph.vertices[v].probabilities[restriction.first], false)
					- Utilities.probaFunction(restriction.second, graph.vertices[v].probabilities[restriction.first],
							true));
		}
	}

	public void updateICAfterAddVertices(Pattern pattern, int nbAddedVertices) {
		double quantity = 0;
		for (Triple<Integer, Double, Double> triple : pattern.charact.restrictions) {
			for (int i = pattern.vertexIndices.size() - nbAddedVertices; i < pattern.vertexIndices.size(); i++) {
				int v = pattern.vertexIndices.get(i);
				quantity = log2(Utilities.probaFunction(triple.third, graph.vertices[v].probabilities[triple.first],
						false)
						- Utilities.probaFunction(triple.second, graph.vertices[v].probabilities[triple.first], true));
				pattern.charact.informationContent -= quantity;
			}
		}
	}

	public double getMaxICByVertexAndChar(int vertex, Triple<Integer, Double, Double> restriction) {
		return -log2(Utilities.probaFunction(restriction.third, graph.vertices[vertex].probabilities[restriction.first],
				false)
				- Utilities.probaFunction(restriction.second, graph.vertices[vertex].probabilities[restriction.first],
						true));
	}

	private double log2(double value) {
		return Math.log(value) / log2Value;
	}

}
