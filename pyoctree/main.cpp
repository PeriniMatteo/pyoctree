#include <string>
#include <stdio.h>

#include <stdlib.h>
#include <iostream>
#include "cOctree.cpp"
#include "octree_builder.h"
#include "octree_export.h"


void ResetInside(cOctNode &node)
{
  // Recursive function used to "reset" an octree
  node.inside = false;
  for (unsigned int i=0; i<node.branches.size(); i++) {
    ResetInside(node.branches[i]);
  }
}

void oct_uniform(cOctNode &node1, cOctNode &node2)
{
  // Recursive function used to uniform the structure of two existing octree
  auto branchOffsets = cOctree::getBranchOffset();
  if (node1.isLeafNode() != node2.isLeafNode()){
    
    if (node1.branches.size() > node2.branches.size()){
      vector<double> position(3);
      for (int i=0; i<node2.NUM_BRANCHES_OCTNODE; i++) {
          for (int j=0; j<3; j++) {
              position[j] = node2.position[j] + 0.25*node2.size*branchOffsets[i][j]; }
          string nid = node2.nid + "-" + NumberToString(i);
          node2.addNode(node2.level+1,nid,position,0.5*node2.size, (int) 1);
      }
      for (unsigned int i=0; i<node2.branches.size(); i++) {
        node2.branches[i].inside = node2.inside;
      }
    }
    if (node1.branches.size() < node2.branches.size()){
      vector<double> position(3);
      for (int i=0; i<node1.NUM_BRANCHES_OCTNODE; i++) {
          for (int j=0; j<3; j++) {
              position[j] = node1.position[j] + 0.25*node1.size*branchOffsets[i][j]; }
          string nid = node1.nid + "-" + NumberToString(i);
          node1.addNode(node1.level+1,nid,position,0.5*node1.size, (int) 1); 
      }
      for (unsigned int i=0; i<node1.branches.size(); i++) {
        node1.branches[i].inside = node1.inside;
      }
    }
    for (unsigned int i=0; i<node1.branches.size(); i++) {
      oct_uniform(node1.branches[i], node2.branches[i]);
    }
  } else {
    if (node1.branches.size() != 0){
      for (unsigned int i=0; i<node1.branches.size(); i++) {
        oct_uniform(node1.branches[i], node2.branches[i]);
      }
    }
  }
}

void oct_sum(cOctNode &node1, cOctNode &node2, cOctNode &node3)
{
    // Recursive function used to compute boolean sum
    if (node1.isLeafNode()) {
      if (node1.numPolys()!=0 || node2.numPolys()!=0) {
        node3.inside = true;
      }
    } else {
      for (unsigned int i=0; i<node1.branches.size(); i++) {
        oct_sum(node1.branches[i], node2.branches[i], node3.branches[i]);
      }
    }
}

void oct_diff(cOctNode &node1, cOctNode &node2, cOctNode &node3)
{
    // Recursive function used to compute boolean difference
    if (node1.isLeafNode()) {
      if (node1.inside != node2.inside) {
        node3.inside = true;
      }
    } else {
      for (unsigned int i=0; i<node1.branches.size(); i++) {
        oct_diff(node1.branches[i], node2.branches[i], node3.branches[i]);
      }
    }
}

void oct_intersect(cOctNode &node1, cOctNode &node2, cOctNode &node3)
{
    // Recursive function used to compute boolean difference
    if (node1.isLeafNode()) {
      if (node1.inside && node2.inside) {
        node3.inside = true;
      }
    } else {
      for (unsigned int i=0; i<node1.branches.size(); i++) {
        oct_intersect(node1.branches[i], node2.branches[i], node3.branches[i]);
      }
    }
}

cOctree oct_reset(cOctree &o1, cOctree &o2)
{
  cOctree o3 = o1;
  ResetInside(o3.root);
  return o3;
}

int main(){
  
  string name;
  //name = "./Examples/sphere.stl";
  int level = 9;
  cOctree o1 =  oct_builder ("./Examples/mesh_original2.stl", level, 2);
  cOctree o2 =  oct_builder ("./Examples/mesh_damaged_scanned2.stl", level, 2);
  
  std::cout << "Leaf nodes of o1 : " << o1.get_Leafs().size() << std::endl;
  std::cout << "Leaf nodes of o2 : " << o2.get_Leafs().size() << std::endl;
  //std::cout << NumberToString( 45 ) << std::endl;

  oct_uniform(o1.root, o2.root);
  cOctree oo = oct_reset(o1,o2);
  cOctree oo1 = oct_reset(o1,o2);
  cOctree oo2 = oct_reset(o1,o2);
  
  std::cout << std::endl << "inside nodes of oo : " << oo.get_Inside().size() << std::endl ;
  std::cout << "Leaf nodes of o1 : " << o1.get_Leafs().size() << std::endl;
  std::cout << "Leaf nodes of o2 : " << o2.get_Leafs().size() << std::endl;
  
  std::cout << "inside nodes of o1 : " << o1.get_Inside().size() << std::endl;
  std::cout << "inside nodes of o2 : " << o2.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo : " << oo.get_Inside().size() << std::endl;
  
  oct_sum(o1.root, o2.root, oo.root);
  oct_diff(o1.root, o2.root, oo1.root);
  oct_intersect(o1.root, o2.root, oo2.root);

  std::cout << "inside nodes of o1 : " << o1.get_Inside().size() << std::endl;
  std::cout << "inside nodes of o2 : " << o2.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo : " << oo.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo1 : " << oo1.get_Inside().size() << std::endl;
  std::cout << "inside nodes of oo2 : " << oo2.get_Inside().size() << std::endl;
  
  //oct_save ( o1, 0, "o_all.vtu" );
  //oct_save ( o1, 1, "o_leaf.vtu" );
  oct_save ( o1, 1, "o1_inside.vtu" );
  oct_save ( o2, 1, "o2_inside.vtu" );
  oct_save ( oo, 2, "oo_sum.vtu" );
  oct_save ( oo1, 2, "oo_diff.vtu" );
  oct_save ( oo2, 2, "oo_int.vtu" );
  
  
  return 0;
}
