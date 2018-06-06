
#include <array>
#include <string>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>

//VTK
#include "vtkXMLUnstructuredGridWriter.h"
#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkHexahedron.h>

//OpenCascade
#include "BRepPrimAPI_MakeBox.hxx"
#include "STEPControl_Writer.hxx"

//My sources
#include "cOctree.h"
#include "octree_builder.h"
#include "octree_export.h"
#include "octree_export_STEP.h"
#include "booleans.h"
#include "filters.h"

vector<double> find_octree_position(string _first_mesh, string _second_mesh){
  cOctree o1 =  oct_builder (_first_mesh, 2, 1);
  cOctree o2 =  oct_builder (_second_mesh, 2, 1);
  std::cout << "xxx" << std::endl;
  vector<double> p1 = o1.getPositionRoot(); //position of the first octree
  vector<double> p2 = o2.getPositionRoot(); //position of the second octree
  vector<double> pc(3); //mean position of the center
  std::cout << "xxx2" << std::endl;
  for (int j=0; j<3; j++) {
    pc[j] = (p1[j] + p2[j])*0.5;
  }
  return pc;
}
double find_octree_size(string _first_mesh, string _second_mesh){
  cOctree o1 =  oct_builder (_first_mesh, 2, 1);
  cOctree o2 =  oct_builder (_second_mesh, 2, 1);
  
  double s1 = o1.getSizeRoot(); //size of the first octree
  double s2 = o2.getSizeRoot(); //size of the second octree

  vector<double> p1 = o1.getPositionRoot(); //position of the first octree
  vector<double> p2 = o2.getPositionRoot(); //position of the second octree
  vector<double> min1(3);
  vector<double> min2(3);
  vector<double> max1(3);
  vector<double> max2(3);
  vector<double> low(3);
  vector<double> upp(3);
  vector<double> range(3);
  
  for (int i=0; i<3; i++) {
    min1[i] = p1[i] - s1 * 0.5;
    max1[i] = p1[i] + s1 * 0.5;
    min2[i] = p2[i] - s2 * 0.5;
    max2[i] = p2[i] + s2 * 0.5;
  }
  
  low = min1;
  upp = max1;
  
  for (int j=0; j<3; j++) {
    if (min2[j] < low[j]) { low[j] = min2[j]; }
    if (max2[j] > upp[j]) { upp[j] = max2[j]; }
  }
  
  range = vectSubtract(upp,low);
  double size = range[0];
  for (int i=1; i<3; i++) {
    if (range[i] > size) { size = range[i]; }
  }
  // Scale up size of node by 5%
  size *= 1.05;
  return size;
}

int main(){
  //string first_mesh = "../Examples/model_rotated.stl";
  string first_mesh = "./Examples/mesh_original2.stl";

  //string second_mesh = "../Examples/model_scanned_aligned.stl";
  string second_mesh = "./Examples/mesh_damaged_scanned2.stl";

  string name;
  vector<double> oct_position = find_octree_position(first_mesh, second_mesh);
  double oct_size = find_octree_size(first_mesh, second_mesh);
  //vector<double> oct_position = find_octree_position(first_mesh, first_mesh);
  //double oct_size = find_octree_size(first_mesh, first_mesh);
  std::cout << "oct_size = " << oct_size << std::endl;
  for (int i = 0; i<3; i++){
    std::cout << "oct_position[" << i <<"] = " << oct_position[i] << std::endl; 
  }
  int level = 7;
  int level_cube = 9;
  vector<double> cube_position = oct_position;
  
  cube_position[0] += 0.2*oct_size;
  cube_position[1] -= 0.3*oct_size;
  cube_position[2] += 0.1*oct_size;
  
  
  double cube_size = 0.25*oct_size;
  std::cout << "cube_size = " << cube_size << std::endl;
  for (int i = 0; i<3; i++){
    std::cout << "cube_position[" << i <<"] = " << cube_position[i] << std::endl; 
  }
  //cOctree o1 =  oct_builder (first_mesh, level, 2);
  //cOctree o2 =  oct_builder (second_mesh, level, 2);
  //cOctree o1 =  oct_builder (first_mesh, level, 2, oct_position, oct_size);
  //cOctree o2 =  oct_builder (second_mesh, level, 2, oct_position, oct_size);
  cOctree o1 =  oct_builder (first_mesh, level, 2, oct_position, oct_size, cube_position, cube_size, level_cube);
  cOctree o2 =  oct_builder (second_mesh, level, 2, oct_position, oct_size, cube_position, cube_size, level_cube);
  /*
  std::cout << "Leaf nodes of o1 : " << o1.get_Leafs().size() << std::endl;
  //std::cout << "Leaf nodes of o2 : " << o2.get_Leafs().size() << std::endl;
  //std::cout << NumberToString( 45 ) << std::endl;*/

  oct_uniform(o1.root, o2.root);
  //cOctree oo = oct_reset(o1,o2);
  cOctree oo1 = oct_clone(o1);
  /*cOctree oo2 = oct_reset(o1,o2);
  
  std::cout << std::endl << "inside nodes of oo : " << oo.get_Inside().size() << std::endl ;
  std::cout << "Leaf nodes of o1 : " << o1.get_Leafs().size() << std::endl;
  std::cout << "Leaf nodes of o2 : " << o2.get_Leafs().size() << std::endl;
  
  std::cout << "inside nodes of o1 : " << o1.get_Inside().size() << std::endl;
  std::cout << "inside nodes of o2 : " << o2.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo : " << oo.get_Inside().size() << std::endl;
  
  oct_sum(o1.root, o2.root, oo.root);*/
  oct_diff(o1.root, o2.root, oo1.root);
  /*oct_intersect(o1.root, o2.root, oo2.root);

  std::cout << "inside nodes of o1 : " << o1.get_Inside().size() << std::endl;
  std::cout << "inside nodes of o2 : " << o2.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo : " << oo.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo1 : " << oo1.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo2 : " << oo2.get_Inside().size() << std::endl;
  //oct_save ( o1, 1, "o_leaf.vtu" );*/
  
  //for (cOctNode* &node : o1.get_Nodes()){
  //  std::cout << node -> nid << "...." << node -> isLeafNode() << "...." << node -> inside  << std::endl;
  //}
  //ResetInside(o1.root);
  //o1.getNodeFromId("0-2-3-4") -> inside = true;
  //cOctNode x = *o1.getNodeFromId("0-2-3-4");
  //inside = true;
  //x.inside = true;
  
  
  
  
  
  //vector<double> vp(3);
  //vp[0] = 0.5;
  //vp[1] = 0.1;
  //vp[2] = -0.6;
  
  /*cOctNode* x = o1.getNodeFromPoint(vp);
  x -> inside = true;
  std::cout << "nid of node that contains vp : " <<  x -> nid << std::endl;
  
  oct_save ( o1, 0, "o_all.vtu" );
  oct_save ( o1, 2, "o1_center.vtu" ); 
  x -> inside = false;
  vector<cOctNode*> xx = o1.getNeighbours(*o1.getNodeFromPoint(vp));
  std::cout << "test" << std::endl;
  for (cOctNode* nod : xx){
    nod -> inside = true;
    std::cout << nod -> nid << std::endl;
  }*/

  oct_save ( o1, 2, "o1_inside.vtu" );
  oct_save ( o2, 2, "o2_inside.vtu" );
  //oct_save ( oo, 2, "oo_sum.vtu" );
  oct_save ( oo1, 2, "oo_diff.vtu" );
  cOctree oo2 = oct_clone(oo1, true); 
  oct_neighbour_filter(oo2,8);
  oct_save ( oo2, 2, "oo_filter.vtu" );
  cOctree oo3 = oct_clone(oo1);
  oct_diff(oo1.root, oo2.root, oo3.root);
  oct_save ( oo3, 2, "oo_filter_diff.vtu" );
  //export_step_model(oo1, "difference.step" );
  return 0;
}
