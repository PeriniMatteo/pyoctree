
cOctree oct_neighbour_filter(cOctree &oct){
  std::cout << "number of inside before : " << oct.get_Inside().size() << std::endl;
  //static cOctree oct_copy = oct;
  int n = 0;
  vector<cOctNode*> nl;
  for (cOctNode* node : oct.get_Inside()){
    //cOctNode nod = *oct_copy.getNodeFromPoint(node -> position);
    //std::cout << nod.nid << std::endl;
    //string xxx = node -> nid;
    //std::cout << xxx << std::endl;
    //cOctNode* nod2 = oct.getNodeFromId(node -> nid);
    if (oct.getNeighboursInside(*node).size() < 3){
      //std::cout << "neighbours number : " << oct_copy.getNeighboursInside(*node).size() << std::endl;
      //oct.getNodeFromId(nod -> nid);
      //nod2 -> inside = true;
      nl.push_back(node);
      //std::cout << "xx" << std::endl;
    }
    for (cOctNode* node : oct.get_Inside()){
      node -> inside = true;
    }
  std::cout << "Node number : " << n << std::endl;
  n++;
  }
  std::cout << "number of inside after : " << oct.get_Inside().size() << std::endl;
  return oct;
}
