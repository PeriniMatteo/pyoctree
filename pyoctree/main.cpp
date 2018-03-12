#include <string>
#include <stdio.h>

#include <stdlib.h>
#include <iostream>
#include "cOctree.cpp"
#include "octree_builder.h"
#include "octree_export.h"


void ResetInside(cOctNode &node)
{
  // Recursive function used by getNodesFromLabel
  if (node.isLeafNode()) {
    node.inside = false;
  } else {
    for (unsigned int i=0; i<node.branches.size(); i++) {
      ResetInside(node.branches[i]);
    }
  }
}

void oct_uniform(cOctNode &node1, cOctNode &node2)
{
  
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
/*
void oct_sum(cOctNode &node)
{
    // Recursive function used by getNodesFromLabel
    if (node1.isLeafNode() && node2.isLeafNode()) {
      if (node1.inside || node2.inside) {
        node3.inside = true;
      }
    } else if (node1.isLeafNode() != node2.isLeafNode()){
      //if (node1.branches.size() != 0){
      //}
      //if (node1.branches.size() != 0){
      //}
      if (node1.branches.size() > node2.branches.size()){
        vector<double> position(3);
        for (int i=0; i<node2.NUM_BRANCHES_OCTNODE; i++) {
            for (int j=0; j<3; j++) {
                position[j] = node2.position[j] + 0.25*node2.size*branchOffsets[i][j]; }
            string nid = node2.nid + "-" + NumberToString(i);
            node2.addNode(node2.level+1,nid,position,0.5*node2.size, (int) 1); 
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
      }
    } else {
      if (node2.branches.size() == node1.branches.size()){
        for (unsigned int i=0; i<node.branches.size(); i++) {
          ResetInside(node.branches[i]);
        }
      }
    }
}*/

cOctree oct_diff(cOctree &o1, cOctree &o2)
{
  cOctree o3 = o1;
  ResetInside(o3.root);
  //oct_uniform(o1.root, o2.root);
  /*
  // Recursive function used by getNodesFromLabel
  if (node.isLeafNode()) {
    nodeList.push_back(&node);
  } else {
    for (unsigned int i=0; i<node.branches.size(); i++) {
      findIfLeaf(node.branches[i], nodeList);
    }
  }*/
  return o3;
}
/*
void CheckIfNodeIsTheSame(cOctNode &node1, cOctNode &node2, cOctNode &node3)
{
  if (node1.level < MAX_OCTREE_LEVELS){
    // Split node into 8 branches
    for (int i=0; i<node.NUM_BRANCHES_OCTNODE; i++) {
      string nid = node.nid + "-" + NumberToString(i);
      
    }
  }
  // Reallocate date from node to branches
  for (int i=0; i<node.NUM_BRANCHES_OCTNODE; i++) {
    if (node.branches[i].numPolys()!=0 && node.branches[i].level < MAX_OCTREE_LEVELS){
      CheckIfNodeIsTheSame(node.branches[i]);
    }   
  }
}
*/
int main(){
  
  string name;
  name = "./Examples/sphere.stl";
  int level = 6;
  cOctree o1 =  oct_builder (name, level-1, 2);
  cOctree o2 =  oct_builder (name, level, 2);
  
  std::cout << "inside node of o1 : " << o1.get_Leafs().size() << std::endl;
  std::cout << "inside node of o2 : " << o2.get_Leafs().size() << std::endl;
  //std::cout << NumberToString( 45 ) << std::endl;
  cOctree oo = oct_diff(o1,o2);
  
  std::cout << "inside node of oo : " << oo.get_Inside().size() << std::endl;

  oct_uniform(o1.root, o2.root);
  std::cout << "inside node of o1 : " << o1.get_Leafs().size() << std::endl;
  std::cout << "inside node of o2 : " << o2.get_Leafs().size() << std::endl;
  //oct_save ( o1, 0, "o_all.vtu" );
  //oct_save ( o1, 1, "o_leaf.vtu" );
  //oct_save ( o1, 2, "o_inside.vtu" );
  oct_save ( oo, 2, "o_inside.vtu" );
  return 0;
}
