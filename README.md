# SIAS-Miner : Mining Subjectively Interesting Attributed Subgraphs

In this work we present SIAS-Miner, a framework that mines subjectively
interesting patterns by enumerating and ranking patterns from attributed graphs.   

Below we provide simple instructions for running SIAS-Miner.  

This was tested on Linux/Mac platform with Java 8.   


## Data
We provide the three graphs that we used in the qualitative experiments and the illustrative results sections. They are available in "Data" folder. 
Each graph is defined by the "graph.json" file. The listing below is a simple example of a graph file:

```

{

	"descriptorName": "FoursquareVenues",	

	"attributesName": ["Health","Tourism","Store","Food","Industry"],

	"vertices" : [

		{

			"vertexId" : "V1",

			"descriptorsValues" :[12,5,4,1,4]

		},

		{

			"vertexId" : "V2",

			"descriptorsValues" :[12,4,4,6,6]

		},

		{

			"vertexId" : "V3",

			"descriptorsValues" :[4,13,6,6,5]

		}

	],

	"edges" : [

		{

			"vertexId" :"V1",

			"connected_vertices" : ["V2","V3"]

		},

		{

			"vertexId" :"V2",

			"connected_vertices" : ["V1"]

		}

	]

}

```

First, this file presents information about the vertex descriptors (descriptorName and attributesName). Second, it defines the vertices and their attribute values. Third, it specifies the edges of the graph.



## How to use the Jar executable
The jar file location is "SiasMiner/Executable/SiasMiner.jar". Its requires two input files:

**1 - The graph file**: This contains the graph that the algorithm will mine. It needs to follow the format explained here in the Data section. 

**2 - Parameters file**: This file contains the values of the algorithm parameters. The location of an example of parameters file is "SiasMiner/Executable/parameters.txt":

```

maxDistance=1

minVertices=30

resultFolderPath=resultsIngredients

inputFilePath=../../Data/Ingredients/graph.json

binningType=percentileBins

topK=10000

applyUpdating=true

topKWithUpdating=300

```

These parameters are explained in what follows:

- maxDistance: the threshold on the maximum distance of a descriptor Nd(v), it corresponds to the variable "D" in the paper. It was set to 3 for London dataset, and to 1 for the other datasets.

- minVertices: the minimum number of vertices in a CSEA pattern

- resultFolderPath: the folder containing the results and the statistics files

- inputFilePath: the input graph file

- binningType: the type of the used binning, there is two choices: percentileBins, and equalBins

- topK: It can be used to specify the number of top patterns to output. If you need to output all the patterns, you can put: topK=-1

- applyUpdating: if it is set to true, a lazy sorter will be applied in SiasMiner output to iteratively select a top k set of patterns using the model updating

- topKWithUpdating: the number of patterns returned by the lazy sorter

In order to run the algorithm, you can move to the folder "SiasMiner/Executable", and run the following command:

```
java -jar SiasMiner.jar parameters.txt
```

This means that you need to specify in the command only the location of the parameters file.


## Performance experiments
Here we provide the datasets used in performance experiments with different dimensions. We also provide the source code of PNR-Miner (the baseline method).










