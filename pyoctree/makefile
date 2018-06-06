# the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
#CFLAGS  = 

# the build target executable:
TARGET = main

#INCLUDE = -I/usr/local/include/opencascade/
INCLUDE = -I/usr/include/oce/ -I/usr/include/vtk-6.3
#INCLUDE = -I/usr/include/vtk-6.3

#OPENCASCADE LIBRARIES
LIBOCE =-L/usr/lib/x86_64-linux-gnu/oce-0.18/ -lTKernel -lTKMath -lTKBRep -lTKTopAlgo -lTKPrim -lTKSTEP 
# -lTKShHealing -lTKBO -lTKBool -lTKHLR -lTKFillet -lTKOffset -lTKFeat -lTKMesh -lTKXMesh -lTKService -lTKV3d -lTKG2d -lTKG3d -lTKOpenGl -lTKMeshVS -lTKGeomBase -lTKGeomAlgo -lTKCDF -lTKLCAF -lTKCAF -lTKBinL -lTKXmlL -lTKBin -lTKSTEPBase -lTKXml -lTKSTEPAttr -lTKSTEP209 -lTKStdLSchema -lTKStdSchema -lTKTObj -lTKBinTObj -lTKXmlTObj -lTKXSBase  -lTKIGES -lTKXCAF -lTKXDEIGES -lTKXDESTEP -lTKSTL -lTKVRML -lTKXmlXCAF -lTKBinXCAF -lTKDraw -lTKTopTest -lTKViewerTest -lTKXSDRAW -lTKXDEDRAW -lTKTObjDRAW -lTKQADraw

#VTK LIBRARIES
LIBVTK = -L/usr/lib/x86_64-linux-gnu/ -L./.. -lvtkCommonCore-6.3 -lvtkIOXML-6.3 -lvtkCommonDataModel-6.3 -lvtkIOGeometry-6.3 -lvtkCommonExecutionModel-6.3

#COCTREE SOURCES
#LIBOCT = -L

all: $(TARGET).cpp cOctree.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(TARGET).cpp cOctree.cpp -o $(TARGET) $(LIBVTK) $(LIBOCE)

clean:
	$(RM) $(TARGET)
