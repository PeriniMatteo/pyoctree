CXX = clang++ -std=c++11
OPT = -Wall
INC = -I/usr/local/include/vtk-8.1
LIB = -L/usr/local/lib -lvtkCommonCore-8.1 -lvtkIOXML-8.1 -lvtkCommonDataModel-8.1 -lvtkIOGeometry-8.1 -lvtkCommonExecutionModel-8.1

#CXX = g++ -std=c++11
#INC = -I/usr/include/vtk-6.3 

all:
	$(CXX) $(INC) $(OPT) main.cpp -o main $(LIB)
