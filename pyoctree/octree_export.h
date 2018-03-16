#include "vtkXMLUnstructuredGridWriter.h"
#include <vtkPolyData.h>
//#include <vtkSTLReader.h>
//#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSet.h>
//#include <vtkCell.h>
#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkHexahedron.h>
#include <vector>
#include <array>
#include <string>
//#include "cOctree.h"



#ifdef __GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif


void getNodeRep(cOctNode &node, 
                std::vector<std::array<double,3>>    &vertexCoords, 
                vector<std::array<int,8>>            &vertexConnect, 
                std::vector<std::array<double,3>>    &offsets){

  std::array<int,8> connect;
  std::array<double,3> vi;
  for (int i=0; i<8; i++){
    for (int j=0;j<3; j++){
      vi[j]= node.position[j] + 0.5*node.size*offsets[i][j];
      connect[i] = vertexCoords.size();
    }
    vertexCoords.push_back({vi[0], vi[1], vi[2]});
  }
  vertexConnect.push_back({connect[0],
                          connect[1],
                          connect[2],
                          connect[3],
                          connect[4],
                          connect[5],
                          connect[6],
                          connect[7]});
}

#ifdef __GCC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

void getTree(cOctNode &node, 
            std::vector<std::array<double,3>>    &vertexCoords, 
            vector<std::array<int,8>>            &vertexConnect,
            std::vector<std::array<double,3>>    &offsets){

  if (node.level==1){
    getNodeRep(node, vertexCoords, vertexConnect, offsets);
  }
  
  if (!node.isLeafNode()){
    for(cOctNode &i : node.branches) {
      getNodeRep(i, vertexCoords, vertexConnect, offsets);
      getTree(i, vertexCoords, vertexConnect, offsets);
    }
  }
}

void getLeafs(cOctNode &node, 
            std::vector<std::array<double,3>>    &vertexCoords, 
            vector<std::array<int,8>>            &vertexConnect,
            std::vector<std::array<double,3>>    &offsets){
  
  if (!node.isLeafNode()){
    for(cOctNode &i : node.branches) {
      if (i.isLeafNode() && !(i.numPolys()==0)){
        getNodeRep(i, vertexCoords, vertexConnect, offsets);
      }else{
        getLeafs(i, vertexCoords, vertexConnect, offsets);
      }
    }
  }
}

void getInside(cOctNode &node, 
            std::vector<std::array<double,3>>    &vertexCoords, 
            vector<std::array<int,8>>            &vertexConnect,
            std::vector<std::array<double,3>>    &offsets){
  
  if (!node.isLeafNode()){
    for(cOctNode &branch : node.branches) {
      if (branch.inside){
        getNodeRep(branch, vertexCoords, vertexConnect, offsets);
      }else{
        getInside(branch, vertexCoords, vertexConnect, offsets);
      }
    }
  }
}


int oct_save ( cOctree oct, int mode, string output_file ){

  int tree_type;
  tree_type = mode;
  string output_file_name = output_file;


#ifdef __GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

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


#ifdef __GCC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

  // Call iterative function 
  switch ( tree_type  ) {

    case 0: 
      getTree(oct.root, vertexCoords, vertexConnect, offsets);
    break;

    case 1: 
      getLeafs(oct.root, vertexCoords, vertexConnect, offsets);
    break;
  
    case 2: 
      getInside(oct.root, vertexCoords, vertexConnect, offsets);
    break;
  }
  
  //std::cout << "Number of coords = " << vertexCoords.size() << std::endl;
  //std::cout << "Number of connects = " << vertexConnect.size() << std::endl;

  // Convert to vtk unstructured grid
  vtkSmartPointer<vtkUnstructuredGrid> uGrid =
    vtkSmartPointer<vtkUnstructuredGrid>::New();

  vtkSmartPointer<vtkFloatArray> coords =
    vtkSmartPointer<vtkFloatArray>::New();

  coords->SetNumberOfComponents(3);

  for (auto &v : vertexCoords){
    float tuple[3];
    tuple[0] = static_cast<float>(v[0]);
    tuple[1] = static_cast<float>(v[1]);
    tuple[2] = static_cast<float>(v[2]);
    coords->InsertNextTuple(tuple);
  }

  vtkSmartPointer<vtkPoints> vertices =
    vtkSmartPointer<vtkPoints>::New();

  vertices->SetData(coords);

  uGrid->SetPoints(vertices);

  // 2. Add element data to unstructured grid
  int numElems = vertexConnect.size();

  for (int i=0; i<numElems; i++){
    vtkSmartPointer<vtkHexahedron> hexelem = 
    vtkSmartPointer<vtkHexahedron>::New();
    for (int j=0; j<8; j++){
      hexelem->GetPointIds()->SetId(j,vertexConnect[i][j]);
    }
    uGrid->InsertNextCell(hexelem->GetCellType(), hexelem->GetPointIds());
  }
  vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer =
    vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();

  writer->SetFileName(output_file.c_str());
  writer->SetInputData(uGrid);
  writer->SetDataModeToAscii();
  writer->Write();
    
  return EXIT_SUCCESS;
}
