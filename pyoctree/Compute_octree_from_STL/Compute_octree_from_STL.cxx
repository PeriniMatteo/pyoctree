#include "vtkXMLUnstructuredGridWriter.h"
#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSet.h>
#include <vtkCell.h>
#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkHexahedron.h>
#include <vector>
#include <array>
#include "../cOctree.cpp"

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


int main ( int argc, char *argv[] ){

  bool fsave = false;
  int tree_type;
  if ( argc < 4 ){
    cout << "Required parameters: Filename, int max_depth, save_vtu(1 or 0), Tree type (0:Tree, 1:Leafs, 2:Inside)" << endl;
    return EXIT_FAILURE;
  }
  
  if (atoi(argv[3])==1){
    fsave = true;
  }else{
    cout << "Error: Saving parameter must be 0 or 1" << endl;
  }
  
  if (atoi(argv[4])==0 || atoi(argv[4])==1 || atoi(argv[4])==2){
    tree_type = atoi(argv[4]);
  }else{
    cout << "Error: Tree type must be 0, 1 or 2" << endl;
  }

  std::string inputFilename = argv[1];

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
  
  int max_depth = atoi(argv[2]);
  
  cOctree oct = cOctree(pointCoords, connectivity, max_depth);

  vector<cOctNode*> n = oct.get_Nodes();
  //std::cout << "Number of nodes = " << n.size() << std::endl;
  
  vector<cOctNode*> l = oct.get_Leafs();
  //std::cout << "Number of leafs = " << l.size() << std::endl;
  


  if (fsave==true){
    
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

    if (tree_type == 2){
      // If you do not compute that, the value of inside is false for all nodes!
      double r = oct.root.size * 2;
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
          cLine ray = cLine(p0,p1,0);
          intersectList = oct.findRayIntersect2(ray);
          int numInts = intersectList.size();
          node->inside = (numInts % 2 == 1);
        }else{
          node->inside = true;
        }
      }
    }
    
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
    
    std::cout << "Number of coords = " << vertexCoords.size() << std::endl;
    std::cout << "Number of connects = " << vertexConnect.size() << std::endl;

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
  
    writer->SetFileName("out.vtu");
    writer->SetInputData(uGrid);
    writer->SetDataModeToAscii();
    writer->Write();
  }

  return EXIT_SUCCESS;
}
