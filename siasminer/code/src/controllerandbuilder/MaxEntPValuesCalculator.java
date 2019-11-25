package controllerandbuilder;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

import model.Graph;
import model.IndexedUniques;
import model.Vertex;
import utils.TabUtilities;

public class MaxEntPValuesCalculator {
	public static double approximateZero=0.00000000000000000001;
	
	public static void transformAllAttributes(Graph graph,int numberOfIterations){
		// initialise tables of probas and pValues
		for (int i=0;i<graph.vertices.length;i++){
			graph.vertices[i].probabilities=new double[graph.descriptorMetaData.attributesName.length];
			graph.vertices[i].pValues=new double[graph.descriptorMetaData.attributesName.length];
		}
		int startInc=0;
		for (int i=0;i<graph.descriptorMetaData.maxEntGroups.length;i++){
			transformAttributes(graph,numberOfIterations,startInc,startInc+graph.descriptorMetaData.maxEntGroups[i]);
			startInc+=graph.descriptorMetaData.maxEntGroups[i];
		}
	}
	
	private static void transformAttributes(Graph graph,int numberOfIterations, int startInc,int endExc){
		
		double n=graph.vertices.length;
		//double m=graph.descriptorMetaData.attributesName.length;
		double m=endExc-startInc;
		double[] logSpaceDist=getLogspace(-5, 1, 100);
		if (m<=5){
			System.out.println("MaxEnt optim: there is less than 5 attributes, step size will be smaller, nb iterations is 1000. (PS: A constraint automatic checker need to be implemented)");
			logSpaceDist=getLogspace(-15,-2,30);
			numberOfIterations=1000;
		}
		double[] pRows=new double[(int)n];
		double[] pCols=new double[(int)m];
		for (int i=0;i<(int)n;i++){
			//pRows[i]=graph.vertices[i].attSum/m;
			pRows[i]=getAttSumInVertex(graph.vertices[i],startInc,endExc)/m;
		}
		for (int i=0;i<(int)m;i++){
			pCols[i]=graph.descriptorMetaData.sumPerAttribute[i+startInc]/n;
		}
		
		//double[][] probabilities=new double[graph.vertices.length][graph.descriptorMetaData.attributesName.length];
		IndexedUniques pRowsUnique=new IndexedUniques(pRows);
		IndexedUniques pColsUnique=new IndexedUniques(pCols);
		int nUnique=pRowsUnique.uniqueValues.length;
		int mUnique=pColsUnique.uniqueValues.length;
		double[] la=TabUtilities.initTable((int)mUnique, -1);
		double[] mu=TabUtilities.initTable((int)nUnique, -1);
		double[] errors=new double[numberOfIterations];
		double[] h=new double[(int)(mUnique+nUnique)];
		
		double[][] E=new double[mu.length][la.length];
		double[][] Etry=new double[mu.length][la.length];
		double[][] pstry=new double[mu.length][la.length];
		double[][] ps=new double[mu.length][la.length];
		double[] gla=new double[la.length];
		double[] gmu=new double[mu.length];
		double[][] H=new double[(int)(mUnique+nUnique)][(int)(mUnique+nUnique)];
		double[] deltalamu=new double[(int)(mUnique+nUnique)];
		double fBest=0;
		double errorBest=0;
		
		
		
		//double[] logSpaceDist=getLogspace(-15,-2,30);
		double[] latry=new double [la.length];
		double[] mutry=new double [mu.length];
		double[] glatry=new double[la.length];
		double[] gmutry=new double[mu.length];
		
		
		for (int iteration=0;iteration<numberOfIterations-1;iteration++){
			for (int i=0;i<mu.length;i++){
				for (int j=0;j<la.length;j++){
					E[i][j]=Math.exp(la[j])*Math.exp(mu[i]);
					ps[i][j]=E[i][j]/(1-E[i][j]);
				}
			}
			for (int i=0;i<la.length;i++){
				gla[i]=-n*pColsUnique.uniqueValues[i];
				for (int j=0;j<mu.length;j++){
					gla[i]+=ps[j][i]*pRowsUnique.nbRepetitions[j];
				}
			}
			for (int i=0;i<mu.length;i++){
				gmu[i]=-m*pRowsUnique.uniqueValues[i];
				for (int j=0;j<la.length;j++){
					gmu[i]+=ps[i][j]*pColsUnique.nbRepetitions[j];
				}
			}
			errors[iteration]=getError(gla,gmu,pColsUnique.indices,pRowsUnique.indices);
			TabUtilities.reinitTable(H, 0);
			for (int i=0;i<nUnique;i++){
				for (int j=0;j<mUnique;j++){
					H[i+mUnique][j]=E[i][j]/((1-E[i][j])*(1-E[i][j]));
				}
			}
			for (int i=0;i<mUnique;i++){
				for (int j=0;j<nUnique;j++){
					H[i][j+mUnique]=H[j+mUnique][i];
				}
			}
			for (int k=0;k<mUnique+nUnique;k++){
				h[k]=0;
				for (int i=0;i<mUnique;i++){
					h[k]+=(H[k][i]*((double)pColsUnique.nbRepetitions[i]));
				}
				for (int i=0;i<nUnique;i++){
					h[k]+=(H[k][mUnique+i]*((double)pRowsUnique.nbRepetitions[i]));
				}
			}
			for (int i=0;i<mUnique;i++){
				deltalamu[i]=-gla[i]/h[i];
			}
			for (int i=0;i<nUnique;i++){
				deltalamu[mUnique+i]=-gmu[i]/h[mUnique+i];
			}
			fBest=0;
			errorBest=errors[iteration];
			for (double f : logSpaceDist){
				for (int i=0;i<la.length;i++){
					latry[i]=la[i];
					latry[i]+=f*deltalamu[i];
				}
				for (int i=0;i<mu.length;i++){
					mutry[i]=mu[i];
					mutry[i]+=f*deltalamu[mUnique+i];
				}
				double maxV=0;
				for (int i=0;i<Etry.length;i++){
					for (int j=0;j<Etry[i].length;j++){
						Etry[i][j]=Math.exp(latry[j])*Math.exp(mutry[i]);
						maxV=(Etry[i][j]>maxV)?Etry[i][j]:maxV;
					}
				}
				if (maxV<1){
					for (int i=0;i<Etry.length;i++){
						for (int j=0;j<Etry[i].length;j++){
							pstry[i][j]=Etry[i][j]/(1-Etry[i][j]);
						}
					}
					for (int i=0;i<mUnique;i++){
						glatry[i]=-n*pColsUnique.uniqueValues[i];
						for (int j=0;j<nUnique;j++){
							glatry[i]+=pstry[j][i]*pRowsUnique.nbRepetitions[j];
						}
					}
					for (int i=0;i<nUnique;i++){
						gmutry[i]=-m*pRowsUnique.uniqueValues[i];
						for (int j=0;j<mUnique;j++){
							gmutry[i]+=pstry[i][j]*pColsUnique.nbRepetitions[j];
						}
					}
					double errorTry=getError(glatry, gmutry, pColsUnique.indices,pRowsUnique.indices);
					if (errorTry<errorBest){
						fBest=f;
						errorBest=errorTry;
					}
				}
			}
			for (int i=0;i<mUnique;i++){
				la[i]=la[i]+fBest*deltalamu[i];
			}
			for (int j=0;j<nUnique;j++){
				mu[j]=mu[j]+fBest*deltalamu[j+mUnique];
			}
		}
		for (int i=0;i<mu.length;i++){
			for (int j=0;j<la.length;j++){
				E[i][j]=Math.exp(la[j])*Math.exp(mu[i]);
				ps[i][j]=E[i][j]/(1-E[i][j]);
			}
		}
		for (int i=0;i<la.length;i++){
			gla[i]=-n*pColsUnique.uniqueValues[i];
			for (int j=0;j<mu.length;j++){
				gla[i]+=ps[j][i]*pRowsUnique.nbRepetitions[j];
			}
		}
		for (int i=0;i<mu.length;i++){
			gmu[i]=-m*pRowsUnique.uniqueValues[i];
			for (int j=0;j<la.length;j++){
				gmu[i]+=ps[i][j]*pColsUnique.nbRepetitions[j];
			}
		}
		errors[numberOfIterations-1]=getError(gla,gmu,pColsUnique.indices,pRowsUnique.indices);
		
		for (int i=0;i<graph.vertices.length;i++){
			for (int j=0;j<m;j++){
				graph.vertices[i].probabilities[j+startInc]=1-E[pRowsUnique.indices[i]][pColsUnique.indices[j]];
				graph.vertices[i].pValues[j+startInc]=Math.pow(1-graph.vertices[i].probabilities[j], graph.vertices[i].attributes[j]);
				graph.vertices[i].pValues[j+startInc]=(graph.vertices[i].pValues[j+startInc]>approximateZero)?graph.vertices[i].pValues[j+startInc]:approximateZero;
			}
		}
	}
	
	
	private static double getError(double[] gla,double[] gmu,int[] jcols,int[] jrows){
		double error=0;
		for (int index : jcols){
			error+=(gla[index]*gla[index]);
		}
		for (int index : jrows){
			error+=(gmu[index]*gmu[index]);
		}
		return Math.sqrt(error);
		
	}
	
	private static double[] getLogspace(double pow1,double pow2,int bins){
		double[] result=new double[bins];
		double binDist=(pow2-pow1)/((double)(bins-1));
		for (int i=0;i<bins;i++){
			result[i]=Math.pow(10, pow1+binDist*((double)i));
		}
		return result;
	}
	
	private static double getAttSumInVertex(Vertex v,int startInc,int endExc){
		double attSum=0;
		for (int i=startInc;i<endExc;i++){
			attSum+=v.attributes[i];
		}
		return attSum;
	}
	
	public static void writePValues(Graph graph) {
		try {
			BufferedWriter writer=new BufferedWriter(new FileWriter("pValues.txt"));
			writer.write("vertexName");
			for (int j=0;j<graph.descriptorMetaData.attributesName.length;j++) {
				writer.write(","+graph.descriptorMetaData.attributesName[j]);
			}
			writer.write("\n");
			for (int i=0;i<graph.vertices.length;i++) {
				writer.write(graph.vertices[i].id);
				for (int j=0;j<graph.descriptorMetaData.attributesName.length;j++) {
					writer.write(","+String.format( "%.1f", graph.vertices[i].pValues[j] ) );
				}
				writer.write("\n");
			}
			writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
}
