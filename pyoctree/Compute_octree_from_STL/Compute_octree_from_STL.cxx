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
#include <vector>
#include "../cOctree.cpp"

int main ( int argc, char *argv[] )
{
  if ( argc != 3 )
  {
    cout << "Required parameters: Filename and max_depth" << endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = argv[1];

  vtkSmartPointer<vtkSTLReader> reader =
    vtkSmartPointer<vtkSTLReader>::New();
  reader->SetFileName(inputFilename.c_str());
  reader->Update();
  
  auto stl = reader->GetOutput();
  
  int numPoints = stl->GetNumberOfPoints();
  std::cout << "Number of vertex = " << numPoints << std::endl;
  
  int numPolys     = stl->GetNumberOfCells();
  std::cout << "Number of faces  = " << numPolys << std::endl;
  
  std::vector<std::vector<double>> pointCoords(numPoints,std::vector<double>(3));
  
  for(vtkIdType i = 0; i < stl->GetNumberOfPoints(); i++)
    {
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

  return EXIT_SUCCESS;
}
