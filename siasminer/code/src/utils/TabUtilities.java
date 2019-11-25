package utils;

public class TabUtilities {
	
	public static double[] initTable(int size,double value){
		double[] table=new double[size];
		for (int i=0;i<table.length;i++){
			table[i]=value;
		}
		return table;
	}
	
	public static void reinitTable(double[][] table,double value){
		for (int i=0;i<table.length;i++){
			for (int j=0;j<table[i].length;j++){
				table[i][j]=value;
			}
		}
	}

}
