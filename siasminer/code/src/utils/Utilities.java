package utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Field;
import java.nio.file.Files;
import java.nio.file.LinkOption;
import java.nio.file.Paths;

import enumerations.BinningType;
import enumerations.DLApproach;
import enumerations.DescriptionLengthType;

public class Utilities {
	private static String fileSeparator = "";

	public static void readParametersFromFile(DesignPoint designPoint, String parametersFilePath) {
		try {
			BufferedReader parametersFile = new BufferedReader(new FileReader(new File(parametersFilePath)));
			String line;
			while ((line = parametersFile.readLine()) != null) {
				updateWithLine(designPoint, line);
			}
			// System.out.print(designPoint.minVertices);
			parametersFile.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void updateWithLine(DesignPoint designPoint, String line) {
		String[] elements = line.split("=");
		try {
			Field curField = DesignPoint.class.getField(elements[0]);
			Class<?> cc = curField.getType();
			if (cc == String.class) {
				curField.set(designPoint, elements[1]);
			} else if (cc == int.class) {
				curField.setInt(designPoint, Integer.parseInt(elements[1]));
			} else if (cc == double.class) {
				curField.setDouble(designPoint, Double.parseDouble(elements[1]));
			} else if (cc == boolean.class) {
				curField.setBoolean(designPoint, Boolean.parseBoolean(elements[1]));
			} else if (cc == BinningType.class) {
				curField.set(designPoint, BinningType.valueOf(elements[1]));
			} else if (cc == DLApproach.class) {
				curField.set(designPoint, DLApproach.valueOf(elements[1]));
			} else if (cc == DescriptionLengthType.class) {
				curField.set(designPoint, DescriptionLengthType.valueOf(elements[1]));
			} else if (cc == int[].class) {
				String[] el = elements[1].split(",");
				int[] tab = new int[el.length];
				for (int cpt = 0; cpt < el.length; cpt++) {
					tab[cpt] = Integer.parseInt(el[cpt]);
				}
				curField.set(designPoint, tab);
			} else if (cc == double[].class) {
				String[] el = elements[1].split(",");
				double[] tab = new double[el.length];
				for (int cpt = 0; cpt < el.length; cpt++) {
					tab[cpt] = Double.parseDouble(el[cpt]);
				}
				curField.set(designPoint, tab);
			}

		} catch (NoSuchFieldException | SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static void createFolder(String folderPath, boolean mustNotExist) {

		if (!Files.exists(Paths.get(folderPath), LinkOption.NOFOLLOW_LINKS)) {
			File f = new File(folderPath);
			f.mkdir();
			f.setExecutable(true);
			f.setReadable(true);
			f.setWritable(true);
		} else {
			if (mustNotExist) {
				throw new RuntimeException("outputFolder already exists");
			}
		}
	}

	public static String getFileSeparator() {
		if (fileSeparator.equals("")) {
			if (System.getProperty("os.name").startsWith("Windows")) {
				// includes: Windows 2000, Windows 95, Windows 98, Windows NT, Windows Vista,
				// Windows XP
				fileSeparator = "\\";
			} else {
				// everything else
				fileSeparator = "/";
			}
		}
		return fileSeparator;
	}

	public static double probaFunction(double bound, double pValue, boolean lowerBound) {
		if (lowerBound) {
			// k_ca
			if (bound == 1) {
				return 1 - pValue;
			}
			if (bound==0) {
				return 0; 
			}
			return Math.pow(1 - pValue, 1+ Math.floor(Math.log(bound) / Math.log(1 - pValue)));
			
		} 
		else {
			// l_ca
			if (bound==0) {
				return 0;
			}
			if (bound==1) {
				return 1;
			}
			return Math.pow(1 - pValue, Math.ceil(Math.log(bound) / Math.log(1 - pValue)));
		}
//		else {
//			// upperbound, if it is 1 then return 1
//			if (bound == 1) {
//				return 1;
//			}
//		}
		
	}
}
