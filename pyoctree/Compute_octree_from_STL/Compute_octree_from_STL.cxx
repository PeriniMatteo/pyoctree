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
    for(cOctNode &i : node.branches) {
      if (i.isLeafNode() && (i.inside)){
        getNodeRep(i, vertexCoords, vertexConnect, offsets);
      }else{
        getInside(i, vertexCoords, vertexConnect, offsets);
      }
    }
  }
}


int main ( int argc, char *argv[] ){

  bool fsave = false;
  if ( argc < 3 ){
    cout << "Required parameters: Filename, int max_depth, save_vtu(1 or 0)" << endl;
    return EXIT_FAILURE;
  }
  if (atoi(argv[3])==1){
    fsave = true;
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

  int max_points=2;
  int max_depth = atoi(argv[2]);
  
  cOctree oct = cOctree(pointCoords, connectivity, max_points, max_depth);

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

    // Call iterative function 
   
    
    if (true){
      double r = oct.root.size * 2;
      //std::array<double,3> vii;
      std::vector<double> p0(3);
      std::vector<double> p1(3);
      vector<Intersection> intersectList;
      for (cOctNode* &node : oct.get_Nodes()){
        if (node->isLeafNode()){
          if (node->numPolys()==0){
            for (int j=0;j<3; j++){
              p0[j]= node->position[j];
              p1[j]= node->position[j];
            }
            p1[2] += r;
            
            //ray = np.array([[coords[0],coords[1],coords[2]+r],[coords[0],coords[1],coords[2]]],dtype=np.float32)
            cLine ray = cLine(p0,p1,0);
            intersectList = oct.findRayIntersect(ray);
            int numInts = intersectList.size();
            if (numInts == 1){
              node->inside = true;
            }
          }else{
            node->inside = false;
          }
        }
      }
    }
    ////////////////////////////////////////////////////////////////////////////
    getInside(oct.root, vertexCoords, vertexConnect, offsets);
    ////////////////////////////////////////////////////////////////////////////
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
