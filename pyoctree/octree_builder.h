
//#include "vtkXMLUnstructuredGridWriter.h"
//#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkDataSet.h>
#include <vtkCell.h>
//#include <vtkUnstructuredGrid.h>
//#include <vtkFloatArray.h>
//#include <vtkPoints.h>
//#include <vtkHexahedron.h>

#include <iostream>
#include <vector>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <sstream>
#include <algorithm> // find, sort
#include <utility>  

//#include "cOctree.cpp"


cOctree oct_builder ( string name, int md, int insid){

  bool fsave = false;
  int tree_type;


  std::string inputFilename = name;

  vtkSmartPointer<vtkSTLReader> reader =
    vtkSmartPointer<vtkSTLReader>::New();

  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  auto stl = reader->GetOutput();
  
  int numPoints = stl->GetNumberOfPoints();
  //std::cout << "Number of vertex = " << numPoints << std::endl;
  
  int numPolys     = stl->GetNumberOfCells();
  //std::cout << "Number of faces  = " << numPolys << std::endl;
  
  std::vector<std::vector<double>> pointCoords(numPoints,std::vector<double>(3));
  
  for(vtkIdType i = 0; i < stl->GetNumberOfPoints(); i++){
    double p[3];
    stl->GetPoint(i,p);
    for (int j=0; j<3; j++){
      pointCoords[i][j] = p[j];
    }
  }
  
  std::vector<std::vector<int>> connectivity(numPolys,std::vector<int>(3));
  
  for(vtkIdType i = 0; i < stl->GetNumberOfCells(); i++){
    auto atri = stl->GetCell(i);
    auto ids = atri->GetPointIds();
    for (int j=0; j<3; j++){
      connectivity[i][j] = ids->GetId(j);
    }
  }
  
  int max_depth = md;
  
  cOctree oct = cOctree(pointCoords, connectivity, max_depth);

  vector<cOctNode*> n = oct.get_Nodes();
  std::cout << "Number of nodes = " << n.size() << std::endl;
  
  vector<cOctNode*> l = oct.get_Leafs();
  std::cout << "Number of leafs = " << l.size() << std::endl;
  



    
  std::vector<std::array<double,3>>  vertexCoords;
  vector<std::array<int,8>>         vertexConnect;
  std::vector<std::array<double,3>>  offsets = {{-1,-1,-1},
                                                {+1,-1,-1},
                                                {+1,+1,-1},
                                                {-1,+1,-1},
                                                {-1,-1,+1},
                                                {+1,-1,+1},
                                                {+1,+1,+1},
                                                {-1,+1,+1}};

  tree_type = insid;
  
  if (tree_type == 2){
    std::cout << "inside" << std::endl;
    // If you do not compute the following code, the value of inside is false for all nodes!
    double r = oct.root.size * 20;
    std::vector<double> p0(3);
    std::vector<double> p1(3);
    vector<int> intersectList;
    for (cOctNode* &node : oct.get_Leafs()){
      if (node->numPolys()==0){
        for (int j=0;j<3; j++){
          p0[j]= node->position[j];
          p1[j]= node->position[j];
        }
        p1[2] = p1[2] + r;
        p1[0] = 0.0;
        p1[1] = 0.0;
        p1[2] = r;
        
        cLine ray = cLine(p0,p1,1);
        intersectList = oct.findRayIntersect2(ray);
        int numInts = intersectList.size();
        node->inside = (numInts % 2 == 1);
      }else{
        node->inside = true;
      }
    }
  }

  return oct;
}

cOctree oct_builder ( string name, int md, int insid, vector<double> _position, double _size){

  bool fsave = false;
  int tree_type;
  vector<double> root_position = _position;
  double root_size = _size;

  std::string inputFilename = name;

  vtkSmartPointer<vtkSTLReader> reader =
    vtkSmartPointer<vtkSTLReader>::New();

  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  auto stl = reader->GetOutput();
  
  int numPoints = stl->GetNumberOfPoints();
  //std::cout << "Number of vertex = " << numPoints << std::endl;
  
  int numPolys     = stl->GetNumberOfCells();
  //std::cout << "Number of faces  = " << numPolys << std::endl;
  
  std::vector<std::vector<double>> pointCoords(numPoints,std::vector<double>(3));
  
  for(vtkIdType i = 0; i < stl->GetNumberOfPoints(); i++){
    double p[3];
    stl->GetPoint(i,p);
    for (int j=0; j<3; j++){
      pointCoords[i][j] = p[j];
    }
  }
  
  std::vector<std::vector<int>> connectivity(numPolys,std::vector<int>(3));
  
  for(vtkIdType i = 0; i < stl->GetNumberOfCells(); i++){
    auto atri = stl->GetCell(i);
    auto ids = atri->GetPointIds();
    for (int j=0; j<3; j++){
      connectivity[i][j] = ids->GetId(j);
    }
  }
  
  int max_depth = md;
  
  cOctree oct = cOctree(pointCoords, connectivity, root_position, root_size, max_depth);

  vector<cOctNode*> n = oct.get_Nodes();
  std::cout << "Number of nodes = " << n.size() << std::endl;
  
  vector<cOctNode*> l = oct.get_Leafs();
  std::cout << "Number of leafs = " << l.size() << std::endl;
  



    
  std::vector<std::array<double,3>>  vertexCoords;
  vector<std::array<int,8>>         vertexConnect;
  std::vector<std::array<double,3>>  offsets = {{-1,-1,-1},
                                                {+1,-1,-1},
                                                {+1,+1,-1},
                                                {-1,+1,-1},
                                                {-1,-1,+1},
                                                {+1,-1,+1},
                                                {+1,+1,+1},
                                                {-1,+1,+1}};

  tree_type = insid;
  
  if (tree_type == 2){
    std::cout << "inside" << std::endl;
    // If you do not compute the following code, the value of inside is false for all nodes!
    double r = oct.root.size * 20;
    std::vector<double> p0(3);
    std::vector<double> p1(3);
    vector<int> intersectList;
    for (cOctNode* &node : oct.get_Leafs()){
      if (node->numPolys()==0){
        for (int j=0;j<3; j++){
          p0[j]= node->position[j];
          p1[j]= node->position[j];
        }
        p1[2] = p1[2] + r;
        p1[0] = 0.0;
        p1[1] = 0.0;
        p1[2] = r;
        
        cLine ray = cLine(p0,p1,1);
        intersectList = oct.findRayIntersect2(ray);
        int numInts = intersectList.size();
        node->inside = (numInts % 2 == 1);
      }else{
        node->inside = true;
      }
    }
  }

  return oct;
}
