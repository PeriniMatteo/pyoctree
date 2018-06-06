#ifndef FILTERS
#define FILTERS

#include "booleans.h"

cOctree oct_neighbour_filter(cOctree &oct, int neighbours){
  std::cout << "number of inside before filter: " << oct.get_Inside().size() << std::endl;
  for (cOctNode* node : oct.get_Inside()){
    if (int(oct.getNeighboursInside(*node).size()) <= neighbours){
      node -> inside = false;
    }
  }
  std::cout << "number of inside after filter : " << oct.get_Inside().size() << std::endl;
  return oct;
}
#endif
