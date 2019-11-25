g++ -std=c++11 -O3 -c -oDatabaseGenerator.o DatabaseGenerator.cpp
g++ -std=c++11 -O3 -c -oNullModel3Ary.o NullModel3Ary.cpp
g++ -std=c++11 -O3 -c -oRMiner.o RMiner.cpp
g++ -std=c++11 -O3 -c -oReadCSV.o ReadCSV.cpp
g++ -std=c++11 -oP-N-RMiner DatabaseGenerator.o NullModel3Ary.o RMiner.o ReadCSV.o
