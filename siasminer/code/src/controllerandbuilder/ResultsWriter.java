package controllerandbuilder;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.reflect.Field;
import java.util.ArrayList;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import model.Graph;
import model.Pattern;
import utils.DesignPoint;
import utils.Statistics;
import utils.Utilities;
import utils.WritableResults;

public class ResultsWriter {
	private DesignPoint designPoint;
	private ArrayList<Pattern> retrievedPatterns;
	private ArrayList<Pattern> summary = null;
	private ArrayList<Pattern> sortedWithUpdate = null;
	private Graph graph;
	private Statistics statistics;

	public ResultsWriter(DesignPoint designPoint, ArrayList<Pattern> retrievedPatterns, Graph graph,
			Statistics statistics) {
		this.retrievedPatterns = retrievedPatterns;
		this.graph = graph;
		this.designPoint = designPoint;
		this.statistics = statistics;
	}

	public ResultsWriter setSummary(ArrayList<Pattern> summary) {
		this.summary = summary;
		return this;
	}

	public ResultsWriter setSortedWithUpdate(ArrayList<Pattern> sortedWithUpdate) {
		this.sortedWithUpdate = sortedWithUpdate;
		return this;
	}

	public void write() {
		Utilities.createFolder(designPoint.resultFolderPath, false);
		long startTime = System.currentTimeMillis();
		WritableResults writableResults = WritableResults.createWritableResults(retrievedPatterns, graph,
				designPoint.writeDetails);
		Gson gson;
		if (designPoint.prettyPrinting) {
			gson = new GsonBuilder().setPrettyPrinting().create();
		} else {
			gson = new GsonBuilder().create();
		}
		if (!designPoint.applyUpdating) {
			try (FileWriter writer = new FileWriter(
					designPoint.resultFolderPath + Utilities.getFileSeparator() + designPoint.resultsFilePath)) {
				gson.toJson(writableResults, writer);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		// write summary
		if (designPoint.summarize) {
			WritableResults writableResults2 = WritableResults.createWritableResults(summary, graph,
					designPoint.writeDetails);
			try (FileWriter writer = new FileWriter(
					designPoint.resultFolderPath + Utilities.getFileSeparator() + designPoint.summaryFilePath)) {
				gson.toJson(writableResults2, writer);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if (designPoint.applyUpdating) {
			WritableResults writableResults3 = WritableResults.createWritableResults(sortedWithUpdate, graph,
					designPoint.writeDetails);
			try (FileWriter writer = new FileWriter(designPoint.resultFolderPath + Utilities.getFileSeparator()
					+ designPoint.sortedWithUpdateFilePath)) {
				gson.toJson(writableResults3, writer);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		long stopTime = System.currentTimeMillis();
		statistics.writingTimeInMS = stopTime - startTime;
		writeStatistics();

	}

	private void writeStatistics() {
		try {
			BufferedWriter resultFile = new BufferedWriter(new FileWriter(
					designPoint.resultFolderPath + Utilities.getFileSeparator() + designPoint.statFilePath));
			Class<?> clazz = Statistics.class;
			for (Field field : clazz.getDeclaredFields()) {
				writeLine(field, statistics, resultFile);
			}
			resultFile.write("\n");
			clazz = DesignPoint.class;
			for (Field field : clazz.getDeclaredFields()) {
				writeLine(field, designPoint, resultFile);
			}

			resultFile.close();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		}

	}

	private void writeLine(Field field, Object objectToWrite, BufferedWriter writer)
			throws IllegalArgumentException, IllegalAccessException, IOException {
		writer.write(field.getName() + ":" + field.get(objectToWrite) + "\n");
	}
}
